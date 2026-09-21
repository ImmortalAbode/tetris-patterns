#ifndef SHAPEABSTRACTFACTORY_H
#define SHAPEABSTRACTFACTORY_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

// ===================================================================================
// ПАТТЕРНЫ: ABSTRACT FACTORY + SINGLETON (реестр одиночек) [+ PROTOTYPE]
// ===================================================================================
// Abstract Factory: игра должна уметь выглядеть по-разному (темы classic и neon),
// причем все части оформления в одной теме обязаны подходить друг к другу.
// ShapeAbstractFactory объявляет интерфейс создания СЕМЕЙСТВА связанных объектов
// (стиль блока + стиль сетки), не привязывая клиента (main) к конкретным классам.
//
// Singleton с реестром: у приложения одна фабрика на каждое семейство продуктов,
// и одна из них является активной. Фабрики регистрируют себя в реестре по имени
// (Register), а Instance() возвращает активную (Lookup по имени, начальное имя
// берется из переменной окружения TETRIS_THEME). Так Instance() не должна знать
// все возможные фабрики. Конструктор защищенный, копирование запрещено.
//
// Prototype: конкретная фабрика - ShapePrototypeFactory - параметризуется прототипами
// продуктов и создает их клонированием (см. ShapePrototypeFactory.h). Поэтому для 
// новой темы не нужен новый класс фабрики, достаточно нового набора прототипов.
//
// Как паттерны связаны между собой:
//  Prototype        -> определяет, ЧТО создает фабрика (клоны образцов темы);
//  Abstract Factory -> задает интерфейс создания согласованного семейства продуктов;
//  Singleton        -> гарантирует одну фабрику на семейство и единую точку доступа
//                      к активной фабрике; смена темы = смена активного одиночки.
//
// Участники:
//  - AbstractFactory           = ShapeAbstractFactory (этот файл)
//  - ConcreteFactory           = ShapePrototypeFactory (ShapePrototypeFactory.h)
//  - AbstractProduct/Prototype = BlockStyle, GridStyle (метод Clone())
//  - ConcreteProduct           = классы в Themes.cpp (клиенту не видны)
// ===================================================================================

// Абстрактный продукт А и прототип: стиль отрисовки одного блока.
class BlockStyle
{
public:
    virtual ~BlockStyle() = default;
    virtual std::unique_ptr<BlockStyle> Clone() const = 0;
    virtual void Draw(sf::RenderTarget& target, int cellX, int cellY, int colorIndex) const = 0;
};

// Абстрактный продукт B и прототип: стиль игрового поля (фон и сетка).
class GridStyle
{
public:
    virtual ~GridStyle() = default;
    virtual std::unique_ptr<GridStyle> Clone() const = 0;
    virtual sf::Color BackgroundColor() const = 0;
    virtual void Draw(sf::RenderTarget& target) const = 0;
};

// Абстрактная фабрика и одновременно одиночка с реестром.
class ShapeAbstractFactory
{
public:
    // --- Singleton с реестром ---

    // Активная фабрика. При первом вызове выбирается по имени из переменной
    // окружения TETRIS_THEME (по умолчанию Classic).
    static ShapeAbstractFactory& Instance();

    // Регистрация фабрики в реестре под ее собственным именем (Name()).
    static void Register(std::unique_ptr<ShapeAbstractFactory> factory);

    // Сделать активной следующую по порядку фабрику из реестра (смена темы).
    static void SelectNext();

    ShapeAbstractFactory(const ShapeAbstractFactory&) = delete;
    ShapeAbstractFactory& operator=(const ShapeAbstractFactory&) = delete;
    virtual ~ShapeAbstractFactory() = default;

    // --- Abstract Factory ---
    virtual std::unique_ptr<BlockStyle> CreateBlockStyle() const = 0;
    virtual std::unique_ptr<GridStyle> CreateGridStyle() const = 0;
    virtual const std::string& Name() const = 0;

protected:
    ShapeAbstractFactory() = default;

    // Поиск фабрики в реестре по имени (nullptr, если такой нет).
    static ShapeAbstractFactory* Lookup(const std::string& name);

private:
    static ShapeAbstractFactory* s_instance;    // активная фабрика
};

#endif // SHAPEABSTRACTFACTORY_H