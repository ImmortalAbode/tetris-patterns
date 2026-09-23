#include "CollisionHandlers.h"

#include "Config.h"
#include "GameBoard.h"

// Стены: у любого блока фигуры столбец вышел за [0, COLS).
bool WallCollisionHandler::CheckCollision(const Tetromino& piece) const
{
    for (const Block& block : piece.Blocks())
        if (block.Col() < 0 || block.Col() >= COLS)
            return true;
    return false;
}

// Пол: у любого блока фигуры строка достигла или превысила ROWS.
// Строки выше поля (block.Row() < 0, во время спавна) не считаются полом.
bool FloorCollisionHandler::CheckCollision(const Tetromino& piece) const
{
    for (const Block& block : piece.Blocks())
        if (block.Row() >= ROWS)
            return true;
    return false;
}

// Осевшие блоки: этот обработчик выполняется, только если стены и пол уже
// подтвердили допустимость хода, поэтому Col() и Row() < ROWS гарантированно
// в границах поля. Row() < 0 (фигура еще выше поля) отдельно пропускаем -
// GameBoard::Get с отрицательной строкой обратился бы за пределы массива.
bool BlockCollisionHandler::CheckCollision(const Tetromino& piece) const
{
    const GameBoard& board = GameBoard::Instance();
    for (const Block& block : piece.Blocks())
        if (block.Row() >= 0 && board.Get(block.Row(), block.Col()))
            return true;
    return false;
}

std::unique_ptr<CollisionHandler> BuildCollisionChain()
{
    auto wall = std::make_unique<WallCollisionHandler>();
    auto floor = std::make_unique<FloorCollisionHandler>();
    auto block = std::make_unique<BlockCollisionHandler>();

    floor->SetSuccessor(std::move(block));
    wall->SetSuccessor(std::move(floor));
    return wall;
}