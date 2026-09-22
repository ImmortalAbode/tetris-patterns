#include "ShapeAbstractFactory.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <map>
#include <utility>

namespace
{
    // Реестр одиночек: имя (в нижнем регистре) -> фабрика. Реестр создается при первом
    // обращении к функции, поэтому порядок инициализации между файлами не важен.
    std::map<std::string, std::unique_ptr<ShapeAbstractFactory>>& Registry()
    {
        static std::map<std::string, std::unique_ptr<ShapeAbstractFactory>> registry;
        return registry;
    }

    // Имена тем сравниваются без учета регистра: "Neon", "neon" и "NEON" - одна тема.
    std::string ToLower(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(),
                        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return text;
    }
} // namespace

ShapeAbstractFactory* ShapeAbstractFactory::s_instance = nullptr;

void ShapeAbstractFactory::Register(std::unique_ptr<ShapeAbstractFactory> factory)
{
    std::string key = ToLower(factory->Name());

    // Если фабрика с таким именем уже есть, новую отбрасываем: замена удалила бы
    // старую, а s_instance мог бы указывать на нее (висячий указатель).
    Registry().try_emplace(key, std::move(factory));
}


ShapeAbstractFactory* ShapeAbstractFactory::Lookup(const std::string& name)
{
    auto& registry = Registry();
    auto it = registry.find(ToLower(name));
    return (it != registry.end()) ? it->second.get() : nullptr;
}

// Отложенная инициализация: активная фабрика выбирается при первом обращении.
ShapeAbstractFactory& ShapeAbstractFactory::Instance()
{
    if (s_instance == nullptr)
    {
        const char* themeName = std::getenv("TETRIS_THEME");
        if (themeName != nullptr)
            s_instance = Lookup(themeName);
        if (s_instance == nullptr)  // переменная не задана или имя неизвестно
            s_instance = Lookup("Classic");
    }
    assert(s_instance != nullptr && "Фабрики тем не зарегистрированы (вызовите RegisterThemes)");
    return *s_instance;
}

void ShapeAbstractFactory::SelectNext()
{
    auto& registry = Registry();
    auto it = registry.find(ToLower(Instance().Name()));
    if (it != registry.end())
        ++it;
    if (it == registry.end())
        it = registry.begin();
    s_instance = it->second.get();
}