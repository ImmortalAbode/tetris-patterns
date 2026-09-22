#include "Config.h"
#include "GameBoard.h"
#include "IInputHandler.h"
#include "KeyboardInputAdapter.h"
#include "PieceFactories.h"
#include "ReportGenerators.h"
#include "ScoreManager.h"
#include "ShapeAbstractFactory.h"
#include "Tetromino.h"
#include "Themes.h"

// ----------------- Логика ----------------
// Сдвинуть фигуру по горизонтали; если уперлись - вернуть назад.
void TryMove(Tetromino& piece, int dx)
{
    piece.Move(dx, 0);
    if (GameBoard::Instance().Collides(piece))
        piece.Move(-dx, 0);
}

// Повернуть фигуру; если после поворота места нет - отменить.
void RotatePiece(Tetromino& piece)
{
    Tetromino backup = piece;
    piece.RotateClockwise();
    if (GameBoard::Instance().Collides(piece))
        piece = backup;
}

// Сохранить отчет об игре во всех трех представлениях. main здесь работает только
// с конкретными генераторами (ReportGenerators.h) и ни разу не упоминает конкретный
// строитель или то, как именно отчет доставляется (консоль/файл/csv) - это решает
// каждый генератор сам внутри своего CreateBuilder() и Save().
void SaveGameReport()
{
    TextReportGenerator().Generate();
    HtmlReportGenerator().Generate();
    CsvReportGenerator().Generate();
}

// Сдвинуть фигуру вниз на 1; если не получилось - зафиксировать ее и создать новую.
// К полю и счету обращаемся через Singleton: передавать их параметрами не нужно.
// Фигуры дает factory - какой именно фабричный метод вызовется (Normal или Sprint),
// решили один раз при запуске.
void MoveDown(std::unique_ptr<Tetromino>& piece, PieceFactory& factory)
{
    GameBoard& board = GameBoard::Instance();

    piece->Move(0, 1);
    if (!board.Collides(*piece))
        return;
    piece->Move(0, -1);

    board.Lock(*piece);
    ScoreManager::Instance().AddPiece(piece->ColorIndex());
    int cleared = board.ClearLines();
    if (cleared > 0)
        ScoreManager::Instance().AddLines(cleared);

    piece = factory.CreatePiece();
    if (board.Collides(*piece))     // новая фигура сразу уперлась - игра окончена.
    {
        SaveGameReport();
        board.Reset();              // рестарт
        ScoreManager::Instance().Reset();
    }
}

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
    const float fallInterval = sprintMode ? 0.25f : 0.5f;   // секунд между автоматическими шагами вниз
    ScoreManager::Instance().SetMode(modeName);             // для отчета (Text/Html/Csv) - см. BuildHeader()

    sf::RenderWindow window(sf::VideoMode({COLS * CELL, ROWS * CELL}), "Tetris");
    window.setFramerateLimit(60);

    // --- Abstract Factory + Prototype + Singleton (реестр) ---
    // Темы регистрируют свои фабрики в реестре. Instance() возвращает активную из них
    // (стартовая тема - из переменной окружения TETRIS_THEME, по умолчанию Classic).
    RegisterThemes();

    // --- Adapter: main работает только с IInputHandler, не с SFML-событиями ---
    std::unique_ptr<IInputHandler> inputHandler = std::make_unique<KeyboardInputAdapter>(window);
    std::unique_ptr<BlockStyle> blockStyle = ShapeAbstractFactory::Instance().CreateBlockStyle();
    std::unique_ptr<GridStyle> gridStyle = ShapeAbstractFactory::Instance().CreateGridStyle();

    // Заголовок окна: тема и счет. shownScore - какой счет сейчас показан в заголовке.
    int shownScore{0};
    auto updateTitle = [&]()
    {
        shownScore = ScoreManager::Instance().Score();
        window.setTitle(std::string("Tetris | Theme: ") + ShapeAbstractFactory::Instance().Name()
            + " | Mode: " + modeName
            + " | Score: " + std::to_string(shownScore)
            + " | Lines: " + std::to_string(ScoreManager::Instance().Lines())
            + " (T - switch)");

    };
    updateTitle();

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
            switch (action)
            {
                case InputAction::Quit:
                    window.close();
                    break;
                case InputAction::MoveLeft:
                    TryMove(*piece, -1);
                    break;
                case InputAction::MoveRight:
                    TryMove(*piece, 1);
                    break;
                case InputAction::SoftDrop:
                    MoveDown(piece, *pieceFactory);
                    break;
                case InputAction::RotateCW:
                    RotatePiece(*piece);
                    break;
                case InputAction::SwitchTheme:
                    // Смена темы: активной становится следующая фабрика из реестра,
                    // у нее просим новое семейство продуктов (Instance() уже другая).
                    ShapeAbstractFactory::SelectNext();
                    blockStyle = ShapeAbstractFactory::Instance().CreateBlockStyle();
                    gridStyle = ShapeAbstractFactory::Instance().CreateGridStyle();
                    updateTitle();
                    break;
                case InputAction::None:
                    break;
            }
        }

        // --- гравитация ---
        if (fallClock.getElapsedTime().asSeconds() >= fallInterval)
        {
            MoveDown(piece, *pieceFactory);
            fallClock.restart();
        }

        // Если счет изменился (убрали линии или рестарт) - обновляем заголовок.
        if (ScoreManager::Instance().Score() != shownScore)
            updateTitle();

        // --- рисование ---
        window.clear(gridStyle->BackgroundColor());
        gridStyle->Draw(window);    // фон и пустые клетки

        const GameBoard& board = GameBoard::Instance();
        for (int r{0}; r < ROWS; ++r)
            for (int c{0}; c < COLS; ++c)
                if (board.Get(r, c))
                    blockStyle->Draw(window, c, r, board.Get(r, c));

        // Composite: раньше здесь был такой же двойной цикл, как для доски выше
        // (перебор size x size клеток с проверкой IsFilled). Теперь Tetromino сам
        // знает, из каких блоков состоит, и рисует себя одним вызовом.
        piece->Draw(window, *blockStyle);

        window.display();
    }

    return 0;
}