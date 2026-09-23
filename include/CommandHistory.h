#ifndef COMMANDHISTORY_H
#define COMMANDHISTORY_H

#include <memory>

#include "Command.h"

// Invoker: хранит последнюю выполненную команду и умеет отменить именно ее.
// Поддерживается один уровень отмены (как минимум требует методичка) -
// после UndoLast() вернуться на шаг еще раньше уже нельзя.
class CommandHistory
{
public:
    // Выполнить команду и запомнить ее как последнюю.
    void Execute(std::unique_ptr<Command> command)
    {
        command->Execute();
        m_last = std::move(command);
    }

    // Запомнить уже выполненную команду как последнюю (например, макрос,
    // который выполнялся пошагово при сборке - см. BuildHardDropCommand).
    void Record(std::unique_ptr<Command> command)
    {
        m_last = std::move(command);
    }

    // Отменить последнюю команду, если она вообще есть.
    void UndoLast()
    {
        if (m_last)
        {
            m_last->Undo();
            m_last.reset();
        }
    }

private:
    std::unique_ptr<Command> m_last;
};

#endif // COMMANDHISTORY_H