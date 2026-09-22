#ifndef PIECEFACTORIES_H
#define PIECEFACTORIES_H

#include <array>
#include <cstddef>

#include "PieceFactory.h"

// Конкретные создатели (ConcreteCreator). Прототипы фигур (7 штук) спрятаны
// в PieceFactories.cpp - клиенту (main) они не видны, только эти два класса.

// Обычный режим: каждая следующая фигура выбирается равномерно случайно из
// 7 видов, независимо от предыдущих (один и тот же вид теоретически может
// выпасть много раз подряд).
class NormalPieceFactory : public PieceFactory
{
public:
    std::unique_ptr<Tetromino> CreatePiece() override;
};

// Спринт: "мешок из 7" (как в современных Тетрисах) - все 7 видов фигур
// выпадают по одному разу в случайном порядке, затем мешок перемешивается
// заново. Гарантирует, что за 7 фигур встретятся все виды.
class SprintPieceFactory : public PieceFactory
{
public:
    SprintPieceFactory();

    std::unique_ptr<Tetromino> CreatePiece() override;

private:
    void RefillBag();

    std::array<int, 7> m_bag{};
    std::size_t m_nextIndex{0};
};

#endif // PIECEFACTORIES_H