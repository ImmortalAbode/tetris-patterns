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

    // --- Command: история для отмены последнего игрового хода ---
    CommandHistory history;

    // Заголовок окна: тема и счет. shownScore - какой счет сейчас показан в заголовке.
    int shownScore{0};
    auto updateTitle = [&]()
    {
        shownScore = ScoreManager::Instance().Score();
        window.setTitle(std::string("Tetris | Theme: ") + ShapeAbstractFactory::Instance().Name()
            + " | Mode: " + modeName
            + " | Score: " + std::to_string(shownScore)
            + " | Lines: " + std::to_string(ScoreManager::Instance().Lines())
            + " (T - switch, Backspace - undo)");

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
                    updateTitle();
                    break;
                case InputAction::None:
                    break;
            }
        }

        // --- гравитация ---
        // Автоматическое падение - не игровое действие игрока, поэтому идет
        // напрямую через GameActions, минуя команды и историю (его не отменить).
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