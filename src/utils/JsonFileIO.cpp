#include "JsonFileIO.hpp"

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>

void ensureDirectoryExists(const QString &path)
{
	QDir dir(path);

	if(!dir.exists())
	{
		dir.mkpath(path);
	}
}

void ensureStorageFileExists(const QString &filePath)
{
	QFile file(filePath);

	if(file.exists())
	{
		return;
	}

	if(file.open(QIODevice::WriteOnly))
	{
		file.write("{\n\t\"omdbApiKey\": \"\",\n\t\"titles\": []\n}\n");
		file.close();
	}
}

QJsonObject readJsonFile(const QString &filePath)
{
	QFile file(filePath);

	if(!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Failed to open file for reading:" << filePath;
		return {};
	}

	QByteArray data = file.readAll();
	file.close();

	return QJsonDocument::fromJson(data).object();
}

bool writeJsonFile(const QString &filePath, const QJsonObject &root)
{
	QFile file(filePath);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		qWarning() << "Failed to open file for writing:" << filePath;
		return false;
	}

	file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
	file.close();
	return true;
}
