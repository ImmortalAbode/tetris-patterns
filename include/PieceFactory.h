#ifndef PIECEFACTORY_H
#define PIECEFACTORY_H

#include <memory>

#include "Config.h"
#include "Piece.h"

// =====================================================================
// ПАТТЕРН: FACTORY METHOD (Фабричный метод) + PROTOTYPE
// =====================================================================
// Зачем здесь: игра должна уметь спавнить фигуры по-разному в зависимости от
// режима (Normal - равномерно случайно, Sprint - "мешок из 7"), но остальной
// код (main) не должен знать, какой именно способ сейчас используется.
// CreatePiece() - фабричный метод: каждый подкласс решает, какую фигуру и как
// выбрать, а возвращает всегда один и тот же тип - unique_ptr<Piece>.
//
// Prototype внутри: сами фигуры фабрика не собирает из таблицы "на лету",
// а клонирует готовый образец (Piece::Clone()) из общего набора прототипов
// (7 штук - I, O, T, S, Z, J, L), см. PieceFactories.cpp. Это тот же прием,
// что и в ShapePrototypeFactory для тем оформления.
//
// Участники:
//   - Product          = Piece (Piece.h)
//   - Creator          = PieceFactory (этот файл)
//   - ConcreteCreator  = NormalPieceFactory, SprintPieceFactory (PieceFactories.h)
// =====================================================================
class PieceFactory
{
public:
    virtual ~PieceFactory() = default;

    // Фабричный метод: вернуть следующую фигуру для спавна.
    virtual std::unique_ptr<Piece> CreatePiece() = 0;

protected:
    // Общий для всех подклассов шаг: поставить фигуру по центру верха поля.
    static void PlaceAtSpawn(Piece& piece)
    {
        piece.SetPosition(COLS / 2 - piece.Size() / 2, 0);
    }
};

#endif // PIECEFACTORY_H