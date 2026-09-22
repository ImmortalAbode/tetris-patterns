# Makefile для проекта Tetris (macOS, clang++, SFML 3 из Homebrew).
# Команды: make (собрать), make run (собрать и запустить, режим Normal),
# make run-sprint (собрать и запустить в режиме Sprint), make clean (удалить сборку).

# Компилятор из Xcode Command Line Tools.
CXX := clang++

# Флаги компиляции:
#   -std=c++17     стандарт языка
#   -Wall -Wextra  больше предупреждений компилятора
#   -g             отладочная информация (нужна для lldb в VS Code)
#   -Iinclude      искать наши заголовки в папке include/
#   -MMD -MP       генерировать .d файлы с зависимостями от заголовков,
#                  чтобы при изменении .hpp пересобирались нужные .cpp
CXXFLAGS := -std=c++17 -Wall -Wextra -g -Iinclude -MMD -MP

# Пути к SFML и флаги линковки берём у pkg-config (он ставится вместе с Homebrew
# и знает, где лежит SFML): -I... для заголовков, -L... -lsfml-* для библиотек.
SFML_CFLAGS := $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS   := $(shell pkg-config --libs sfml-graphics)

# Имя исполняемого файла и папка для результатов сборки.
TARGET    := build/tetris
BUILD_DIR := build

# Все .cpp из src/ (включая вложенные папки) и соответствующие им .o в build/.
# Новые файлы подхватываются автоматически, список руками вести не нужно.
SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Цель по умолчанию: просто "make" собирает программу.
all: $(TARGET)

# Линковка: собираем все .o и библиотеки SFML в один исполняемый файл.
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(SFML_LIBS) -o $@

# Компиляция одного .cpp в .o. Папка назначения создаётся при необходимости.
$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c $< -o $@

# Собрать и запустить (режим Normal).
run: $(TARGET)
	./$(TARGET)

# Собрать и запустить в режиме Sprint. "sprint" передаётся программе как
# argv[1], а не через make - в этом весь смысл отдельной цели: сам make
# аргументов не принимает, "make run sprint" пытался бы собрать цель "sprint".
run-sprint: $(TARGET)
	./$(TARGET) sprint

# Удалить всё, что создала сборка.
clean:
	rm -rf $(BUILD_DIR)

# Подключить .d файлы зависимостей (если их ещё нет — не ошибка).
-include $(DEPS)

# run, run-sprint, clean и all — не файлы, а команды.
.PHONY: all run run-sprint clean
