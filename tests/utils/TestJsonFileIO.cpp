#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTest>

#include "JsonFileIO.hpp"

class TestJsonFileIO : public QObject
{
    Q_OBJECT

  private slots:

    // ── ensureDirectoryExists ────────────────────────────────────────────────

    void ensureDirectoryExistsCreatesNestedPath()
    {
        QTemporaryDir tmp;
        const QString path = tmp.filePath("a/b/c");
        QVERIFY(!QDir(path).exists());
        ensureDirectoryExists(path);
        QVERIFY(QDir(path).exists());
    }

    void ensureDirectoryExistsIsIdempotent()
    {
        QTemporaryDir tmp;
        const QString path = tmp.filePath("x");
        ensureDirectoryExists(path);
        ensureDirectoryExists(path); // already exists: must not fail
        QVERIFY(QDir(path).exists());
    }

    // ── write / read round-trip ──────────────────────────────────────────────

    void writeThenReadPreservesData()
    {
        QTemporaryDir tmp;
        const QString file = tmp.filePath("data.json");

        QJsonObject out;
        out["name"] = "Batman";
        out["year"] = 2008;

        QVERIFY(writeJsonFile(file, out));

        const QJsonObject in = readJsonFile(file);
        QCOMPARE(in["name"].toString(), QString("Batman"));
        QCOMPARE(in["year"].toInt(), 2008);
    }

    void writeJsonFileReportsSuccess()
    {
        QTemporaryDir tmp;
        QJsonObject   obj;
        obj["k"] = "v";
        QVERIFY(writeJsonFile(tmp.filePath("ok.json"), obj));
    }

    // ── read failure modes ───────────────────────────────────────────────────

    void readMissingFileReturnsEmptyObject()
    {
        QTemporaryDir tmp;
        const QJsonObject in = readJsonFile(tmp.filePath("does_not_exist.json"));
        QVERIFY(in.isEmpty());
    }

    void readInvalidJsonReturnsEmptyObject()
    {
        QTemporaryDir tmp;
        const QString file = tmp.filePath("bad.json");
        QFile         f(file);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("{ this is not valid json");
        f.close();

        const QJsonObject in = readJsonFile(file);
        QVERIFY(in.isEmpty());
    }

    // ── write failure mode ───────────────────────────────────────────────────

    void writeJsonFileFailsForUnwritablePath()
    {
        QJsonObject obj;
        obj["k"] = "v";
        // Parent directory does not exist, so QSaveFile cannot open its temp file.
        QVERIFY(!writeJsonFile("/no_such_directory_xyz/sub/data.json", obj));
    }

    // ── ensureStorageFileExists ──────────────────────────────────────────────

    void ensureStorageFileExistsCreatesMissingFile()
    {
        QTemporaryDir tmp;
        const QString file = tmp.filePath("store.json");
        QVERIFY(!QFile::exists(file));
        ensureStorageFileExists(file);
        QVERIFY(QFile::exists(file));
    }

    void ensureStorageFileExistsLeavesExistingContentUntouched()
    {
        QTemporaryDir tmp;
        const QString file = tmp.filePath("store.json");

        QFile seed(file);
        QVERIFY(seed.open(QIODevice::WriteOnly));
        seed.write("existing");
        seed.close();

        ensureStorageFileExists(file);

        QFile readback(file);
        QVERIFY(readback.open(QIODevice::ReadOnly));
        QCOMPARE(readback.readAll(), QByteArray("existing"));
    }
};

#include "TestJsonFileIO.moc"

QObject *createTestJsonFileIO() { return new TestJsonFileIO(); }
