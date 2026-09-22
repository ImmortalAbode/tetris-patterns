#ifndef GAMEREPORTBUILDER_H
#define GAMEREPORTBUILDER_H

#include <string>

// =====================================================================
// ПАТТЕРН: BUILDER (Строитель)
// =====================================================================
// Зачем здесь: отчет об окончании игры состоит из нескольких частей (шапка,
// блок со статистикой, концовка), а нужен он в разных представлениях: как
// текст в консоль, как HTML-страница, как строка в CSV-истории игр. Один и
// тот же порядок сборки (см. ReportDirector::Construct) должен давать
// разные представления одних и тех же данных - в этом и есть назначение
// Builder'а (Строитель).
//
// Участники:
//   - Builder (GameReportBuilder)          - абстрактный интерфейс сборки
//   - ConcreteBuilder (ReportBuilders.h)   - TextReportBuilder,
//     HtmlReportBuilder, CsvReportBuilder: собирают и хранят свой формат
//   - Director (ReportDirector)            - задает фиксированный порядок
//     вызова шагов, не зная, как каждый шаг оформляется
//   - Product                              - готовый std::string (у каждого
//     строителя свой: обычный текст, HTML-документ или CSV-строка)
// =====================================================================
class GameReportBuilder
{
public:
    virtual ~GameReportBuilder() = default;

    virtual void BuildHeader() = 0;
    virtual void BuildBlock() = 0;
    virtual void BuildEnding() = 0;

    virtual std::string GetResult() const = 0;
};

// Распорядитель: знает порядок сборки отчета (шапка -> статистика -> конец),
// но не знает, как каждая часть оформляется - это скрыто за интерфейсом Builder.
class ReportDirector
{
public:
    void Construct(GameReportBuilder& builder) const
    {
        builder.BuildHeader();
        builder.BuildBlock();
        builder.BuildEnding();
    }
};

#endif // GAMEREPORTBUILDER_H