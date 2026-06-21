#include <QApplication>
#include <QEnterEvent>
#include <QTest>

#include "TextButton.hpp"

static constexpr int SIZE  = 32;
static const QString COLOR1 = "#aabbcc"; // active/hover background
static const QString COLOR2 = "#112233"; // normal background
static const QString TEXT   = "Movies";

class TestTextButton : public QObject
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

    // -- constructor ----------------------------------------------------------

    void constructorSetsText()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        QCOMPARE(btn.text(), TEXT);
    }

    void constructorIsNotActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        QVERIFY(!btn.isActive());
    }

    void constructorAppliesNormalStylesheet()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    // -- enterEvent -----------------------------------------------------------

    void enterEventAppliesActiveStyleWhenNotActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        sendEnter(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
    }

    void enterDoesNotRetainNormalBgColor()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        sendEnter(&btn);
        QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR2));
    }

    void enterEventDoesNotChangeStyleWhenActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.toggleActive(); // active=true, activeStyle (bg=color1)
        sendEnter(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
    }

    // -- leaveEvent -----------------------------------------------------------

    void leaveEventAppliesNormalStyleWhenNotActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        sendEnter(&btn);
        sendLeave(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    void leaveDoesNotRetainActiveBgColor()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        sendEnter(&btn);
        sendLeave(&btn);
        QVERIFY(!btn.styleSheet().contains("background-color: " + COLOR1));
    }

    void leaveEventDoesNotChangeStyleWhenActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.toggleActive(); // active=true, activeStyle (bg=color1)
        sendLeave(&btn);
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
    }

    // -- toggleActive ---------------------------------------------------------

    void toggleActiveFlipsIsActive()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        QVERIFY(!btn.isActive());
        btn.toggleActive();
        QVERIFY(btn.isActive());
        btn.toggleActive();
        QVERIFY(!btn.isActive());
    }

    void toggleActiveFirstCallAppliesActiveStyle()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.toggleActive();
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR1));
    }

    void toggleActiveSecondCallAppliesNormalStyle()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.toggleActive();
        btn.toggleActive();
        QVERIFY(btn.styleSheet().contains("background-color: " + COLOR2));
    }

    // -- updateColors ---------------------------------------------------------

    void updateColorsWhenNotActiveAppliesNewNormalStyle()
    {
        TextButton    btn(TEXT, SIZE, COLOR1, COLOR2);
        const QString newC1 = "#ff0000";
        const QString newC2 = "#00ff00";
        btn.updateColors(newC1, newC2);
        QVERIFY(btn.styleSheet().contains("background-color: " + newC2));
    }

    void updateColorsWhenActiveAppliesNewActiveStyle()
    {
        TextButton    btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.toggleActive();
        const QString newC1 = "#ff0000";
        const QString newC2 = "#00ff00";
        btn.updateColors(newC1, newC2);
        QVERIFY(btn.styleSheet().contains("background-color: " + newC1));
    }

    void updateColorsOldColorsNoLongerUsed()
    {
        TextButton btn(TEXT, SIZE, COLOR1, COLOR2);
        btn.updateColors("#ff0000", "#00ff00");
        QVERIFY(!btn.styleSheet().contains(COLOR1));
        QVERIFY(!btn.styleSheet().contains(COLOR2));
    }
};

#include "TestTextButton.moc"

QObject *createTestTextButton() { return new TestTextButton(); }
