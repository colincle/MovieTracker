#include <QSignalSpy>
#include <QTest>

#include "SearchBar.hpp"

class TestSearchBar : public QObject
{
	Q_OBJECT

  private slots:

	void escapePressedEmitsEscapeSignal()
	{
		SearchBar  bar;
		QSignalSpy spy(&bar, &SearchBar::escapePressed);
		QTest::keyClick(&bar, Qt::Key_Escape);
		QCOMPARE(spy.count(), 1);
	}

	void returnKeyDoesNotEmitEscapeSignal()
	{
		SearchBar  bar;
		QSignalSpy spy(&bar, &SearchBar::escapePressed);
		QTest::keyClick(&bar, Qt::Key_Return);
		QCOMPARE(spy.count(), 0);
	}

	void typingAddsTextNormally()
	{
		SearchBar bar;
		QTest::keyClicks(&bar, "hello");
		QCOMPARE(bar.text(), QString("hello"));
	}

	void escapeDoesNotAddText()
	{
		SearchBar bar;
		QTest::keyClicks(&bar, "hi");
		QTest::keyClick(&bar, Qt::Key_Escape);
		QCOMPARE(bar.text(), QString("hi"));
	}
};

#include "TestSearchBar.moc"

QObject *createTestSearchBar()
{
	return new TestSearchBar();
}
