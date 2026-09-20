#include "Themes.h"
#include "Config.h"

// Конкретные продукты двух тем. Лежат в анонимном namespace, то есть видны только
// в этом файле: снаружи их можно получить лишь через фабрики.
namespace
{
    // Индекс 0 не используется (пустые клетки рисует GridStyle), 1..7 - I, O, T, S, Z, J, L.
    const sf::Color CLASSIC_COLORS[8] = {
        sf::Color(0, 0, 0),
        sf::Color(0, 255, 255),
        sf::Color(255, 255, 0),
        sf::Color(170, 0, 255),
        sf::Color(0, 255, 0),
        sf::Color(255, 0, 0),
        sf::Color(0, 0, 255),
        sf::Color(255, 165, 0),
    };

    const sf::Color NEON_COLORS[8] = {
        sf::Color(0, 0, 0),
        sf::Color(0, 255, 255),
        sf::Color(255, 255, 0),
        sf::Color(255, 0, 255),
        sf::Color(57, 255, 20),
        sf::Color(255, 20, 60),
        sf::Color(30, 144, 255),
        sf::Color(255, 140, 0),
    };

    // -------- Тема Classic --------

    // Сплошной цветной квадрат.
    class ClassicBlockStyle : public BlockStyle
    {
    public:
        void Draw(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setPosition(sf::Vector2f(cellX * CELL, cellY * CELL));
            rect.setFillColor(CLASSIC_COLORS[colorIndex]);
            target.draw(rect);
        }
    };

    // Черный фон и темно-серые пустые клетки.
    class ClassicGridStyle : public GridStyle 
    {
    public:
        sf::Color BackgroundColor() const override
        {
            return sf::Color::Black;
        }

        void Draw(sf::RenderTarget& target) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setFillColor(sf::Color(30, 30, 30));
            for (int r{0}; r < ROWS; ++r)
            {
                for (int c{0}; c < COLS; ++c)
                {
                    rect.setPosition(sf::Vector2f(c * CELL, r * CELL));
                    target.draw(rect);
                }
            }
        }
    };

    // ---------- Тема Neon ----------

    // Темный блок с яркой светящейся рамкой.
    class NeonBlockStyle : public BlockStyle
    {
    public:
        void Draw(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const override
        {
            const sf::Color& color = NEON_COLORS[colorIndex];
            sf::RectangleShape rect(sf::Vector2f(CELL - 8, CELL - 8));
            rect.setPosition(sf::Vector2f(cellX * CELL + 4, cellY * CELL + 4));
            rect.setFillColor(sf::Color(color.r / 5, color.g / 5, color.b / 5));
            rect.setOutlineThickness(3.f);
            rect.setOutlineColor(color);
            target.draw(rect);
        }
    };

    // Темно-синий фон и тонкие линии сетки.
    class NeonGridStyle : public GridStyle
    {
    public:
        sf::Color BackgroundColor() const override 
        {
            return sf::Color(5, 5, 20);
        }

        void Draw(sf::RenderTarget& target) const override
        {
            sf::RectangleShape line;
            line.setFillColor(sf::Color(0, 70, 90));

            for (int c{0}; c <= COLS; ++c)
            {
                int x{ (c == COLS) ? c * CELL - 1 : c * CELL }; // последняя линия внутри окна
                line.setSize(sf::Vector2f(1, ROWS * CELL));
                line.setPosition(sf::Vector2f(x, 0));
                target.draw(line);
            }
            for (int r{0}; r <= ROWS; ++r)
            {
                int y{ (r == ROWS) ? r * CELL - 1 : r * CELL };
                line.setSize(sf::Vector2f(COLS * CELL, 1));
                line.setPosition(sf::Vector2f(0, y));
                target.draw(line);
            }
        }
    };
} // namespace

// --------- Конкретные фабрики ---------
// Каждый метод Create... по сути фабричный метод: возвращает продукт нужной темы.

std::unique_ptr<BlockStyle> ClassicThemeFactory::CreateBlockStyle() const
{
    return std::make_unique<ClassicBlockStyle>();
}

std::unique_ptr<GridStyle> ClassicThemeFactory::CreateGridStyle() const 
{
    return std::make_unique<ClassicGridStyle>();
}

std::unique_ptr<BlockStyle> NeonThemeFactory::CreateBlockStyle() const
{
    return std::make_unique<NeonBlockStyle>();
}

std::unique_ptr<GridStyle> NeonThemeFactory::CreateGridStyle() const 
{
    return std::make_unique<NeonGridStyle>();
}