#ifndef GAMEACTIONS_H
#define GAMEACTIONS_H

#include <memory>

#include "PieceFactory.h"
#include "Tetromino.h"

// Игровые действия (раньше жили прямо в main.cpp). Вынесены в отдельный файл,
// чтобы ими могли пользоваться и main, и конкретные команды (Commands.h).

// Сдвинуть фигуру по горизонтали; если уперлись - вернуть назад.
void TryMove(Tetromino& piece, int dx);

// Повернуть фигуру; если после поворота места нет - отменить.
void RotatePiece(Tetromino& piece);

// Сдвинуть фигуру вниз на 1. Возвращает true, если фигура просто сдвинулась
// (обратимо), false - если она зафиксировалась и была заменена новой
// (необратимо: поле, счет и текущая фигура изменились).
bool MoveDown(std::unique_ptr<Tetromino>& piece, PieceFactory& factory);

// Сохранить отчет об игре во всех трех представлениях (Builder + Factory Method).
void SaveGameReport();

#endif // GAMEACTIONS_H