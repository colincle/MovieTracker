#include <QTest>

#include "ElidedLabel.hpp"

class TestElidedLabel : public QObject
{
    Q_OBJECT

  private slots:

    void textFullyDisplayedWhenWideEnough()
    {
        ElidedLabel label("Hello World", 1);
        label.setFixedWidth(9999);
        label.refreshElision();
        QCOMPARE(label.text(), QString("Hello World"));
    }

    void textElidedWithEllipsisWhenTooNarrow()
    {
        ElidedLabel label("Hello World", 1);
        // Half the full-text width: too narrow for the whole string, but wide
        // enough for the ellipsis. (At width 1 even "…" doesn't fit and Qt
        // returns an empty string, so a degenerate width can't be used here.)
        const int fullWidth = label.fontMetrics().horizontalAdvance("Hello World");
        label.setFixedWidth(fullWidth / 2);
        label.refreshElision();
        QVERIFY(label.text() != QString("Hello World"));
        QVERIFY(label.text().endsWith(QString::fromUtf8("\xe2\x80\xa6"))); // "..."
    }

    void setTextUpdatesDisplayedContent()
    {
        ElidedLabel label("Original", 1);
        label.setFixedWidth(9999);
        label.setText("Updated");
        QCOMPARE(label.text(), QString("Updated"));
    }

    void minimumSizeHintHeightScalesWithMaxLines()
    {
        ElidedLabel label1("text", 1);
        ElidedLabel label3("text", 3);
        QCOMPARE(label3.minimumSizeHint().height(), label1.minimumSizeHint().height() * 3);
    }
};

#include "TestElidedLabel.moc"

QObject *createTestElidedLabel() { return new TestElidedLabel(); }
