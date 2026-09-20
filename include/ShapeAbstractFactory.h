#ifndef SHAPEABSTRACTFACTORY_H
#define SHAPEABSTRACTFACTORY_H

#include <SFML/Graphics.hpp>
#include <memory>

// ====================================================================================
// ПАТТЕРН: ABSTRACT FACTORY (Абстрактная фабрика)
// ====================================================================================
// Применение: игра должна уметь выглядеть по-разному (темы classic и neon, к примеру),
// причем все части оформления в одной теме обязаны подходить друг к другу.
// Абстрактная фабрика создает СЕМЕЙСТВО связанных объектов, не привязывая клиентский код
// (main) к конкретным классам.
//
// Участники паттерна:
//  - AbstractProductA = BlockStyle (как рисуется блок фигуры)
//  - AbstractProductB = GridStyle  (как рисуется сетка/фон поля)
//  - AbstractFactory  = ShapeAbstractFactory (этот файл)
//  - ConcreteFactory  = ClassicThemeFactory, NeonThemeFactory (Themes.h)
//  - ConcreteProduct  = классы в Themes.cpp (клиенту не видны)
// ====================================================================================

// Абстрактный продукт A: стиль отрисовки одного блока.
class BlockStyle
{
public:
    virtual ~BlockStyle() = default;
    virtual void Draw(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const = 0;
};

// Абстрактный продукт B: стиль игрового поля (фон и сетка).
class GridStyle
{
public:
    virtual ~GridStyle() = default;
    virtual sf::Color BackgroundColor() const = 0;
    virtual void Draw(sf::RenderTarget& target) const = 0;
};

// Абстрактная фабрика: умеет создавать оба продукта одной темы.
class ShapeAbstractFactory
{
public: 
    virtual ~ShapeAbstractFactory() = default;
    virtual std::unique_ptr<BlockStyle> CreateBlockStyle() const = 0;
    virtual std::unique_ptr<GridStyle> CreateGridStyle() const = 0;
    virtual const char* Name() const = 0;
};

#endif // SHAPEABSTRACTFACTORY_H