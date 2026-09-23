#include "Themes.h"
#include "Config.h"
#include "ShapePrototypeFactory.h"

// Конкретные прототипы двух тем (ConcretePrototype). Лежат в анонимном namespace,
// то есть видны только в этом файле: снаружи их можно получить лишь через фабрику,
// которая клонирует эти образцы.
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
        std::unique_ptr<BlockStyle> Clone() const override
        {
            return std::make_unique<ClassicBlockStyle>(*this);
        }

        void Draw(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setPosition(sf::Vector2f(cellX * CELL, cellY * CELL));
            rect.setFillColor(CLASSIC_COLORS[colorIndex]);
            target.draw(rect);
        }

        // Призрак: тот же квадрат, но без заливки - только полупрозрачный контур.
        void DrawGhost(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const override
        {
            const sf::Color& color = CLASSIC_COLORS[colorIndex];
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setPosition(sf::Vector2f(cellX * CELL, cellY * CELL));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineThickness(2.f);
            rect.setOutlineColor(sf::Color(color.r, color.g, color.b, 160));
            target.draw(rect);
        }
    };

    // Черный фон и темно-серые пустые клетки.
    class ClassicGridStyle : public GridStyle 
    {
    public:
        std::unique_ptr<GridStyle> Clone() const override
        {
            return std::make_unique<ClassicGridStyle>(*this);
        }

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
        std::unique_ptr<BlockStyle> Clone() const override
        {
            return std::make_unique<NeonBlockStyle>(*this);
        }

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

        // Призрак: без свечения и заливки - тонкий тусклый контур.
        void DrawGhost(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const override
        {
            const sf::Color& color = NEON_COLORS[colorIndex];
            sf::RectangleShape rect(sf::Vector2f(CELL - 8, CELL - 8));
            rect.setPosition(sf::Vector2f(cellX * CELL + 4, cellY * CELL + 4));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineThickness(2.f);
            rect.setOutlineColor(sf::Color(color.r, color.g, color.b, 90));
            target.draw(rect);
        }
    };

    // Темно-синий фон и тонкие линии сетки.
    class NeonGridStyle : public GridStyle
    {
    public:
        std::unique_ptr<GridStyle> Clone() const override
        {
            return std::make_unique<NeonGridStyle>(*this);
        }

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

    // ---------- Тема Retro ----------
    // Монохромный "фосфорный" зеленый, как на старых терминалах и ранних портативных
    // консолях (например, оригинальный Game Boy тоже рисовал Тетрис одним цветом).
    // В отличие от Classic и Neon, colorIndex здесь не используется для выбора цвета -
    // все фигуры одного оттенка зеленого, различать их приходится по форме, а не по цвету.
    const sf::Color RETRO_COLOR{60, 255, 60};
    const sf::Color RETRO_BACKGROUND{0, 10, 0};
    const sf::Color RETRO_EMPTY_CELL{0, 40, 0};

    // Сплошной монохромный зеленый квадрат.
    class RetroBlockStyle : public BlockStyle
    {
    public:
        std::unique_ptr<BlockStyle> Clone() const override
        {
            return std::make_unique<RetroBlockStyle>(*this);
        }

        void Draw(sf::RenderTarget& target, int cellX, int cellY, int /*colorIndex*/) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setPosition(sf::Vector2f(cellX * CELL, cellY * CELL));
            rect.setFillColor(RETRO_COLOR);
            target.draw(rect);
        }

        // Призрак: тусклый контур в том же фосфорном зеленом.
        void DrawGhost(sf::RenderTarget& target, int cellX, int cellY, int /*colorIndex*/) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setPosition(sf::Vector2f(cellX * CELL, cellY * CELL));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineThickness(2.f);
            rect.setOutlineColor(sf::Color(RETRO_COLOR.r, RETRO_COLOR.g, RETRO_COLOR.b, 120));
            target.draw(rect);
        }
    };

    // Почти черный фон с легким зеленым оттенком и темно-зелеными пустыми клетками.
    class RetroGridStyle : public GridStyle
    {
    public:
        std::unique_ptr<GridStyle> Clone() const override
        {
            return std::make_unique<RetroGridStyle>(*this);
        }

        sf::Color BackgroundColor() const override
        {
            return RETRO_BACKGROUND;
        }

        void Draw(sf::RenderTarget& target) const override
        {
            sf::RectangleShape rect(sf::Vector2f(CELL - 1, CELL - 1));
            rect.setFillColor(RETRO_EMPTY_CELL);
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
} // namespace

// --------- Темы = наборы прототипов ---------
// Здесь единственный раз создаются образцы (make_unique). Дальше фабрика только
// клонирует их. Одна тема - пара прототипов, подходящих друг другу; фабрика темы -
// объект ShapePrototypeFactory, зарегистрированный в реестре под именем темы.

void RegisterThemes()
{
    ShapeAbstractFactory::Register(std::make_unique<ShapePrototypeFactory>(
        "Classic",
        std::make_unique<ClassicBlockStyle>(),
        std::make_unique<ClassicGridStyle>()));

    ShapeAbstractFactory::Register(std::make_unique<ShapePrototypeFactory>(
        "Neon",
        std::make_unique<NeonBlockStyle>(),
        std::make_unique<NeonGridStyle>()));

    ShapeAbstractFactory::Register(std::make_unique<ShapePrototypeFactory>(
        "Retro",
        std::make_unique<RetroBlockStyle>(),
        std::make_unique<RetroGridStyle>()));
}