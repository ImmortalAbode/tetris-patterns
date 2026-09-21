#ifndef SHAPEPROTOTYPEFACTORY_H
#define SHAPEPROTOTYPEFACTORY_H

#include <string>
#include <utility>

#include "ShapeAbstractFactory.h"

// ====================================================================================
// ПАТТЕРН: PROTOTYPE (Прототип) - конкретная фабрика, параметризованная прототипами
// ====================================================================================
// Конкретная фабрика хранит по одному образцу (прототипу) каждого продукта своего
// семейства и создает новые продукты клонированием (Clone()). Прототипы передаются
// в конструктор. Благодаря этому:
//   - для каждой темы не нужен свой подкласс фабрики: Classic и Neon - это два объекта
//     одного класса с разными наборами прототипов;
//   - фабрика не знает конкретных классов продуктов, она вызывает только Clone();
//   - новую тему можно добавить, не меняя код фабрики.
//
// Участники:
//   - Prototype          = BlockStyle, GridStyle (интерфейс Clone())
//   - ConcretePrototype  = классы в Themes.cpp
//   - Client             = ShapePrototypeFactory: просит прототип клонировать себя
//
// Клонирование поверхностное (конструктор копирования по умолчанию): у наших стилей
// нет указателей на другие объекты, поэтому глубокое копирование не требуется.
// ====================================================================================
class ShapePrototypeFactory : public ShapeAbstractFactory
{
public:
    ShapePrototypeFactory(std::string name,
                          std::unique_ptr<BlockStyle> blockPrototype,
                          std::unique_ptr<GridStyle> gridPrototype)
        : m_name{std::move(name)},
          m_blockPrototype{std::move(blockPrototype)},
          m_gridPrototype{std::move(gridPrototype)}
    {
    }

    // Создание продукта = клон образца, а не new.
    std::unique_ptr<BlockStyle> CreateBlockStyle() const override
    {
        return m_blockPrototype->Clone();
    }

    std::unique_ptr<GridStyle> CreateGridStyle() const override
    {
        return m_gridPrototype->Clone();
    }

    const std::string& Name() const override
    {
        return m_name;
    }

private:
    std::string m_name;
    std::unique_ptr<BlockStyle> m_blockPrototype;
    std::unique_ptr<GridStyle> m_gridPrototype;
};

#endif // SHAPEPROTOTYPEFACTORY_H