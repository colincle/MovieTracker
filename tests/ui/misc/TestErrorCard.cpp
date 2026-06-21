#include <QLabel>
#include <QTest>

#include "ErrorCard.hpp"
#include "HoverButton.hpp"

class TestErrorCard : public QObject
{
    Q_OBJECT

  private slots:

    void initiallyHidden()
    {
        ErrorCard card(nullptr, "Something went wrong");
        QVERIFY(card.isHidden());
    }

    void closeButtonHidesCard()
    {
        ErrorCard card(nullptr, "Something went wrong");
        card.show();
        auto *closeBtn = card.findChild<HoverButton *>();
        QVERIFY(closeBtn);
        closeBtn->click();
        QVERIFY(card.isHidden());
    }

    void setMessageUpdatesLabel()
    {
        ErrorCard card(nullptr, "Initial message");
        card.setMessage("Updated message");
        auto *label = card.findChild<QLabel *>();
        QVERIFY(label);
        QVERIFY(label->text().contains("Updated message"));
    }
};

#include "TestErrorCard.moc"

QObject *createTestErrorCard() { return new TestErrorCard(); }
