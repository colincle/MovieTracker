#include <QApplication>
#include <QEnterEvent>
#include <QSize>
#include <QTest>

#include "IconButton.hpp"

static constexpr int BTN_SIZE = 40;
static constexpr int ICON_SIZE = BTN_SIZE / 1.5; // 26
static const QString COLOR1 = "#aabbcc";         // hover background
static const QString COLOR2 = "#112233";         // normal background

class TestIconButton : public QObject
{
	Q_OBJECT

  private:
	// IconButton::styleSheet(bgColor) shadows QWidget::styleSheet() — go through base
	static QString style(IconButton &b) { return static_cast<QWidget &>(b).styleSheet(); }

	static IconButton *makeBtn(QWidget *parent = nullptr)
	{
		// empty path: loadColoredSvg returns null QIcon gracefully
		return new IconButton("", BTN_SIZE, COLOR1, COLOR2, parent);
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

	void constructorSetsIconSize()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		QCOMPARE(btn.iconSize(), QSize(ICON_SIZE, ICON_SIZE));
	}

	void constructorAppliesNormalStyle()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		QVERIFY(style(btn).contains(COLOR2)); // normal background = color2
	}

	// -- applyNormal / applyHover via events ----------------------------------

	void enterEventAppliesHoverStyle()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		sendEnter(&btn);
		QVERIFY(style(btn).contains(COLOR1)); // hover background = color1
	}

	void leaveEventAppliesNormalStyle()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		sendEnter(&btn);
		sendLeave(&btn);
		QVERIFY(style(btn).contains(COLOR2)); // back to normal
	}

	void enterDoesNotLeakNormalColor()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		sendEnter(&btn);
		QVERIFY(!style(btn).contains(COLOR2)); // normal color absent in hover state
	}

	void leaveDoesNotLeakHoverColor()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		sendEnter(&btn);
		sendLeave(&btn);
		QVERIFY(!style(btn).contains(COLOR1)); // hover color absent in normal state
	}

	// -- unhover --------------------------------------------------------------

	void unhoverAppliesNormalStyle()
	{
		IconButton btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		sendEnter(&btn); // go to hover
		btn.unhover();
		QVERIFY(style(btn).contains(COLOR2)); // back to normal
	}

	// -- updateColors ---------------------------------------------------------

	void updateColorsAppliesNewNormalStyle()
	{
		IconButton    btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		const QString newC1 = "#ff0000";
		const QString newC2 = "#00ff00";
		btn.updateColors(newC1, newC2);
		QVERIFY(style(btn).contains(newC2)); // new normal background
	}

	void updateColorsHoverUsesNewColor1()
	{
		IconButton    btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		const QString newC1 = "#ff0000";
		const QString newC2 = "#00ff00";
		btn.updateColors(newC1, newC2);
		sendEnter(&btn);
		QVERIFY(style(btn).contains(newC1)); // new hover background
	}

	void updateColorsOldColorsNoLongerUsed()
	{
		IconButton    btn("", BTN_SIZE, COLOR1, COLOR2, nullptr);
		const QString newC1 = "#ff0000";
		const QString newC2 = "#00ff00";
		btn.updateColors(newC1, newC2);
		QVERIFY(!style(btn).contains(COLOR2)); // old normal color gone
	}
};

#include "TestIconButton.moc"

QObject *createTestIconButton()
{
	return new TestIconButton();
}
