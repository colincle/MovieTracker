#include "SeasonUpdate.hpp"

#include <QDate>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

static constexpr int RECHECK_INTERVAL_DAYS = 14;

SeasonUpdate::SeasonUpdate(AppStorage &appStorage, QObject *parent)
	: QObject(parent)
	, appStorage(appStorage)
{
	QMutexLocker locker(&appStorage.getMutex());

	for(Title &t : appStorage.getTitlesMutable())
		if(isEligible(t))
		{
			titles.push_back(&t);
		}

	if(titles.empty())
	{
		queueEmpty = true;
	}
}

bool SeasonUpdate::isEligible(const Title &t) const
{
	if(!t.isSeries)
{
	return false;
}

return t.lastChecked.daysTo(QDate::currentDate()) > RECHECK_INTERVAL_DAYS;
}

static bool isAuthError(const QString &message)
{
	return message.contains("api key", Qt::CaseInsensitive)
	       || message.contains("authentication", Qt::CaseInsensitive);
}

void SeasonUpdate::updateSeries()
{
	QMutexLocker locker(&appStorage.getMutex());

	QNetworkAccessManager manager;

	for(Title *t : titles)
	{
		QUrl url("https://omdbapi.com/");
		QUrlQuery query;
		query.addQueryItem("apikey", appStorage.getKey());
		query.addQueryItem("i", t->imdbId);
		url.setQuery(query);

		QEventLoop loop;
		QNetworkReply *reply = manager.get(QNetworkRequest(url));
		QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		if(reply->error() != QNetworkReply::NoError)
		{
			const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			const QString message = reply->errorString();
			reply->deleteLater();

			if(status == 401 || isAuthError(message))
			{
				emit apiKeyError();
			}
			else
			{
				emit networkError();
			}

			return;
		}

		QByteArray data = reply->readAll();
		reply->deleteLater();

		QJsonObject root = QJsonDocument::fromJson(data).object();

		if(root["Response"].toString() == "False")
		{
			if(isAuthError(root["Error"].toString()))
			{
				emit apiKeyError();
				return;
			}

			continue;
		}

		t->lastChecked = QDate::currentDate();

		int newSeasons = root["totalSeasons"].toString().toInt();
		int oldSeasons = t->totalSeasons.toInt();

		if(newSeasons > oldSeasons)
		{
			t->totalSeasons = QString::number(newSeasons);
			t->viewed = false;
		}
	}

	appStorage.save();
}