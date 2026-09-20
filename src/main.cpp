// Шаг 1: "Наивный" Тетрис без паттернов. Все в одном файле, глобальные переменные,
// никаких классов. Дальше будет рефакторинг этого под паттерны.

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cstring>
#include <ctime>

// --------------- Константы ---------------
constexpr int COLS{ 10 }; // ширина поля в клетках
constexpr int ROWS{ 20 }; // высота поля в клетках
constexpr int CELL{ 30 }; // размер клетки в пикселях

// Цвета: индекс 0 - пустая клетка, 1..7 - цвета фигур.
const sf::Color COLORS[8] = {
    sf::Color(30, 30, 30),  // пусто
    sf::Color(0, 255, 255), // I
    sf::Color(255, 255, 0), // O
    sf::Color(170, 0, 255), // T
    sf::Color(0, 255, 0),   // S
    sf::Color(255, 0, 0),   // Z
    sf::Color(0, 0, 255),   // J
    sf::Color(255, 165, 0), // L
};

// ------------ Описание фигур -------------
// size - сторона квадрата size x size, в котором лежит фигура (в нем же она вращается).
// cells - клетки построчно, 'X' = занято, '.' - пусто.
struct ShapeDef 
{
    int size{};
    const char* cells{};
};

const ShapeDef SHAPES[7] = {
    {4, "....XXXX........"}, // I: ..../XXXX/..../....
    {2, "XXXX"},             // O: XX/XX
    {3, ".X.XXX..."},        // T: .X./XXX/...
    {3, ".XXXX...."},        // S: .XX/XX./...
    {3, "XX..XX..."},        // Z: XX./.XX/...
    {3, "X..XXX..."},        // J: X../XXX/... 
    {3, "..XXXX..."}         // L: ..X/XXX/...
};

// ------ Глобальное состояние игры -------
int board[ROWS][COLS]{};    // 0 - пусто, 1..7 - цвет осевшей клетки

int piece[4][4]{};      // клетки текущей фигуры (1 - занято)
int pieceSize{};        // сторона квадрата текущей фигуры
int pieceX{}, pieceY{}; // позиция левого верхнего угла квадрата фигуры на поле
int pieceColor{};       // индекс цвета текущей фигуры

// ----------------- Логика ----------------
// Пересекается ли текущая фигура, смещенная в (x, y), со стенами, полом или осевшими клетками.
bool collides(int x, int y)
{
    for (int r{0}; r < pieceSize; ++r)
    {
        for (int c{0}; c < pieceSize; ++c) 
        {
            if (!piece[r][c]) 
                continue;
            int bx{ x + c};
            int by{ y + r};
            // Стены и пол.
            if (bx < 0 || bx >= COLS || by >= ROWS)
                return true;
            // Другие блоки (осевшие).
            if (by >= 0 && board[by][bx])
                return true;
        }
    }
    return false;
}

// Создать новую случайную фигуру вверху поля.
void spawnPiece()
{
    int type = std::rand() % 7;
    pieceSize = SHAPES[type].size;
    pieceColor = type + 1;

    std::memset(piece, 0, sizeof(piece)); // обнуление
    for (int r{0}; r < pieceSize; ++r)
        for (int c{0}; c < pieceSize; ++c)
            piece[r][c] = (SHAPES[type].cells[r * pieceSize + c] == 'X') ? 1 : 0;
    
    pieceX = COLS / 2 - pieceSize / 2;
    pieceY = 0;
}

// Повернуть фигуру на 90° по часовой стрелке;
// если после поворота места нет - отменить.
void rotatePiece()
{
    int old[4][4]{};
    std::memcpy(old, piece, sizeof(old));

    for (int r{0}; r < pieceSize; ++r)
        for (int c{0}; c < pieceSize; ++c)
            piece[r][c] = old[pieceSize - 1 - c][r];

    if (collides(pieceX, pieceY))
        std::memcpy(piece, old, sizeof(piece));
}

// Вписать текущую фигуру в поле (она "приземлилась").
void lockPiece()
{
    for (int r{0}; r < pieceSize; ++r)
        for (int c{0}; c < pieceSize; ++c)
            if (piece[r][c])
                board[pieceY + r][pieceX + c] = pieceColor;
}

// Удалить заполненные строки: все, что выше, сдвигается вниз.
void clearLines()
{
    int r{ ROWS - 1};
    while (r >= 0)
    {
        bool full = true;
        for (int c = 0; c < COLS; ++c)
            if (!board[r][c])
                full = false;

        if (full)
        {
            for (int rr{r}; rr > 0; --rr)
                for (int c{0}; c < COLS; ++c)
                    board[rr][c] = board[rr - 1][c];
            for (int c{0}; c < COLS; ++c)
                board[0][c] = 0;
            // r не уменьшается: на этом месте теперь другая строка, ее тоже нужно проверить.
        }
        else
        {
            --r;
        }
    }
}

// Сдвинуть фигуру вниз на 1; если не получилось - зафиксировать ее и создать новую.
void moveDown()
{
    if (!collides(pieceX, pieceY + 1))
    {
        ++pieceY;
        return;
    }
    lockPiece();
    clearLines();
    spawnPiece();
    if (collides(pieceX, pieceY))               // новая фигура сразу уперлась - игра окончена.
        std::memset(board, 0, sizeof(board));   // рестарт
}

// --------------- Отрисовка ---------------
void drawCell(sf::RenderWindow& window, int x, int y, int colorIndex)
{
    sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
    rect.setPosition(sf::Vector2f(x * CELL, y * CELL));
    rect.setFillColor(COLORS[colorIndex]);
    window.draw(rect);
}

int main() 
{
    sf::RenderWindow window(sf::VideoMode({COLS * CELL, ROWS * CELL}), "Tetris");
    window.setFramerateLimit(60);

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    spawnPiece();

    sf::Clock fallClock{};
    const float fallInterval = 0.5f;    // секунд между автоматическими шагами вниз

    while (window.isOpen())
    {
        // --- ввод ---
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Left && !collides(pieceX - 1, pieceY))
                    --pieceX;
                else if (key->code == sf::Keyboard::Key::Right && !collides(pieceX + 1, pieceY))
                    ++pieceX;
                else if (key->code == sf::Keyboard::Key::Down)
                    moveDown();
                else if (key->code == sf::Keyboard::Key::Up)
                    rotatePiece();
            }
        }

        // --- гравитация ---
        if (fallClock.getElapsedTime().asSeconds() >= fallInterval)
        {
            moveDown();
            fallClock.restart();
        }

        // --- рисование ---
        window.clear(sf::Color::Black);

        for (int r{0}; r < ROWS; ++r)
            for (int c{0}; c < COLS; ++c)
                drawCell(window, c, r, board[r][c]);

        for (int r{0}; r < pieceSize; ++r)
            for (int c{0}; c < pieceSize; ++c)
                if (piece[r][c])
                    drawCell(window, pieceX + c, pieceY + r, pieceColor);

        window.display();
    }

    return 0;
}
