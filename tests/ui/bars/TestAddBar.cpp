#include <QPushButton>
#include <QSignalSpy>
#include <QTest>

#include "AddBar.hpp"
#include "SearchBar.hpp"

class TestAddBar : public QObject
{
	Q_OBJECT

  private:
	static SearchBar   *searchBar(AddBar &bar) { return bar.findChild<SearchBar *>(); }
	static QPushButton *returnButton(AddBar &bar)
	{
		return bar.findChild<QPushButton *>();
	}

  private slots:

	// -- return button ---------------------------------------------------------

	void returnButtonClickEmitsRequestNormalMode()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::requestNormalMode);
		auto      *btn = returnButton(bar);
		QVERIFY(btn);
		btn->click();
		QCOMPARE(spy.count(), 1);
	}

	// -- search bar: Return key ------------------------------------------------

	void returnPressedWithTextEmitsSearchRequested()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::searchRequested);
		auto      *sb = searchBar(bar);
		QVERIFY(sb);
		sb->setText("Batman");
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(spy.count(), 1);
		QCOMPARE(spy.first().first().toString(), "Batman");
	}

	void returnPressedWithEmptyTextDoesNotEmitSearchRequested()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::searchRequested);
		auto      *sb = searchBar(bar);
		QVERIFY(sb);
		sb->clear();
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(spy.count(), 0);
	}

	void returnPressedWithWhitespaceOnlyDoesNotEmitSearchRequested()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::searchRequested);
		auto      *sb = searchBar(bar);
		QVERIFY(sb);
		sb->setText("   ");
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(spy.count(), 0);
	}

	void searchRequestedQueryIsTrimmed()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::searchRequested);
		auto      *sb = searchBar(bar);
		QVERIFY(sb);
		sb->setText("  batman  ");
		QTest::keyClick(sb, Qt::Key_Return);
		QCOMPARE(spy.count(), 1);
		QCOMPARE(spy.first().first().toString(), "batman");
	}

	// -- search bar: Escape key ------------------------------------------------

	void escapePressedEmitsRequestNormalMode()
	{
		AddBar     bar;
		QSignalSpy spy(&bar, &AddBar::requestNormalMode);
		auto      *sb = searchBar(bar);
		QVERIFY(sb);
		QTest::keyClick(sb, Qt::Key_Escape);
		QCOMPARE(spy.count(), 1);
	}

	void escapePressedClearsSearchBar()
	{
		AddBar bar;
		auto  *sb = searchBar(bar);
		QVERIFY(sb);
		sb->setText("batman");
		QTest::keyClick(sb, Qt::Key_Escape);
		QVERIFY(sb->text().isEmpty());
	}

	// -- misc ------------------------------------------------------------------

	void refreshStyleDoesNotCrash()
	{
		AddBar bar;
		bar.refreshStyle();
	}
};

#include "TestAddBar.moc"

QObject *createTestAddBar()
{
	return new TestAddBar();
}
