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
    // Если новая фигура сразу не помещается - это конец игры. MoveDown сюда
    // не лезет: main сам проверяет board.Collides(*piece) сразу после любого
    // вызова MoveDown (из гравитации или из команды) и показывает экран
    // Game Over - отчет сохраняется и поле/счет сбрасываются только тогда,
    // когда игрок это увидел и подтвердил рестарт (клавиша R).
    return false;
}