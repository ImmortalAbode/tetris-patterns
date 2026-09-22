#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <memory>
#include <string>

#include "GameReportBuilder.h"

// =====================================================================
// ПАТТЕРНЫ: FACTORY METHOD (Фабричный метод) + TEMPLATE METHOD
// =====================================================================
// Здесь клиент строителя - это сам Generate(): фиксированный алгоритм 
// (шаблонный метод), который не знает, какой именно конкретный строитель 
// ему подсунут.
// CreateBuilder() - фабричный метод: каждый подкласс переопределяет его,
// чтобы вернуть СВОЙ конкретный строитель (Text/Html/Csv), а остальной код
// Generate() от этого не меняется ни на строчку.
//
// main.cpp вообще не видит имен строителей - только конкретные генераторы 
// (ReportGenerators.h), а строители спрятаны внутри ReportGenerator.cpp.
//
// Участники:
//   - Product         = GameReportBuilder (GameReportBuilder.h)
//   - Creator         = ReportGenerator (этот файл)
//   - ConcreteCreator = TextReportGenerator, HtmlReportGenerator,
//     CsvReportGenerator (ReportGenerators.h)
// =====================================================================
class ReportGenerator
{
public:
    virtual ~ReportGenerator() = default;

    // Шаблонный метод: порядок действий фиксирован и от подкласса не зависит.
    void Generate() const
    {
        std::unique_ptr<GameReportBuilder> builder = CreateBuilder();   // фабричный метод

        ReportDirector director;
        director.Construct(*builder);

        Save(builder->GetResult());
    }

protected:
    // Фабричный метод: подкласс решает, какой конкретный строитель создать.
    virtual std::unique_ptr<GameReportBuilder> CreateBuilder() const = 0;

    // Куда доставить готовый отчет (консоль/файл) - тоже решает подкласс.
    virtual void Save(const std::string& result) const = 0;
};

#endif // REPORTGENERATOR_H
