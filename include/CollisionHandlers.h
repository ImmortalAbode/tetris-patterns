#ifndef COLLISIONHANDLERS_H
#define COLLISIONHANDLERS_H

#include "CollisionHandler.h"

// Конкретные обработчики (ConcreteHandler). Каждый отвечает только за одну
// причину столкновения и ничего не знает об остальных.

// Вышла ли фигура за левую или правую границу поля?
class WallCollisionHandler : public CollisionHandler
{
protected:
    bool CheckCollision(const Tetromino& piece) const override;
};

// Уперлась ли фигура в пол (нижнюю границу поля)?
class FloorCollisionHandler : public CollisionHandler
{
protected:
    bool CheckCollision(const Tetromino& piece) const override;
};

// Пересекается ли фигура с уже осевшими (зафиксированными) блоками?
class BlockCollisionHandler : public CollisionHandler
{
protected:
    bool CheckCollision(const Tetromino& piece) const override;
};

// Собирает цепочку WallCollisionHandler -> FloorCollisionHandler ->
// BlockCollisionHandler в готовом для использования виде.
std::unique_ptr<CollisionHandler> BuildCollisionChain();

#endif // COLLISIONHANDLERS_H