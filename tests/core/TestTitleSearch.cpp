#include <QTest>

#include "SortEnums.hpp"
#include "Title.hpp"
#include "TitleSearch.hpp"

class TestTitleSearch : public QObject
{
    Q_OBJECT

  private slots:

    // ── sortTitles AlphaAZ ───────────────────────────────────────────────────

    void sortAlphaAZSortsAZ()
    {
        std::vector<Title> titles = {makeTitle("Zorro"), makeTitle("Alien"), makeTitle("Batman")};
        sortTitles(titles, SortMode::AlphaAZ);
        QCOMPARE(titles[0].title, "Alien");
        QCOMPARE(titles[1].title, "Batman");
        QCOMPARE(titles[2].title, "Zorro");
    }

    void sortAlphaAZStripsLeadingThe()
    {
        std::vector<Title> titles = {makeTitle("Alien"), makeTitle("The Batman")};
        sortTitles(titles, SortMode::AlphaAZ);
        QCOMPARE(titles[0].title, "Alien");
        QCOMPARE(titles[1].title, "The Batman"); // sorts as "batman"
    }

    void sortAlphaAZStripsLeadingA()
    {
        std::vector<Title> titles = {makeTitle("Zorro"), makeTitle("A Bug's Life")};
        sortTitles(titles, SortMode::AlphaAZ);
        QCOMPARE(titles[0].title, "A Bug's Life"); // sorts as "bug's life"
        QCOMPARE(titles[1].title, "Zorro");
    }

    void sortAlphaAZStripsLeadingAn()
    {
        std::vector<Title> titles = {makeTitle("Zorro"), makeTitle("An Inconvenient Truth")};
        sortTitles(titles, SortMode::AlphaAZ);
        QCOMPARE(titles[0].title, "An Inconvenient Truth"); // sorts as "inconvenient truth"
        QCOMPARE(titles[1].title, "Zorro");
    }

    // ── sortTitles Release ───────────────────────────────────────────────────

    void sortReleaseSortsByDateDescending()
    {
        std::vector<Title> titles = {
            makeTitleWithRelease("Old Movie", "01 Jan 2000"),
            makeTitleWithRelease("New Movie", "15 Jun 2023"),
            makeTitleWithRelease("Mid Movie", "20 Mar 2010"),
        };
        sortTitles(titles, SortMode::Release);
        QCOMPARE(titles[0].title, "New Movie");
        QCOMPARE(titles[1].title, "Mid Movie");
        QCOMPARE(titles[2].title, "Old Movie");
    }

    // ── sortTitles WatchDate ─────────────────────────────────────────────────

    void sortWatchDateSortsByDateAscending()
    {
        std::vector<Title> titles = {
            makeTitleWithWatchDate("Recent",  QDate(2024, 6, 1)),
            makeTitleWithWatchDate("Oldest",  QDate(2020, 1, 1)),
            makeTitleWithWatchDate("Middle",  QDate(2022, 3, 15)),
        };
        sortTitles(titles, SortMode::WatchDate);
        QCOMPARE(titles[0].title, "Oldest");
        QCOMPARE(titles[1].title, "Middle");
        QCOMPARE(titles[2].title, "Recent");
    }

    void sortWatchDatePutsNeverWatchedFirst()
    {
        std::vector<Title> titles = {
            makeTitle("Unwatched"),
            makeTitleWithWatchDate("Watched", QDate(2024, 1, 1)),
        };
        sortTitles(titles, SortMode::WatchDate);
        QCOMPARE(titles[0].title, "Unwatched");
        QCOMPARE(titles[1].title, "Watched");
    }

    // ── sortTitles Rank ──────────────────────────────────────────────────────

    void sortRankSortsByRankAscending()
    {
        std::vector<Title> titles = {
            makeTitleWithRank("Third",  3),
            makeTitleWithRank("First",  1),
            makeTitleWithRank("Second", 2),
        };
        sortTitles(titles, SortMode::Rank);
        QCOMPARE(titles[0].title, "First");
        QCOMPARE(titles[1].title, "Second");
        QCOMPARE(titles[2].title, "Third");
    }

    void sortRankRemovesUnrankedTitles()
    {
        std::vector<Title> titles = {
            makeTitleWithRank("Ranked",   1),
            makeTitleWithRank("Unranked", 0),
        };
        sortTitles(titles, SortMode::Rank);
        QCOMPARE((int)titles.size(), 1);
        QCOMPARE(titles[0].title, "Ranked");
    }

    // ── filterTitles ─────────────────────────────────────────────────────────

    void filterTitlesMoviesTabReturnsOnlyMovies()
    {
        std::vector<Title> titles = {
            makeTitleWithType("Movie A", "movie"),
            makeTitleWithType("Series A", "series"),
            makeTitleWithType("Movie B", "movie"),
        };
        auto result = filterTitles(titles, LibraryTab::Movies, ViewFilter::All);
        QCOMPARE((int)result.size(), 2);
        QCOMPARE(result[0].title, "Movie A");
        QCOMPARE(result[1].title, "Movie B");
    }

    void filterTitlesTvShowsTabReturnsOnlySeries()
    {
        std::vector<Title> titles = {
            makeTitleWithType("Movie A", "movie"),
            makeTitleWithType("Series A", "series"),
        };
        auto result = filterTitles(titles, LibraryTab::TvShows, ViewFilter::All);
        QCOMPARE((int)result.size(), 1);
        QCOMPARE(result[0].title, "Series A");
    }

    void filterTitlesToWatchExcludesViewed()
    {
        std::vector<Title> titles = {
            makeTitleWatched("Watched Movie", "movie", true),
            makeTitleWatched("Unwatched Movie", "movie", false),
        };
        auto result = filterTitles(titles, LibraryTab::Movies, ViewFilter::ToWatch);
        QCOMPARE((int)result.size(), 1);
        QCOMPARE(result[0].title, "Unwatched Movie");
    }

    void filterTitlesAllFilterIncludesViewed()
    {
        std::vector<Title> titles = {
            makeTitleWatched("Watched Movie", "movie", true),
            makeTitleWatched("Unwatched Movie", "movie", false),
        };
        auto result = filterTitles(titles, LibraryTab::Movies, ViewFilter::All);
        QCOMPARE((int)result.size(), 2);
    }

    // ── scoreAndRankTitles ───────────────────────────────────────────────────

    void scoreAndRankEmptyQueryReturnsAllTitles()
    {
        std::vector<Title> titles = {makeTitle("Alien"), makeTitle("Batman")};
        auto results = scoreAndRankTitles(titles, "");
        QCOMPARE((int)results.size(), 2);
    }

    void scoreAndRankQueryMatchesTitle()
    {
        std::vector<Title> titles = {makeTitle("Alien"), makeTitle("Batman")};
        auto results = scoreAndRankTitles(titles, "Batman");
        QVERIFY(!results.empty());
        QCOMPARE(results[0].title, "Batman");
    }

    void scoreAndRankNoMatchReturnsEmpty()
    {
        std::vector<Title> titles = {makeTitle("Alien"), makeTitle("Batman")};
        auto results = scoreAndRankTitles(titles, "xqzwrty");
        QCOMPARE((int)results.size(), 0);
    }

    void scoreAndRankBestMatchComesFirst()
    {
        std::vector<Title> titles = {makeTitle("Batman Begins"), makeTitle("Batman")};
        auto results = scoreAndRankTitles(titles, "Batman");
        QVERIFY(!results.empty());
        QCOMPARE(results[0].title, "Batman");
    }

  private:
    static Title makeTitle(const QString &name)
    {
        Title t;
        t.title = name;
        return t;
    }

    static Title makeTitleWithRelease(const QString &name, const QString &released)
    {
        Title t = makeTitle(name);
        t.released = released;
        return t;
    }

    static Title makeTitleWithWatchDate(const QString &name, const QDate &date)
    {
        Title t = makeTitle(name);
        t.lastViewed = date;
        return t;
    }

    static Title makeTitleWithRank(const QString &name, int rank)
    {
        Title t = makeTitle(name);
        t.rank = rank;
        return t;
    }

    static Title makeTitleWithType(const QString &name, const QString &type)
    {
        Title t = makeTitle(name);
        t.type = type;
        return t;
    }

    static Title makeTitleWatched(const QString &name, const QString &type, bool viewed)
    {
        Title t = makeTitleWithType(name, type);
        t.viewed = viewed;
        return t;
    }
};

#include "TestTitleSearch.moc"

QObject *createTestTitleSearch() { return new TestTitleSearch(); }
