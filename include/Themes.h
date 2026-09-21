#ifndef THEMES_H
#define THEMES_H

// Регистрирует в реестре абстрактной фабрики фабрики всех тем (Classic, Neon).
// Вызывается один раз в начале main. Сами прототипы (ClassicBlockStyle и др.)
// спрятаны в Themes.cpp.
void RegisterThemes();

#endif // THEMES_H