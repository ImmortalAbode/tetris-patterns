#include "GameActions.h"

#include "GameBoard.h"
#include "ReportGenerators.h"
#include "ScoreManager.h"

void TryMove(Tetromino& piece, int dx)
{
    piece.Move(dx, 0);
    if (GameBoard::Instance().Collides(piece))
        piece.Move(-dx, 0);
}

void RotatePiece(Tetromino& piece)
{
    Tetromino backup = piece;
    piece.RotateClockwise();
    if (GameBoard::Instance().Collides(piece))
        piece = backup;
}

void SaveGameReport()
{
    TextReportGenerator().Generate();
    HtmlReportGenerator().Generate();
    CsvReportGenerator().Generate();
}

bool MoveDown(std::unique_ptr<Tetromino>& piece, PieceFactory& factory)
{
    GameBoard& board = GameBoard::Instance();

    piece->Move(0, 1);
    if (!board.Collides(*piece))
        return true;
    piece->Move(0, -1);

    board.Lock(*piece);
    ScoreManager::Instance().AddPiece(piece->ColorIndex());
    int cleared = board.ClearLines();
    if (cleared > 0)
        ScoreManager::Instance().AddLines(cleared);

    piece = factory.CreatePiece();
    if (board.Collides(*piece))     // новая фигура сразу уперлась - игра окончена.
    {
        SaveGameReport();
        board.Reset();              // рестарт
        ScoreManager::Instance().Reset();
    }
    return false;
}