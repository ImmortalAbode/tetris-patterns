#ifndef CONFIG_H
#define CONFIG_H

// Общие константы игры.
constexpr int COLS{ 10 }; // ширина поля в клетках
constexpr int ROWS{ 20 }; // высота поля в клетках
constexpr int CELL{ 36 }; // размер клетки в пикселях

// Боковая HUD-панель (счет/тема/режим/превью следующей фигуры).
// Ширина в тех же "клетках", что и поле - превью фигуры рисуется тем же
// BlockStyle::Draw(cellX, cellY, ...), что и само поле, просто со сдвигом
// по колонке за пределы поля (cellX = COLS + ...).
constexpr int SIDEBAR_COLS{ 6 };
constexpr int SIDEBAR_WIDTH{ SIDEBAR_COLS * CELL };

// Окошко превью следующей фигуры внутри HUD-панели - тоже в клетках поля
// (PREVIEW_ANCHOR_COL/ROW - куда ставить его левый верхний угол), чтобы
// фигура превью рисовалась тем же blockStyle->Draw(cellX, cellY, ...), что
// и вся остальная игра, и попадала ровно на границы фоновых клеток окошка.
constexpr int PREVIEW_COLS{ 4 };
constexpr int PREVIEW_ROWS{ 2 };
constexpr int PREVIEW_ANCHOR_COL{ COLS + 1 };
constexpr int PREVIEW_ANCHOR_ROW{ 2 };

// Системный шрифт macOS (моноширинный, есть на любом современном Mac).
constexpr const char* FONT_PATH{ "/System/Library/Fonts/SFNSMono.ttf" };

#endif // CONFIG_H