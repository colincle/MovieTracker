#include <QColor>
#include <QFile>
#include <QIcon>
#include <QImage>
#include <QPixmap>
#include <QTemporaryDir>
#include <QTest>

#include "SvgUtils.hpp"

class TestSvgUtils : public QObject
{
    Q_OBJECT

  private:
    // Writes a fully-opaque square SVG and returns its path.
    static QString writeSquareSvg(QTemporaryDir &dir)
    {
        const QString path = dir.filePath("square.svg");
        QFile         f(path);
        if(!f.open(QIODevice::WriteOnly))
            return {};
        f.write(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\">"
            "<rect width=\"24\" height=\"24\" fill=\"#000000\"/></svg>"
        );
        f.close();
        return path;
    }

  private slots:

    void missingFileReturnsNullIcon()
    {
        const QIcon icon = loadColoredSvg("/no_such_file_xyz.svg", "#ff0000", 24);
        QVERIFY(icon.isNull());
    }

    void validSvgReturnsNonNullIcon()
    {
        QTemporaryDir dir;
        const QIcon   icon = loadColoredSvg(writeSquareSvg(dir), "#ff0000", 24);
        QVERIFY(!icon.isNull());
    }

    void iconPixmapHasRequestedSize()
    {
        QTemporaryDir dir;
        const QIcon   icon = loadColoredSvg(writeSquareSvg(dir), "#ff0000", 24);
        QCOMPARE(icon.pixmap(24, 24).size(), QSize(24, 24));
    }

    void opaqueAreaIsRecoloredToRequestedColor()
    {
        QTemporaryDir dir;
        const QIcon   icon = loadColoredSvg(writeSquareSvg(dir), "#ff0000", 24);

        // SourceIn compositing tints the opaque square to the requested color.
        const QColor center = icon.pixmap(24, 24).toImage().pixelColor(12, 12);
        QCOMPARE(center.red(), 255);
        QCOMPARE(center.green(), 0);
        QCOMPARE(center.blue(), 0);
    }

    void differentColorProducesDifferentTint()
    {
        QTemporaryDir dir;
        const QString svg = writeSquareSvg(dir);

        const QColor red = loadColoredSvg(svg, "#ff0000", 24).pixmap(24, 24).toImage().pixelColor(12, 12);
        const QColor blue = loadColoredSvg(svg, "#0000ff", 24).pixmap(24, 24).toImage().pixelColor(12, 12);

        QVERIFY(red != blue);
        QCOMPARE(blue.blue(), 255);
        QCOMPARE(blue.red(), 0);
    }
};

#include "TestSvgUtils.moc"

QObject *createTestSvgUtils() { return new TestSvgUtils(); }
