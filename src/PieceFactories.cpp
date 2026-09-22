#include "PieceFactories.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <numeric>
#include <random>

namespace
{
    // Описание фигур: раньше эта таблица лежала в main.cpp.
    struct ShapeDef
    {
        int size{};
        const char* cells{};
    };

    const ShapeDef SHAPES[7] = {
        {4, "....XXXX........"}, // I: ..../XXXX/..../....
        {2, "XXXX"},             // O: XX/XX
        {3, ".X.XXX..."},        // T: .X./XXX/...
        {3, ".XXXX...."},        // S: .XX/XX./...
        {3, "XX..XX..."},        // Z: XX./.XX/...
        {3, "X..XXX..."},        // J: X../XXX/...
        {3, "..XXXX..."}         // L: ..X/XXX/...
    };

    // Прототипы 7 фигур (Prototype). Число видов тетромино фиксировано, поэтому
    // достаточно простого массива по индексу - полноценный реестр с именами и
    // операциями Register/Unregister (как для тем в ShapeAbstractFactory) здесь
    // избыточен: такой реестр нужен, когда набор прототипов может меняться во
    // время выполнения, а у нас он всегда один и тот же.
    std::array<std::unique_ptr<Tetromino>, 7>& Prototypes()
    {
        static std::array<std::unique_ptr<Tetromino>, 7> prototypes = []
        {
            std::array<std::unique_ptr<Tetromino>, 7> result;
            for (int i{0}; i < 7; ++i)
                result[i] = std::make_unique<Tetromino>(SHAPES[i].size, SHAPES[i].cells, i + 1);
            return result;
        }();
        return prototypes;
    }
} // namespace

// ---------- NormalPieceFactory ----------

std::unique_ptr<Tetromino> NormalPieceFactory::CreatePiece()
{
    int index = std::rand() % 7;
    std::unique_ptr<Tetromino> piece = Prototypes()[index]->Clone();
    PlaceAtSpawn(*piece);
    return piece;
}

// ---------- SprintPieceFactory ----------

SprintPieceFactory::SprintPieceFactory()
{
    RefillBag();
}

void SprintPieceFactory::RefillBag()
{
    std::iota(m_bag.begin(), m_bag.end(), 0);   // 0, 1, 2, ..., 6

    static std::mt19937 rng{std::random_device{}()};
    std::shuffle(m_bag.begin(), m_bag.end(), rng);

    m_nextIndex = 0;
}

std::unique_ptr<Tetromino> SprintPieceFactory::CreatePiece()
{
    if (m_nextIndex >= m_bag.size())
        RefillBag();

    int index = m_bag[m_nextIndex++];
    std::unique_ptr<Tetromino> piece = Prototypes()[index]->Clone();
    PlaceAtSpawn(*piece);
    return piece;
}