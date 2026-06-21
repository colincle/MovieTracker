#include <QApplication>
#include <QEnterEvent>
#include <QTest>

#include "AppStorage.hpp"
#include "StreamingPlatformButton.hpp"

static constexpr int HEIGHT = 36;
static const QString COLOR1 = "#aabbcc";
static const QString COLOR2 = "#112233";

class TestStreamingPlatformButton : public QObject
{
	Q_OBJECT

  private:
	static StreamingPlatform
	makePlatform(const QString &name = "Netflix", const QString &image = "")
	{
		StreamingPlatform p;
		p.name = name;
		p.image = image;
		return p;
	}

	static void sendEnter(QWidget *w)
	{
		QEnterEvent ev(QPointF(5, 5), QPointF(5, 5), QPointF(100, 100));
		QApplication::sendEvent(w, &ev);
	}

	static void sendLeave(QWidget *w)
	{
		QEvent ev(QEvent::Leave);
		QApplication::sendEvent(w, &ev);
	}

  private slots:

	// -- constructor ----------------------------------------------------------

	void constructorSetsPlatformNameAsText()
	{
		StreamingPlatformButton btn(makePlatform("Disney+"), HEIGHT, COLOR1, COLOR2);
		QCOMPARE(btn.text(), QString("Disney+"));
	}

	void constructorWithEmptyImagePathSetsNoIcon()
	{
		StreamingPlatformButton btn(makePlatform("Netflix", ""), HEIGHT, COLOR1, COLOR2);
		QVERIFY(btn.icon().isNull());
	}

	void constructorWithInvalidImagePathDoesNotCrash()
	{
		StreamingPlatformButton btn(
		    makePlatform("Netflix", "/nonexistent/image.png"),
		    HEIGHT,
		    COLOR1,
		    COLOR2
		);
		QVERIFY(btn.icon().isNull());
	}

	void constructorAppliesNormalStylesheet()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
	}

	// -- enterEvent -----------------------------------------------------------

	void enterEventAppliesHoverStylesheet()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		sendEnter(&btn);
		QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
	}

	void enterDoesNotUseNormalBgColor()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		sendEnter(&btn);
		QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR2));
	}

	// -- leaveEvent -----------------------------------------------------------

	void leaveEventAppliesNormalStylesheet()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		sendEnter(&btn);
		sendLeave(&btn);
		QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
	}

	void leaveDoesNotUseHoverBgColor()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		sendEnter(&btn);
		sendLeave(&btn);
		QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR1));
	}

	// -- unhover --------------------------------------------------------------

	void unhoverAppliesNormalStylesheet()
	{
		StreamingPlatformButton btn(makePlatform(), HEIGHT, COLOR1, COLOR2);
		sendEnter(&btn);
		btn.unhover();
		QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
	}
};

#include "TestStreamingPlatformButton.moc"

QObject *createTestStreamingPlatformButton()
{
	return new TestStreamingPlatformButton();
}
