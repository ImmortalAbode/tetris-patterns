#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>

#include "ShapeAbstractFactory.h"   // BlockStyle

// =====================================================================
// ПАТТЕРН: COMPOSITE (Компоновщик)
// =====================================================================
// Зачем здесь: падающая фигура (Tetromino) состоит из нескольких клеток
// (Block), и клиенту (GameBoard, main) не должно быть важно, работает ли
// он с одной клеткой или с целой фигурой - у обоих один и тот же
// интерфейс: "нарисуй себя" и "занимаешь ли ты клетку поля (col, row)".
// Раньше фигура хранилась как фиксированный bool[4][4] 
// - Composite заменяет это деревом объектов.
//
// Задание методички (лаба 5) - линии/окружности/прямоугольники с проверкой
// "точка входит в область" - переносим на клетки игрового поля: Occupies
// играет ту же роль, что в методичке играет проверка "точка входит в
// область фигуры".
//
// Участники:
//   - Component (GameObject)      - этот файл
//   - Leaf (Block)                - одна клетка (Block.h)
//   - Composite (Tetromino)       - фигура из клеток (Tetromino.h)
//   - Client (GameBoard, main)    - работает только с GameObject/Tetromino,
//     не знает, что фигура внутри - список Block
// =====================================================================
class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Draw(sf::RenderTarget& target, const BlockStyle& style) const = 0;

    // То же дерево, но в режиме "призрака" (см. BlockStyle::DrawGhost) - для
    // проекции падающей фигуры на дно поля.
    virtual void DrawGhost(sf::RenderTarget& target, const BlockStyle& style) const = 0;

    // Занимает ли объект клетку поля (col, row)? Используется для проверки коллизий.
    virtual bool Occupies(int col, int row) const = 0;
};

#endif // GAMEOBJECT_H