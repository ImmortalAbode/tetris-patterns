#ifndef REPORTGENERATORS_H
#define REPORTGENERATORS_H

#include "ReportGenerator.h"

// Конкретные создатели (ConcreteCreator). Каждый переопределяет CreateBuilder(),
// чтобы вернуть свой конкретный строитель, и Save(), чтобы доставить готовый
// отчет по назначению. Конкретные строители (TextReportBuilder и т.д.) скрыты
// в ReportGenerator.cpp - клиенту (main) они не видны и не нужны.

class TextReportGenerator : public ReportGenerator
{
protected:
    std::unique_ptr<GameReportBuilder> CreateBuilder() const override;
    void Save(const std::string& result) const override;
};

class HtmlReportGenerator : public ReportGenerator
{
protected:
    std::unique_ptr<GameReportBuilder> CreateBuilder() const override;
    void Save(const std::string& result) const override;
};

class CsvReportGenerator : public ReportGenerator
{
protected:
    std::unique_ptr<GameReportBuilder> CreateBuilder() const override;
    void Save(const std::string& result) const override;
};

#endif // REPORTGENERATORS_H
