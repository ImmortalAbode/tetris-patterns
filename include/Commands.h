#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <vector>

#include "Command.h"
#include "GameActions.h"
#include "GameBoard.h"
#include "PieceFactory.h"
#include "Tetromino.h"

// Конкретные команды (ConcreteCommand). Получатель (Receiver) - Tetromino
// (и, для DropCommand, GameBoard/PieceFactory через GameActions.h). Каждая
// команда сохраняет ровно то состояние, которое нужно, чтобы Undo() вернул
// именно то, что реально изменилось - а не то, что команда пыталась сделать.

// --- Сдвиг по горизонтали ---
class MoveCommand : public Command
{
public:
    MoveCommand(Tetromino& piece, int dx) : m_piece{piece}, m_dx{dx} {}

    void Execute() override
    {
        m_piece.Move(m_dx, 0);
        if (GameBoard::Instance().Collides(m_piece))
        {
            m_piece.Move(-m_dx, 0);
            m_moved = false;
        }
        else
        {
            m_moved = true;
        }
    }

    // Откатываем, только если Execute() реально подвинул фигуру - иначе
    // откатывать нечего (она и так на месте).
    void Undo() override
    {
        if (m_moved)
            m_piece.Move(-m_dx, 0);
    }

private:
    Tetromino& m_piece;
    int m_dx;
    bool m_moved{false};
};

class MoveLeftCommand : public MoveCommand
{
public:
    explicit MoveLeftCommand(Tetromino& piece) : MoveCommand(piece, -1) {}
};

class MoveRightCommand : public MoveCommand
{
public:
    explicit MoveRightCommand(Tetromino& piece) : MoveCommand(piece, 1) {}
};

// --- Поворот ---
class RotateCommand : public Command
{
public:
    explicit RotateCommand(Tetromino& piece) : m_piece{piece}, m_before{piece} {}

    void Execute() override
    {
        m_piece.RotateClockwise();
        if (GameBoard::Instance().Collides(m_piece))
            m_piece = m_before;   // не влезло - откат сразу внутри Execute
    }

    // Возвращаем ровно ту форму/позицию, что были до попытки поворота.
    void Undo() override
    {
        m_piece = m_before;
    }

private:
    Tetromino& m_piece;
    Tetromino m_before;
};

// --- Падение на одну клетку (по нажатию вниз) ---
class DropCommand : public Command
{
public:
    DropCommand(std::unique_ptr<Tetromino>& piece, PieceFactory& factory)
        : m_piece{piece}, m_factory{factory}, m_before{*piece} {}

    void Execute() override
    {
        m_reversible = MoveDown(m_piece, m_factory);
    }

    // Если фигура зафиксировалась (m_reversible == false), откат невозможен:
    // как и в настоящем Тетрисе, зафиксированный ход назад не отменить.
    void Undo() override
    {
        if (m_reversible)
            *m_piece = m_before;
    }

    bool IsReversible() const override { return m_reversible; }

private:
    std::unique_ptr<Tetromino>& m_piece;
    PieceFactory& m_factory;
    Tetromino m_before;
    bool m_reversible{false};
};

// --- Макрокоманда: последовательность команд, выполняемых и отменяемых как одно целое ---
class MacroCommand : public Command
{
public:
    void Add(std::unique_ptr<Command> command)
    {
        m_commands.push_back(std::move(command));
    }

    void Execute() override
    {
        for (auto& command : m_commands)
            command->Execute();
    }

    // Отмена в порядке, ОБРАТНОМ выполнению. Но если хоть одна подкоманда 
    // необратима (фигура зафиксировалась и была заменена новой), весь макрос 
    // отменить нельзя: более ранние подкоманды ссылаются на уже не существующую
    // фигуру, и частичный откат исказил бы состояние новой.
    void Undo() override
    {
        if (!IsReversible())
            return;

        for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it)
            (*it)->Undo();
    }

    bool IsReversible() const override
    {
        for (const auto& command : m_commands)
            if (!command->IsReversible())
                return false;
        return true;
    }

private:
    std::vector<std::unique_ptr<Command>> m_commands;
};

// Жесткое падение (hard drop): повторяем DropCommand, пока фигура не
// зафиксируется. Каждый шаг выполняется сразу же при сборке, поэтому
// у каждого DropCommand верный снимок состояния "до" - именно того момента,
// когда он реально создавался и выполнялся.
inline std::unique_ptr<MacroCommand> BuildHardDropCommand(
    std::unique_ptr<Tetromino>& piece, PieceFactory& factory)
{
    auto macro = std::make_unique<MacroCommand>();
    while (true)
    {
        auto step = std::make_unique<DropCommand>(piece, factory);
        step->Execute();
        bool reversible = step->IsReversible();
        macro->Add(std::move(step));
        if (!reversible)
            break;
    }
    return macro;
}

#endif // COMMANDS_H