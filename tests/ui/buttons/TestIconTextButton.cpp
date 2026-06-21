#include <QApplication>
#include <QEnterEvent>
#include <QSize>
#include <QTest>

#include "IconTextButton.hpp"

static constexpr int SIZE      = 40;
static constexpr int ICON_SIZE = SIZE / 1.5; // 26
static const QString COLOR1    = "#aabbcc";
static const QString COLOR2    = "#112233";
static const QString TEXT      = "Click me";

class TestIconTextButton : public QObject
{
    Q_OBJECT

  private:
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

    // -- constructor (alwaysShowText=false) ------------------------------------

    void constructorCollapsedWidthIsSizeWhenNotAlwaysShow()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        QCOMPARE(btn.width(), SIZE);
    }

    void constructorTextIsEmptyWhenNotAlwaysShow()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        QVERIFY(btn.text().isEmpty());
    }

    void constructorAppliesNormalStylesheet()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    void iconSizeIsHeightDividedByOnePointFive()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        QCOMPARE(btn.iconSize(), QSize(ICON_SIZE, ICON_SIZE));
    }

    // -- constructor (alwaysShowText=true) ------------------------------------

    void constructorAlwaysShowTextExpandsWidth()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, true);
        QVERIFY(btn.width() > SIZE);
    }

    void constructorAlwaysShowTextDisplaysText()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, true);
        QCOMPARE(btn.text(), TEXT);
    }

    void constructorAlwaysShowTextAppliesNormalStylesheet()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, true);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    // -- textOnLeft -----------------------------------------------------------

    void textOnLeftSetsRightToLeftLayoutDirection()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false, true);
        QCOMPARE(btn.layoutDirection(), Qt::RightToLeft);
    }

    void textOnLeftFalseKeepsLeftToRightLayoutDirection()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false, false);
        QCOMPARE(btn.layoutDirection(), Qt::LeftToRight);
    }

    // -- enterEvent -----------------------------------------------------------

    void enterEventAppliesHoverStylesheet()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        sendEnter(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
    }

    void enterEventSetsTextWhenNotAlwaysShow()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        sendEnter(&btn);
        QCOMPARE(btn.text(), TEXT);
    }

    void enterDoesNotUseNormalBgColor()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        sendEnter(&btn);
        QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR2));
    }

    // -- leaveEvent -----------------------------------------------------------

    void leaveEventAppliesNormalStylesheet()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        sendEnter(&btn);
        sendLeave(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    void leaveDoesNotUseHoverBgColor()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        sendEnter(&btn);
        sendLeave(&btn);
        QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR1));
    }

    // -- showEvent ------------------------------------------------------------

    void showEventDoesNotImmediatelyApplyHover()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        QShowEvent     ev;
        QApplication::sendEvent(&btn, &ev);
        // singleShot(0,...) only calls applyHover() if underMouse(), which is false off-screen
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    // -- updateColors ---------------------------------------------------------

    void updateColorsAppliesNewNormalStylesheet()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        const QString  newC1 = "#ff0000";
        const QString  newC2 = "#00ff00";
        btn.updateColors(newC1, newC2);
        QVERIFY(btn.styleSheet().contains("background-color: " + newC2));
    }

    void updateColorsHoverUsesNewColor1()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        const QString  newC1 = "#ff0000";
        const QString  newC2 = "#00ff00";
        btn.updateColors(newC1, newC2);
        sendEnter(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + newC1));
    }

    void updateColorsOldColorsNoLongerUsed()
    {
        IconTextButton btn("", TEXT, SIZE, COLOR1, COLOR2, false);
        btn.updateColors("#ff0000", "#00ff00");
        QVERIFY(!btn.styleSheet().contains(COLOR1));
        QVERIFY(!btn.styleSheet().contains(COLOR2));
    }
};

#include "TestIconTextButton.moc"

QObject *createTestIconTextButton() { return new TestIconTextButton(); }
