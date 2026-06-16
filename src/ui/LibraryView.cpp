#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"
#include "Levenshtein.hpp"

#include "LibraryView.hpp"
#include "ColorPalette.hpp"

#include <QFrame>
#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>

LibraryView::LibraryView(AppStorage &appStorage, QWidget *parent)
	: QWidget(parent)
	, appStorage(appStorage)
{
	setupUi();
	connectSignals();

	titles = appStorage.getTitles();
	populate();
}

void LibraryView::setupUi()
{
	setStyleSheet("background-color: " COLOR_BG_PRIMARY ";");
	setAttribute(Qt::WA_StyledBackground, true);

	auto *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, MARGIN, 0, MARGIN);
	layout->setSpacing(16);

	layout->addWidget(makeTopBarWrapper());
	layout->addWidget(makeScrollArea());

	resizeTimer = new QTimer(this);
	resizeTimer->setSingleShot(true);
	resizeTimer->setInterval(150);
}

QWidget *LibraryView::makeTopBarWrapper()
{
	libraryViewTopBar = new LibraryViewTopBar(this);

	auto *wrapper = new QWidget;
	auto *layout = new QVBoxLayout(wrapper);
	layout->setContentsMargins(MARGIN, 0, MARGIN, 0);
	layout->addWidget(libraryViewTopBar);

	return wrapper;
}

QWidget *LibraryView::makeScrollArea()
{
	cardsContainer = new QWidget;
	cardsContainer->setStyleSheet("background: transparent;");

	cardsLayout = new QGridLayout(cardsContainer);
	cardsLayout->setContentsMargins(0, 0, 0, 0);
	cardsLayout->setSpacing(CARD_SPACING);
	cardsLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setWidget(cardsContainer);
	scrollArea->setStyleSheet(
	    "QScrollArea { background: transparent; border: none; }"
	    "QScrollArea > QWidget > QWidget { background: transparent; }"
	    "QScrollBar:vertical { width: 0px; }"
	);

	return scrollArea;
}

void LibraryView::connectSignals()
{
	connect(&appStorage, &AppStorage::titlesUpdated, this, [this]()
	{
		onSearchRequested(currentQuery);
	});

	connect(libraryViewTopBar, &LibraryViewTopBar::searchRequested, this, &LibraryView::onSearchRequested);
	connect(libraryViewTopBar, &LibraryViewTopBar::filterChanged, this, &LibraryView::setFilter);
	connect(resizeTimer, &QTimer::timeout, this, &LibraryView::populate);
}

static int fuzzyScore(const QString &pattern, const QString &str)
{
	int score = 0;
	const QByteArray p = pattern.toLower().toUtf8();
	const QByteArray s = str.toLower().toUtf8();
	fts::fuzzy_match(p.constData(), s.constData(), score);
	return score;
}

static bool titleLevenshteinMatches(const Title &t, const QString &query)
{
	return levenshteinMatches(query, t.title)
	       || levenshteinMatches(query, t.actors)
	       || levenshteinMatches(query, t.director)
	       || levenshteinMatches(query, t.year)
	       || levenshteinMatches(query, t.released);
}

static int titleBestScore(const Title &t, const QString &query)
{
	return std::max(
	{
		fuzzyScore(query, t.title),
		fuzzyScore(query, t.actors),
		fuzzyScore(query, t.director),
		fuzzyScore(query, t.year),
		fuzzyScore(query, t.released),
	});
}

void LibraryView::onSearchRequested(const QString &query)
{
	currentQuery = query;
	titles.clear();

	if(query.isEmpty())
	{
		titles = appStorage.getTitles();
		populate();
		return;
	}

	std::vector<std::pair<int, Title>> scored;

	for(const Title &t : appStorage.getTitles())
	{
		int score = titleBestScore(t, query);

		if(score > 0)
		{
			scored.emplace_back(score, t);
		}
		else
			if(titleLevenshteinMatches(t, query))
			{
				scored.emplace_back(1, t);
			}
	}

	std::sort(scored.begin(), scored.end(), [](const auto & a, const auto & b)
	{
		return a.first > b.first;
	});

	for(auto &[score, t] : scored)
	{
		titles.push_back(std::move(t));
	}

	populate();
}

void LibraryView::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	resizeTimer->start();
}

int LibraryView::computeColumns() const
{
	int available = scrollArea->viewport()->width();
	int cols = (available + CARD_SPACING) / (CARD_WIDTH + CARD_SPACING);
	return qMax(1, cols);
}

int LibraryView::computeSpacing() const
{
	int available = scrollArea->viewport()->width();
	int cols = computeColumns();
	return (available - cols * CARD_WIDTH) / (cols + 1);
}

static QString sortKey(const QString &title)
{
	QString s = title.toLower();

	if(s.startsWith("the "))
	{
		return s.sliced(4);
	}

	if(s.startsWith("an "))
	{
		return s.sliced(3);
	}

	if(s.startsWith("a "))
	{
		return s.sliced(2);
	}

	return s;
}

static std::vector<Title> filterTitles(const std::vector<Title> &titles, LibraryTab tab, ViewFilter filter)
{
	std::vector<Title> result;

	for(const Title &t : titles)
	{
		if(filter == ViewFilter::ToWatch && t.viewed)
		{
			continue;
		}

		if(tab == LibraryTab::Movies && t.isMovie)
		{
			result.push_back(t);
		}

		if(tab == LibraryTab::TvShows && t.isSeries)
		{
			result.push_back(t);
		}
	}

	return result;
}

static void sortTitles(std::vector<Title> &titles, SortMode mode)
{
	switch(mode)
	{
	case SortMode::AlphaAZ:
		std::sort(titles.begin(), titles.end(), [](const Title & a, const Title & b)
		{
			return sortKey(a.title) < sortKey(b.title);
		});
		break;

	case SortMode::Release:
		std::sort(titles.begin(), titles.end(), [](const Title & a, const Title & b)
		{
			return QDate::fromString(a.released, "dd MMM yyyy")
			       > QDate::fromString(b.released, "dd MMM yyyy");
		});
		break;

	case SortMode::LastViewed:
		std::sort(titles.begin(), titles.end(), [](const Title & a, const Title & b)
		{
			return a.lastViewed < b.lastViewed;
		});
		break;

	case SortMode::Rank:
		std::sort(titles.begin(), titles.end(), [](const Title & a, const Title & b)
		{
			return a.rank < b.rank;
		});
		break;
	}
}

void LibraryView::populate()
{
	clear();

	std::vector<Title> filtered = filterTitles(titles, currentTab, currentFilter);
	sortTitles(filtered, currentSort);

	const int cols = computeColumns();
	const int spacing = computeSpacing();

	cardsLayout->setSpacing(spacing);
	cardsLayout->setContentsMargins(spacing, 0, spacing, 0);

	int col = 0;
	int row = 0;

	for(const Title &t : filtered)
	{
		cardsLayout->addWidget(new TitleCard(t, appStorage, cardsContainer), row, col);

		if(++col >= cols)
		{
			col = 0;
			++row;
		}
	}
}

void LibraryView::clear()
{
	while(QLayoutItem *item = cardsLayout->takeAt(0))
	{
		if(item->widget())
		{
			item->widget()->deleteLater();
		}

		delete item;
	}
}