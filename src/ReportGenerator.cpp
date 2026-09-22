#include "ReportGenerators.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ScoreManager.h"

namespace
{
    // Названия фигур по индексу цвета (1..7), как в таблице SHAPES из main.cpp.
    const char* const PIECE_NAMES[8] = {"", "I", "O", "T", "S", "Z", "J", "L"};

    // Читаемая метка времени для заголовка отчета: "2026-09-22 18:30:00".
    std::string CurrentTimestamp()
    {
        std::time_t now = std::time(nullptr);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buffer;
    }

    // Метка времени для имени файла: без пробелов и двоеточий.
    std::string FilenameTimestamp()
    {
        std::time_t now = std::time(nullptr);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&now));
        return buffer;
    }

    // ---------- Конкретные строители (ConcreteBuilder), скрыты от клиента ----------
    // Клиент (main) видит только TextReportGenerator и т.д. из ReportGenerators.h,
    // эти классы создаются только внутри их фабричных методов CreateBuilder().

    class TextReportBuilder : public GameReportBuilder
    {
    public:
        void BuildHeader() override
        {
            m_result << "=== Tetris - Game Report ===\n";
            m_result << "Date: " << CurrentTimestamp() << "\n";
            m_result << "Mode: " << ScoreManager::Instance().Mode() << "\n\n";
        }


        void BuildBlock() override
        {
            ScoreManager& score = ScoreManager::Instance();
            m_result << "Score: " << score.Score() << "\n";
            m_result << "Lines cleared: " << score.Lines() << "\n";
            m_result << "Pieces placed: " << score.TotalPieces() << "\n";
            for (int i{1}; i <= 7; ++i)
                m_result << "  " << PIECE_NAMES[i] << ": " << score.PieceCount(i) << "\n";
        }

        void BuildEnding() override
        {
            m_result << "\nThanks for playing!\n";
        }

        std::string GetResult() const override 
        { 
            return m_result.str(); 
        }

    private:
        std::ostringstream m_result;
    };

    class HtmlReportBuilder : public GameReportBuilder
    {
    public:
        void BuildHeader() override
        {
            m_result << "<html><head><meta charset=\"utf-8\"><title>Tetris Report</title></head><body>\n";
            m_result << "<h1>Tetris - Game Report</h1>\n";
            m_result << "<p>Date: " << CurrentTimestamp() << "</p>\n";
            m_result << "<p>Mode: " << ScoreManager::Instance().Mode() << "</p>\n";
        }


        void BuildBlock() override
        {
            ScoreManager& score = ScoreManager::Instance();
            m_result << "<h2>Statistics</h2>\n<ul>\n";
            m_result << "<li>Score: " << score.Score() << "</li>\n";
            m_result << "<li>Lines cleared: " << score.Lines() << "</li>\n";
            m_result << "<li>Pieces placed: " << score.TotalPieces() << "</li>\n";
            m_result << "</ul>\n<table border=\"1\"><tr><th>Piece</th><th>Count</th></tr>\n";
            for (int i{1}; i <= 7; ++i)
                m_result << "<tr><td>" << PIECE_NAMES[i] << "</td><td>" << score.PieceCount(i) << "</td></tr>\n";
            m_result << "</table>\n";
        }

        void BuildEnding() override
        {
            m_result << "<p>Thanks for playing!</p>\n</body></html>\n";
        }

        std::string GetResult() const override { return m_result.str(); }

    private:
        std::ostringstream m_result;
    };

    class CsvReportBuilder : public GameReportBuilder
    {
    public:
        // date,mode,score,lines,total_pieces,I,O,T,S,Z,J,L
        void BuildHeader() override
        {
            m_result << CurrentTimestamp() << "," << ScoreManager::Instance().Mode() << ",";
        }


        void BuildBlock() override
        {
            ScoreManager& score = ScoreManager::Instance();
            m_result << score.Score() << "," << score.Lines() << "," << score.TotalPieces();
            for (int i{1}; i <= 7; ++i)
                m_result << "," << score.PieceCount(i);
        }

        void BuildEnding() override
        {
            m_result << "\n";
        }

        std::string GetResult() const override { return m_result.str(); }

    private:
        std::ostringstream m_result;
    };
} // namespace

// ---------- Конкретные создатели (ConcreteCreator) ----------

std::unique_ptr<GameReportBuilder> TextReportGenerator::CreateBuilder() const
{
    return std::make_unique<TextReportBuilder>();
}

void TextReportGenerator::Save(const std::string& result) const
{
    std::cout << result;
}

std::unique_ptr<GameReportBuilder> HtmlReportGenerator::CreateBuilder() const
{
    return std::make_unique<HtmlReportBuilder>();
}

void HtmlReportGenerator::Save(const std::string& result) const
{
    std::ofstream file("report_" + FilenameTimestamp() + ".html");
    file << result;
}

std::unique_ptr<GameReportBuilder> CsvReportGenerator::CreateBuilder() const
{
    return std::make_unique<CsvReportBuilder>();
}

void CsvReportGenerator::Save(const std::string& result) const
{
    bool fileExisted = static_cast<bool>(std::ifstream("results.csv"));
    std::ofstream file("results.csv", std::ios::app);
    if (!fileExisted)
        file << "date,mode,score,lines,total_pieces,I,O,T,S,Z,J,L\n";
    file << result;
}