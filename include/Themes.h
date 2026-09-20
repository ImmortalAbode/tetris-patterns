#ifndef THEMES_H
#define THEMES_H

#include "ShapeAbstractFactory.h"

// Конкретные фабрики (ConcreteFactory). Сами продукты спрятаны в Themes.cpp:
// клиенту достаточно знать только абстрактные BlockStyle и GridStyle.

class ClassicThemeFactory : public ShapeAbstractFactory
{
public:
    std::unique_ptr<BlockStyle> CreateBlockStyle() const override;
    std::unique_ptr<GridStyle> CreateGridStyle() const override;
    const char* Name() const override
    {
        return "Classic";
    }
};

class NeonThemeFactory : public ShapeAbstractFactory
{
public:
    std::unique_ptr<BlockStyle> CreateBlockStyle() const override;
    std::unique_ptr<GridStyle> CreateGridStyle() const override;
    const char* Name() const override 
    {
        return "Neon";
    }
};

#endif //THEMES_H