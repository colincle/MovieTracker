#include <QApplication>
#include <QEnterEvent>
#include <QTest>

#include "HoverButton.hpp"

class TestableHoverButton : public HoverButton
{
  public:
    explicit TestableHoverButton(QWidget *parent = nullptr) : HoverButton(parent) {}

    int normalCount = 0;
    int hoverCount  = 0;

  protected:
    void applyNormal() override { ++normalCount; }
    void applyHover() override { ++hoverCount; }
};

class TestHoverButton : public QObject
{
    Q_OBJECT

  private slots:
    void enterEventCallsApplyHover()
    {
        TestableHoverButton btn;
        int                 before = btn.hoverCount;
        QEnterEvent         ev(QPointF(5, 5), QPointF(5, 5), QPointF(100, 100));
        QApplication::sendEvent(&btn, &ev);
        QCOMPARE(btn.hoverCount, before + 1);
    }

    void leaveEventCallsApplyNormal()
    {
        TestableHoverButton btn;
        int                 before = btn.normalCount;
        QEvent              ev(QEvent::Leave);
        QApplication::sendEvent(&btn, &ev);
        QCOMPARE(btn.normalCount, before + 1);
    }

    void unhoverCallsApplyNormal()
    {
        TestableHoverButton btn;
        int                 before = btn.normalCount;
        btn.unhover();
        QCOMPARE(btn.normalCount, before + 1);
    }

    void enterThenLeaveRestoresToNormal()
    {
        TestableHoverButton btn;
        QEnterEvent         enter(QPointF(5, 5), QPointF(5, 5), QPointF(100, 100));
        QApplication::sendEvent(&btn, &enter);
        int normalBefore = btn.normalCount;
        QEvent leave(QEvent::Leave);
        QApplication::sendEvent(&btn, &leave);
        QCOMPARE(btn.normalCount, normalBefore + 1);
    }

    void showEventDoesNotImmediatelyCallApplyHover()
    {
        TestableHoverButton btn;
        int                 before = btn.hoverCount;
        QShowEvent          ev;
        QApplication::sendEvent(&btn, &ev);
        // hover is only called after a 0ms timer if underMouse() is true;
        // since the button is not on screen under the cursor, it should not fire immediately
        QCOMPARE(btn.hoverCount, before);
    }
};

#include "TestHoverButton.moc"

QObject *createTestHoverButton() { return new TestHoverButton(); }
