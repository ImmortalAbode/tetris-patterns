#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "CommandHistory.h"
#include "Commands.h"
#include "Config.h"
#include "GameActions.h"
#include "GameBoard.h"
#include "IInputHandler.h"
#include "KeyboardInputAdapter.h"
#include "PieceFactories.h"
#include "ScoreManager.h"
#include "ShapeAbstractFactory.h"
#include "Tetromino.h"
#include "Themes.h"

namespace
{
    // sf::Text/sf::String трактуют const char*/std::string как Latin-1, а не
    // UTF-8 - кириллические литералы (исходники в UTF-8) без этой обертки
    // превращаются в мусор. fromUtf8() декодирует байты правильно.
    sf::String Utf8(const std::string& text)
    {
        return sf::String::fromUtf8(text.begin(), text.end());
    }
} // namespace

int main(int argc, char** argv)
{
    // --- Factory Method: режим выбирает конкретную фабрику фигур ---
    bool sprintMode = (argc > 1) && (std::string(argv[1]) == "sprint");

    std::unique_ptr<PieceFactory> pieceFactory;
    const char* modeName;
    if (sprintMode)
    {
        pieceFactory = std::make_unique<SprintPieceFactory>();
        modeName = "Sprint";
    }
    else
    {
        pieceFactory = std::make_unique<NormalPieceFactory>();
        modeName = "Normal";
    }
    const float baseFallInterval = sprintMode ? 0.25f : 0.5f;   // секунд между автоматическими шагами вниз на 1 уровне
    ScoreManager::Instance().SetMode(modeName);             // для отчета (Text/Html/Csv) - см. BuildHeader()

    sf::RenderWindow window(sf::VideoMode({COLS * CELL + SIDEBAR_WIDTH, ROWS * CELL}), "Tetris");
    window.setFramerateLimit(60);

    // Шрифт для HUD-панели (счет/тема/режим/превью). Без шрифта sf::Text
    // в SFML 3 создать нельзя (конструктору нужна ссылка на Font), поэтому
    // при неудаче загрузки дальше двигаться некуда.
    sf::Font font;
    if (!font.openFromFile(FONT_PATH))
    {
        std::cerr << "Не удалось загрузить шрифт: " << FONT_PATH << "\n";
        return 1;
    }
    // --- HUD-панель: подложка, подписи (мелкие, тусклые) и значения (крупные) ---
    sf::RectangleShape sidebarBg(sf::Vector2f(static_cast<float>(SIDEBAR_WIDTH), static_cast<float>(ROWS * CELL)));
    sidebarBg.setPosition(sf::Vector2f(static_cast<float>(COLS * CELL), 0.f));
    sidebarBg.setFillColor(sf::Color(24, 24, 30));

    const float panelX{ COLS * CELL + 24.f };
    const sf::Color labelColor{ 150, 150, 160 };

    sf::Text labelNext(font, Utf8("Следующая фигура"), 16);
    labelNext.setFillColor(labelColor);
    labelNext.setPosition(sf::Vector2f(panelX, 20.f));

    // Фоновые клетки окошка превью - играют ту же роль, что GridStyle для
    // всего поля (показать пустые клетки), только не привязаны к теме: сама
    // фигура превью рисуется через blockStyle->Draw() ниже, в цикле отрисовки,
    // и вот она уже одевается в текущую тему автоматически.
    std::vector<sf::RectangleShape> previewCells;
    for (int r{0}; r < PREVIEW_ROWS; ++r)
    {
        for (int c{0}; c < PREVIEW_COLS; ++c)
        {
            sf::RectangleShape cell(sf::Vector2f(CELL - 4.f, CELL - 4.f));
            cell.setPosition(sf::Vector2f(
                (PREVIEW_ANCHOR_COL + c) * CELL, (PREVIEW_ANCHOR_ROW + r) * CELL));
            cell.setFillColor(sf::Color(12, 12, 16));
            previewCells.push_back(cell);
        }
    }

    const float statsY{ (PREVIEW_ANCHOR_ROW + PREVIEW_ROWS) * CELL + 28.f };
    const float groupGap{ 64.f };

    sf::Text labelScore(font, Utf8("Счёт"), 16);
    labelScore.setFillColor(labelColor);
    labelScore.setPosition(sf::Vector2f(panelX, statsY));
    sf::Text valueScore(font, "", 34);
    valueScore.setFillColor(sf::Color::White);
    valueScore.setPosition(sf::Vector2f(panelX, statsY + 24.f));

    sf::Text labelLevel(font, Utf8("Уровень"), 16);
    labelLevel.setFillColor(labelColor);
    labelLevel.setPosition(sf::Vector2f(panelX, statsY + groupGap));
    sf::Text valueLevel(font, "", 34);
    valueLevel.setFillColor(sf::Color::White);
    valueLevel.setPosition(sf::Vector2f(panelX, statsY + groupGap + 24.f));

    sf::Text labelLines(font, Utf8("Линии"), 16);
    labelLines.setFillColor(labelColor);
    labelLines.setPosition(sf::Vector2f(panelX, statsY + 2 * groupGap));
    sf::Text valueLines(font, "", 34);
    valueLines.setFillColor(sf::Color::White);
    valueLines.setPosition(sf::Vector2f(panelX, statsY + 2 * groupGap + 24.f));

    // Тема/режим и подсказки по клавишам - мелким текстом внизу панели
    // (раньше это была строка заголовка окна). Каждая часть - на своей
    // строке: при ширине панели в SIDEBAR_COLS клеток длинная строка вроде
    // "T - тема, Backspace - отмена" не влезает и обрезается краем окна.
    sf::Text hint(font, "", 13);
    hint.setFillColor(labelColor);
    hint.setPosition(sf::Vector2f(panelX, ROWS * CELL - 120.f));

    // Центрирует текст (по его собственным границам) вокруг точки (x, y) -
    // общий помощник для экранов паузы и Game Over, у которых текст всегда
    // должен быть посреди поля, а не в его левом верхнем углу, как у HUD.
    auto centerText = [](sf::Text& text, float x, float y)
    {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.getCenter());
        text.setPosition(sf::Vector2f(x, y));
    };

    // --- Экран паузы: полупрозрачная подложка на все поле + надпись ---
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(COLS * CELL), static_cast<float>(ROWS * CELL)));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));

    sf::Text pauseTitle(font, Utf8("ПАУЗА"), 40);
    pauseTitle.setFillColor(sf::Color::White);
    centerText(pauseTitle, COLS * CELL / 2.f, ROWS * CELL / 2.f);

    // --- Экран Game Over: та же подложка + заголовок, итоги, подсказка ---
    // Итоги (gameOverStats) заполняются один раз в момент проигрыша (см.
    // ниже, в игровом цикле) - там же известен финальный счет.
    sf::Text gameOverTitle(font, Utf8("ИГРА ОКОНЧЕНА"), 36);
    gameOverTitle.setFillColor(sf::Color::White);
    centerText(gameOverTitle, COLS * CELL / 2.f, ROWS * CELL / 2.f - 100.f);

    sf::Text gameOverStats(font, "", 20);
    gameOverStats.setFillColor(labelColor);

    sf::Text gameOverHint(font, Utf8("R - начать заново"), 16);
    gameOverHint.setFillColor(labelColor);
    centerText(gameOverHint, COLS * CELL / 2.f, ROWS * CELL / 2.f + 110.f);

    bool paused{false};
    bool gameOver{false};

    // --- Abstract Factory + Prototype + Singleton (реестр) ---
    // Темы регистрируют свои фабрики в реестре. Instance() возвращает активную из них
    // (стартовая тема - из переменной окружения TETRIS_THEME, по умолчанию Classic).
    RegisterThemes();

    // --- Adapter: main работает только с IInputHandler, не с SFML-событиями ---
    std::unique_ptr<IInputHandler> inputHandler = std::make_unique<KeyboardInputAdapter>(window);
    std::unique_ptr<BlockStyle> blockStyle = ShapeAbstractFactory::Instance().CreateBlockStyle();
    std::unique_ptr<GridStyle> gridStyle = ShapeAbstractFactory::Instance().CreateGridStyle();

    // --- Command: история для отмены последнего игрового хода ---
    CommandHistory history;

    // shownScore - какой счет сейчас показан (чтобы не перестраивать текст
    // каждый кадр без нужды). Раньше вся эта информация шла в заголовок окна
    // текстом одной строкой - теперь заголовок окна просто "Tetris", а
    // подробности рисуются в самом окне через несколько sf::Text.
    int shownScore{0};
    auto updateHud = [&]()
    {
        shownScore = ScoreManager::Instance().Score();
        valueScore.setString(std::to_string(shownScore));
        valueLevel.setString(std::to_string(ScoreManager::Instance().Level()));
        valueLines.setString(std::to_string(ScoreManager::Instance().Lines()));
        hint.setString(Utf8(
            "Тема: " + std::string(ShapeAbstractFactory::Instance().Name()) + "\n"
            "Режим: " + modeName + "\n"
            "\n"
            "T - тема\n"
            "P - пауза\n"
            "Backspace - отмена"));
    };
    updateHud();

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::unique_ptr<Tetromino> piece = pieceFactory->CreatePiece();

    sf::Clock fallClock{};

    while (window.isOpen())
    {
        // --- ввод ---
        // main опрашивает действия через IInputHandler и не знает, что за ним
        // стоит SFML: тот же цикл работал бы и с любым другим адаптером ввода.
        InputAction action{InputAction::None};
        while ((action = inputHandler->PollAction()) != InputAction::None)
        {
            // На паузе и на экране Game Over игровые действия (движение,
            // поворот, падение, отмена) игнорируем - реагируем только на
            // Quit/SwitchTheme и на Pause/Restart (обрабатываются в switch).
            const bool frozen{ paused || gameOver };
            if (frozen
                && (action == InputAction::MoveLeft || action == InputAction::MoveRight
                    || action == InputAction::SoftDrop || action == InputAction::HardDrop
                    || action == InputAction::RotateCW || action == InputAction::Undo))
            {
                continue;
            }

            switch (action)
            {
                case InputAction::Quit:
                    window.close();
                    break;
                case InputAction::MoveLeft:
                    history.Execute(std::make_unique<MoveLeftCommand>(*piece));
                    break;
                case InputAction::MoveRight:
                    history.Execute(std::make_unique<MoveRightCommand>(*piece));
                    break;
                case InputAction::SoftDrop:
                    history.Execute(std::make_unique<DropCommand>(piece, *pieceFactory));
                    break;
                case InputAction::HardDrop:
                    // MacroCommand: собирается и выполняется пошагово внутри
                    // BuildHardDropCommand, поэтому здесь только запоминаем ее.
                    history.Record(BuildHardDropCommand(piece, *pieceFactory));
                    break;
                case InputAction::RotateCW:
                    history.Execute(std::make_unique<RotateCommand>(*piece));
                    break;
                case InputAction::Undo:
                    history.UndoLast();
                    break;
                case InputAction::SwitchTheme:
                    // Смена темы: активной становится следующая фабрика из реестра,
                    // у нее просим новое семейство продуктов (Instance() уже другая).
                    ShapeAbstractFactory::SelectNext();
                    blockStyle = ShapeAbstractFactory::Instance().CreateBlockStyle();
                    gridStyle = ShapeAbstractFactory::Instance().CreateGridStyle();
                    updateHud();
                    break;
                case InputAction::Pause:
                    if (!gameOver)
                    {
                        paused = !paused;
                        if (!paused)
                            fallClock.restart();   // не даем накопленному простою вызвать мгновенное падение
                    }
                    break;
                case InputAction::Restart:
                    if (gameOver)
                    {
                        GameBoard::Instance().Reset();
                        ScoreManager::Instance().Reset();
                        piece = pieceFactory->CreatePiece();
                        gameOver = false;
                        fallClock.restart();
                        updateHud();
                    }
                    break;
                case InputAction::None:
                    break;
            }
        }

        // --- гравитация ---
        // Автоматическое падение - не игровое действие игрока, поэтому идет
        // напрямую через GameActions, минуя команды и историю (его не отменить).
        // Скорость растет с уровнем (Level() = линии/10 + 1); минимум ограничен,
        // чтобы на высоких уровнях фигуры не падали быстрее, чем можно среагировать.
        const float fallInterval = std::max(0.08f,
            baseFallInterval - 0.03f * (ScoreManager::Instance().Level() - 1));

        if (!paused && !gameOver && fallClock.getElapsedTime().asSeconds() >= fallInterval)
        {
            MoveDown(piece, *pieceFactory);
            fallClock.restart();
        }

        // Игра окончена, если фигура, которую только что заспавнил MoveDown
        // (из гравитации выше или из команды падения в цикле ввода), сразу
        // не помещается - используем тот же GameBoard::Collides (Chain of
        // Responsibility), что и вся остальная игра, вместо отдельного
        // канала сигнала через Command/GameActions.
        if (!gameOver && GameBoard::Instance().Collides(*piece))
        {
            gameOver = true;
            SaveGameReport();

            gameOverStats.setString(Utf8(
                "Счёт: " + std::to_string(ScoreManager::Instance().Score()) + "\n"
                "Линии: " + std::to_string(ScoreManager::Instance().Lines()) + "\n"
                "Уровень: " + std::to_string(ScoreManager::Instance().Level())));
            centerText(gameOverStats, COLS * CELL / 2.f, ROWS * CELL / 2.f);
        }

        // Если счет изменился (убрали линии или рестарт) - обновляем HUD.
        if (ScoreManager::Instance().Score() != shownScore)
            updateHud();

        // --- рисование ---
        window.clear(gridStyle->BackgroundColor());
        gridStyle->Draw(window);    // фон и пустые клетки

        const GameBoard& board = GameBoard::Instance();
        for (int r{0}; r < ROWS; ++r)
            for (int c{0}; c < COLS; ++c)
                if (board.Get(r, c))
                    blockStyle->Draw(window, c, r, board.Get(r, c));

        // Пока не наступил Game Over, рисуем призрак и саму фигуру. После
        // Game Over piece формально "лежит" в клетке, где только что не
        // поместился - показывать его незачем, экран все равно закрыт оверлеем.
        if (!gameOver)
        {
            // Призрак: проекция фигуры на дно поля - клон, двигаемый вниз тем
            // же способом, что и MoveDown() (шаг + проверка через ту же цепочку
            // Chain of Responsibility + откат последнего шага). Рисуется первым,
            // чтобы настоящая фигура (если они совпадают - фигура уже у дна)
            // легла поверх него.
            Tetromino ghost = *piece;
            while (true)
            {
                ghost.Move(0, 1);
                if (board.Collides(ghost))
                {
                    ghost.Move(0, -1);
                    break;
                }
            }
            ghost.DrawGhost(window, *blockStyle);

            // Composite: раньше здесь был такой же двойной цикл, как для доски
            // выше (перебор size x size клеток с проверкой IsFilled). Теперь
            // Tetromino сам знает, из каких блоков состоит, и рисует себя одним
            // вызовом.
            piece->Draw(window, *blockStyle);
        }

        // --- HUD-панель ---
        window.draw(sidebarBg);
        window.draw(labelNext);
        for (const sf::RectangleShape& cell : previewCells)
            window.draw(cell);
        window.draw(labelScore);
        window.draw(valueScore);
        window.draw(labelLevel);
        window.draw(valueLevel);
        window.draw(labelLines);
        window.draw(valueLines);
        window.draw(hint);

        // Превью следующей фигуры: тот же PeekNext() (Factory Method), что и
        // CreatePiece(), только не выбирает фигуру окончательно. Рисуется тем
        // же BlockStyle::Draw, что и все остальные блоки, поэтому подхватывает
        // текущую тему автоматически. Фигура хранится в квадрате size x size,
        // а занятые клетки внутри него могут начинаться не с нулевой строки/
        // колонки (пример - "I": единственная занятая строка - вторая из
        // четырех), поэтому перед показом ищем реальный левый верхний угол
        // (minRow/minCol) и сдвигаем фигуру так, чтобы он лег точно в угол
        // окошка превью - иначе такие фигуры "плавали" бы внутри окошка.
        std::unique_ptr<Tetromino> preview = pieceFactory->PeekNext();
        int minRow{std::numeric_limits<int>::max()};
        int minCol{std::numeric_limits<int>::max()};
        for (const Block& block : preview->Blocks())
        {
            minRow = std::min(minRow, block.Row());
            minCol = std::min(minCol, block.Col());
        }
        preview->SetPosition(PREVIEW_ANCHOR_COL - minCol, PREVIEW_ANCHOR_ROW - minRow);
        preview->Draw(window, *blockStyle);

        // --- оверлеи паузы и Game Over - поверх поля, взаимно исключающие ---
        if (paused)
        {
            window.draw(overlay);
            window.draw(pauseTitle);
        }
        else if (gameOver)
        {
            window.draw(overlay);
            window.draw(gameOverTitle);
            window.draw(gameOverStats);
            window.draw(gameOverHint);
        }

        window.display();
    }

    return 0;
}