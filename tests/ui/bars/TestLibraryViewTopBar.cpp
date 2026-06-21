#include <QPushButton>
#include <QSignalSpy>
#include <QTest>

#include "IconButton.hpp"
#include "LibraryViewTopBar.hpp"
#include "SearchBar.hpp"
#include "SortEnums.hpp"
#include "TextButton.hpp"

class TestLibraryViewTopBar : public QObject
{
	Q_OBJECT

  private:
	static QPushButton *findByText(LibraryViewTopBar &bar, const QString &text)
	{
		for(auto *b : bar.findChildren<QPushButton *>())
			if(b->text() == text)
				return b;
		return nullptr;
	}

	static QList<IconButton *> findIconButtons(LibraryViewTopBar &bar)
	{
		QList<IconButton *> result;
		for(auto *b : bar.findChildren<HoverButton *>())
			if(auto *ib = dynamic_cast<IconButton *>(b))
				result.append(ib);
		return result;
	}

  private slots:

	// -- filter buttons -------------------------------------------------------

	void showAllAlreadyActiveDoesNotEmitFilterChanged()
	{
		LibraryViewTopBar bar;
		int               count = 0;
		connect(&bar, &LibraryViewTopBar::filterChanged, [&](ViewFilter) { ++count; });
		auto *btn = findByText(bar, "All");
		QVERIFY(btn);
		btn->click(); // already active
		QCOMPARE(count, 0);
	}

	void showToWatchClickedEmitsFilterChangedToWatch()
	{
		LibraryViewTopBar bar;
		ViewFilter        received = ViewFilter::All;
		int               count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::filterChanged,
		    [&](ViewFilter f)
		    {
			    received = f;
			    ++count;
		    }
		);
		auto *btn = findByText(bar, "To watch");
		QVERIFY(btn);
		btn->click();
		QCOMPARE(count, 1);
		QCOMPARE(received, ViewFilter::ToWatch);
	}

	void showToWatchAlreadyActiveDoesNotEmitFilterChanged()
	{
		LibraryViewTopBar bar;
		int               count = 0;
		connect(&bar, &LibraryViewTopBar::filterChanged, [&](ViewFilter) { ++count; });
		auto *btn = findByText(bar, "To watch");
		QVERIFY(btn);
		btn->click(); // first click activates
		count = 0;
		btn->click(); // already active — no emit
		QCOMPARE(count, 0);
	}

	void showAllClickedAfterToWatchEmitsFilterChangedAll()
	{
		LibraryViewTopBar bar;
		ViewFilter        received = ViewFilter::ToWatch;
		int               count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::filterChanged,
		    [&](ViewFilter f)
		    {
			    received = f;
			    ++count;
		    }
		);
		findByText(bar, "To watch")->click();
		count = 0;
		received = ViewFilter::ToWatch;
		findByText(bar, "All")->click();
		QCOMPARE(count, 1);
		QCOMPARE(received, ViewFilter::All);
	}

	// -- search: Return key ---------------------------------------------------

	void returnPressedEmitsSearchRequested()
	{
		LibraryViewTopBar bar;
		QString           received;
		int               count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::searchRequested,
		    [&](const QString &q)
		    {
			    received = q;
			    ++count;
		    }
		);
		auto *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("Batman");
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(count, 1);
		QCOMPARE(received, "Batman");
	}

	void returnPressedQueryIsTrimmed()
	{
		LibraryViewTopBar bar;
		QString           received;
		connect(
		    &bar,
		    &LibraryViewTopBar::searchRequested,
		    [&](const QString &q) { received = q; }
		);
		auto *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("  batman  ");
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(received, "batman");
	}

	// -- search: text changes -------------------------------------------------

	void clearingSearchTextEmitsSearchRequestedEmpty()
	{
		LibraryViewTopBar bar;
		int               count = 0;
		QString           received = "x";
		connect(
		    &bar,
		    &LibraryViewTopBar::searchRequested,
		    [&](const QString &q)
		    {
			    received = q;
			    ++count;
		    }
		);
		auto *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("Batman");
		count = 0;
		sb->clear();
		QCOMPARE(count, 1);
		QVERIFY(received.isEmpty());
	}

	void typingNonEmptyTextDoesNotEmitSearchRequested()
	{
		LibraryViewTopBar bar;
		int               count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::searchRequested,
		    [&](const QString &) { ++count; }
		);
		auto *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("B");
		QCOMPARE(count, 0);
	}

	// -- closeSearch ----------------------------------------------------------

	void escapePressedEmitsSearchRequestedEmpty()
	{
		LibraryViewTopBar bar;
		QString           received = "x";
		connect(
		    &bar,
		    &LibraryViewTopBar::searchRequested,
		    [&](const QString &q) { received = q; }
		);
		auto *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("batman");
		QTest::keyClick(sb, Qt::Key_Escape);
		QVERIFY(received.isEmpty());
	}

	void escapePressedClearsSearchInput()
	{
		LibraryViewTopBar bar;
		auto             *sb = bar.findChild<SearchBar *>();
		QVERIFY(sb);
		sb->setText("batman");
		QTest::keyClick(sb, Qt::Key_Escape);
		QVERIFY(sb->text().isEmpty());
	}

	// -- zoom buttons ---------------------------------------------------------
	// IconButton creation order: searchButton[0], closeButton[1], zoomIn[2], zoomOut[3]

	void zoomInButtonClickEmitsZoomRequestedPlus10()
	{
		LibraryViewTopBar bar;
		int               received = 0, count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::zoomRequested,
		    [&](int v)
		    {
			    received = v;
			    ++count;
		    }
		);
		auto icons = findIconButtons(bar);
		QCOMPARE(icons.size(), 4);
		icons[2]->click();
		QCOMPARE(count, 1);
		QCOMPARE(received, 10);
	}

	void zoomOutButtonClickEmitsZoomRequestedMinus10()
	{
		LibraryViewTopBar bar;
		int               received = 0, count = 0;
		connect(
		    &bar,
		    &LibraryViewTopBar::zoomRequested,
		    [&](int v)
		    {
			    received = v;
			    ++count;
		    }
		);
		auto icons = findIconButtons(bar);
		QCOMPARE(icons.size(), 4);
		icons[3]->click();
		QCOMPARE(count, 1);
		QCOMPARE(received, -10);
	}

	// -- misc -----------------------------------------------------------------

	void refreshStyleDoesNotCrash()
	{
		LibraryViewTopBar bar;
		bar.refreshStyle();
	}
};

#include "TestLibraryViewTopBar.moc"

QObject *createTestLibraryViewTopBar()
{
	return new TestLibraryViewTopBar();
}
