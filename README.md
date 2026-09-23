# Тетрис на C++17 с 9 паттернами проектирования

Учебный проект — лабораторные работы по дисциплине «Проектирование и внедрение распределенных систем»
(Паттерны проектирования).
Игра Тетрис на C++17, SFML 3.0.2, сборка через Makefile (clang, macOS).

## Сборка и запуск

```
brew install sfml
make            # собрать
make run        # собрать и запустить (режим Normal)
make run-sprint # собрать и запустить в режиме Sprint
make clean      # удалить сборку
```

Тема оформления при старте задаётся переменной окружения `TETRIS_THEME`
(`Classic` / `Neon` / `Retro`, по умолчанию `Classic`):

```
TETRIS_THEME=neon make run
```

## Управление

| Клавиша | Действие |
|---|---|
| ← → | сдвиг фигуры |
| ↓ | мягкое падение на одну клетку |
| Space | жёсткое падение (мгновенно на дно) |
| ↑ | поворот по часовой стрелке |
| Backspace | отмена последнего хода (один уровень) |
| T | смена темы оформления |

При окончании игры в консоль выводится текстовый отчёт, а также создаются
`report_<дата>_<время>.html` и дописывается строка в `results.csv`.

## Паттерны проектирования

Все 9 паттернов явно выделены в коде: у каждого — свой файл (или чётко
обозначенный блок) и комментарий вида `// ПАТТЕРН: ...` с описанием
назначения и участников.

| № | Паттерн | Классы | Файлы |
|---|---|---|---|
| 1 | **Abstract Factory** | `ShapeAbstractFactory` (абстрактная фабрика), `ShapePrototypeFactory` (конкретная фабрика), `BlockStyle`/`GridStyle` (абстрактные продукты) | `include/ShapeAbstractFactory.h`, `include/ShapePrototypeFactory.h`, `src/ShapeAbstractFactory.cpp`, `src/Themes.cpp` |
| 2 | **Singleton** | `ShapeAbstractFactory` (одиночка с реестром тем), `GameBoard` (игровое поле), `ScoreManager` (счёт и статистика) | `include/ShapeAbstractFactory.h`, `include/GameBoard.h`, `include/ScoreManager.h` |
| 3 | **Builder** | `GameReportBuilder` (строитель), `ReportDirector` (распорядитель), `TextReportBuilder`/`HtmlReportBuilder`/`CsvReportBuilder` (конкретные строители, скрыты в `.cpp`) | `include/GameReportBuilder.h`, `src/ReportGenerator.cpp` |
| 4 | **Factory Method** | (а) `ReportGenerator` + `TextReportGenerator`/`HtmlReportGenerator`/`CsvReportGenerator` — создание строителя отчёта; (б) `PieceFactory` + `NormalPieceFactory`/`SprintPieceFactory` — создание фигур | `include/ReportGenerator.h`, `include/ReportGenerators.h`, `src/ReportGenerator.cpp`; `include/PieceFactory.h`, `include/PieceFactories.h`, `src/PieceFactories.cpp` |
| 5 | **Prototype** | (а) `BlockStyle::Clone()`/`GridStyle::Clone()` — образцы стилей темы; (б) `Tetromino::Clone()` — реестр из 7 образцов фигур | `include/ShapeAbstractFactory.h`, `src/Themes.cpp`; `include/Tetromino.h`, `src/Tetromino.cpp`, `src/PieceFactories.cpp` |
| 6 | **Adapter** | `IInputHandler` (целевой интерфейс), `KeyboardInputAdapter` (объектный адаптер к SFML) | `include/IInputHandler.h`, `include/KeyboardInputAdapter.h`, `src/KeyboardInputAdapter.cpp` |
| 7 | **Компоновщик (Composite)** | `GameObject` (компонент), `Block` (лист), `Tetromino` (составной объект) | `include/GameObject.h`, `include/Block.h`, `include/Tetromino.h`, `src/Tetromino.cpp` |
| 8 | **Chain of Responsibility** | `CollisionHandler` (обработчик), `WallCollisionHandler` → `FloorCollisionHandler` → `BlockCollisionHandler` (конкретные обработчики) | `include/CollisionHandler.h`, `include/CollisionHandlers.h`, `src/CollisionHandlers.cpp` |
| 9 | **Command** | `Command` (команда), `MoveLeftCommand`/`MoveRightCommand`/`RotateCommand`/`DropCommand`/`MacroCommand` (конкретные команды), `CommandHistory` (инициатор) | `include/Command.h`, `include/Commands.h`, `include/CommandHistory.h`, `src/GameActions.cpp` (получатель) |

### Как паттерны связаны друг с другом

- **Abstract Factory ↔ Prototype ↔ Singleton**: `ShapeAbstractFactory` —
  одиночка (реестр тем), которая параметризуется прототипами стилей и
  создаёт продукты клонированием, а не через `new`.
- **Factory Method ↔ Prototype**: `PieceFactory::CreatePiece()` не строит
  фигуру заново, а клонирует нужный образец из общего реестра прототипов.
- **Chain of Responsibility ↔ Composite**: обработчики столкновений
  перебирают потомков (`Tetromino::Blocks()`) — результат работы Composite.
- **Command ↔ Composite**: `MacroCommand` (жёсткое падение) — составной
  объект над простыми командами `DropCommand`, выполняется и отменяется как
  единое целое.
- **Builder ↔ Factory Method**: `ReportGenerator::CreateBuilder()` создаёт
  нужного строителя, а `Generate()` (шаблонный метод) использует его через
  общий алгоритм сборки отчёта.

## Структура проекта

```
include/   заголовки классов
src/       реализация (.cpp)
assets/    ресурсы (не используются — фокус на паттернах, а не на графике)
```

Методички к лабораторным работам во многих случаях описывают задачу,
неприменимую напрямую к Тетрису (диалоговые окна, меню рисования фигур,
MFC-классы Windows). В таких случаях в коде реализована исходная идея
паттерна, применённая к реальной задаче проекта — это отмечено в
комментариях `// ПАТТЕРН: ...` каждого файла.
