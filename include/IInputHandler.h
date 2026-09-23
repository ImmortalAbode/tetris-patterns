#ifndef IINPUTHANDLER_H
#define IINPUTHANDLER_H

// =====================================================================
// ПАТТЕРН: ADAPTER (Адаптер), объектный вариант (композиция)
// =====================================================================
// Задание методички (лаба 4) - привести интерфейс CList (MFC, только Windows)
// к интерфейсу Queue/Stack - неприменимо на macOS без MFC. Используем ту же
// идею паттерна на реальной задаче проекта: игровая логика (main) не должна
// зависеть от конкретного источника ввода (клавиатура, в перспективе -
// геймпад). main работает только с игровыми действиями через IInputHandler.
//
// Участники:
//   - Target (IInputHandler)                     - интерфейс, которого ждет клиент
//   - Client (main)                              - использует только IInputHandler
//   - Adaptee (sf::Event / sf::Keyboard из SFML) - несовместимый интерфейс:
//     оперирует кодами клавиш, а не игровыми действиями
//   - Adapter (KeyboardInputAdapter)             - переводит события SFML
//     в InputAction; композиция (хранит ссылку на sf::RenderWindow), а не
//     наследование - объектный вариант адаптера.
// =====================================================================

// Игровые действия - то, чем оперирует main, вместо кодов клавиш SFML.
enum class InputAction
{
    None,
    Quit,
    MoveLeft,
    MoveRight,
    SoftDrop,
    HardDrop,
    RotateCW,
    Undo,
    SwitchTheme,
    SwitchMode,
    Pause,
    Restart,   // подтвердить экран Game Over и начать заново
};

// Target: единый интерфейс ввода, не зависящий от конкретного устройства.
class IInputHandler
{
public:
    virtual ~IInputHandler() = default;

    // Опросить очередное игровое действие. InputAction::None - действий
    // сейчас нет (очередь событий пуста на этом кадре).
    virtual InputAction PollAction() = 0;
};

#endif // IINPUTHANDLER_H