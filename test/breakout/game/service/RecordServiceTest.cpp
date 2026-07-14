#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

#include "breakout/game/service/RecordService.h"
#include "breakout/game/service/repository/RecordRepository.h"

// As REGRAS de recorde, exercitadas com um repositorio de mentira: ranking, corte
// do top-N e o que conta como recorde novo. Nada de disco aqui — o TSV de verdade
// vive no FileRecordRepositoryTest.

namespace {

class FakeRecordRepository final: public brk::RecordRepository
{
public:
    std::vector<brk::Record> stored;
    int                      saveCount = 0;

    [[nodiscard]] std::vector<brk::Record> loadAll() override { return stored; }

    void saveAll(const std::vector<brk::Record>& records) override
    {
        stored = records;
        ++saveCount;
    }
};

brk::Record record(const int score, const std::string& name = "P1", const uint32_t level = 1)
{
    return brk::Record{ name, score, level, "2026-07-14 10:00" };
}

// Um repositorio ja cheio, com pontuacoes 10, 20, ... — o ultimo colocado tem 10.
std::shared_ptr<FakeRecordRepository> fullRepository()
{
    auto repository = std::make_shared<FakeRecordRepository>();
    for (size_t i = 0; i < brk::RecordService::kMaxRecords; ++i)
    {
        repository->stored.push_back(record(10 * static_cast<int>(i + 1)));
    }
    return repository;
}

} // namespace

TEST(RecordServiceTest, StartsEmptyWhenNothingWasPersisted)
{
    const brk::RecordService service{ std::make_shared<FakeRecordRepository>() };

    EXPECT_TRUE(service.listByScore().empty());
}

TEST(RecordServiceTest, RanksByScoreDescending)
{
    brk::RecordService service{ std::make_shared<FakeRecordRepository>() };

    service.addRecord(record(50, "MEIO"));
    service.addRecord(record(90, "TOPO"));
    service.addRecord(record(10, "FUNDO"));

    const auto& ranked = service.listByScore();
    ASSERT_EQ(ranked.size(), 3u);
    EXPECT_EQ(ranked[0].name(), "TOPO");
    EXPECT_EQ(ranked[1].name(), "MEIO");
    EXPECT_EQ(ranked[2].name(), "FUNDO");
}

TEST(RecordServiceTest, PersistsOnEveryNewRecord)
{
    const auto         repository = std::make_shared<FakeRecordRepository>();
    brk::RecordService service{ repository };

    service.addRecord(record(50, "P1", 3));

    EXPECT_EQ(repository->saveCount, 1);
    ASSERT_EQ(repository->stored.size(), 1u);
    EXPECT_EQ(repository->stored[0].score(), 50);
    EXPECT_EQ(repository->stored[0].level(), 3u);
}

TEST(RecordServiceTest, KeepsOnlyTheTopNAndDoesNotPersistTheRest)
{
    const auto         repository = fullRepository(); // ja cheio; ultimo tem 10
    brk::RecordService service{ repository };

    service.addRecord(record(5, "FRACO"));

    EXPECT_EQ(service.listByScore().size(), brk::RecordService::kMaxRecords);
    EXPECT_EQ(repository->stored.size(), brk::RecordService::kMaxRecords);
    for (const auto& stored : repository->stored)
    {
        EXPECT_NE(stored.name(), "FRACO") << "o que nao entra no top-N nao vai para o arquivo";
    }
}

TEST(RecordServiceTest, GoodScoreEvictsTheLastPlace)
{
    const auto         repository = fullRepository();
    brk::RecordService service{ repository };

    service.addRecord(record(9999, "CAMPEAO"));

    const auto& ranked = service.listByScore();
    ASSERT_EQ(ranked.size(), brk::RecordService::kMaxRecords);
    EXPECT_EQ(ranked.front().name(), "CAMPEAO");
    EXPECT_EQ(ranked.back().score(), 20) << "o antigo ultimo colocado (10) foi expulso";
}

TEST(RecordServiceTest, AnyScoreIsARecordWhileThereIsRoom)
{
    const brk::RecordService service{ std::make_shared<FakeRecordRepository>() };

    EXPECT_TRUE(service.isNewRecord(1)) << "tabela vazia: ate 1 ponto entra";
}

TEST(RecordServiceTest, WithAFullTableOnlyBeatingTheLastPlaceCounts)
{
    const brk::RecordService service{ fullRepository() }; // ultimo colocado: 10

    EXPECT_TRUE(service.isNewRecord(11));
    EXPECT_FALSE(service.isNewRecord(10)) << "empatar com o ultimo nao entra";
    EXPECT_FALSE(service.isNewRecord(9));
}

TEST(RecordServiceTest, NormalizesAFileThatCameUnsortedOrTooBig)
{
    const auto repository = std::make_shared<FakeRecordRepository>();

    for (size_t i = 0; i < brk::RecordService::kMaxRecords + 5; ++i)
    {
        repository->stored.push_back(record(static_cast<int>(i) * 7 % 50));
    }

    const brk::RecordService service{ repository };

    const auto& ranked = service.listByScore();
    ASSERT_EQ(ranked.size(), brk::RecordService::kMaxRecords);
    EXPECT_TRUE(std::is_sorted(ranked.begin(), ranked.end(),
                               [](const brk::Record& a, const brk::Record& b) { return a.scoresHigherThan(b); }));
}

TEST(RecordServiceTest, SanitizesTheNameThatWouldBreakTheTsv)
{
    // Um tab no nome partiria a linha em campos errados na proxima leitura.
    EXPECT_EQ(brk::RecordService::sanitizeName("A\tB"), "AB");
    EXPECT_EQ(brk::RecordService::sanitizeName("A\nB"), "AB");
}

TEST(RecordServiceTest, TrimsLongNamesAndNamesTheNameless)
{
    EXPECT_EQ(brk::RecordService::sanitizeName("NOMEMUITOLONGO").size(), brk::RecordService::kMaxNameLength);
    EXPECT_EQ(brk::RecordService::sanitizeName(""), "ANONIMO");
    EXPECT_EQ(brk::RecordService::sanitizeName("\t\t"), "ANONIMO");
}

TEST(RecordServiceTest, StoredNameIsTheSanitizedOne)
{
    const auto         repository = std::make_shared<FakeRecordRepository>();
    brk::RecordService service{ repository };

    service.addRecord(record(100, "COM\tTAB"));

    ASSERT_EQ(repository->stored.size(), 1u);
    EXPECT_EQ(repository->stored[0].name(), "COMTAB");
}
