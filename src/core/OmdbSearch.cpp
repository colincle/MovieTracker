#include "OmdbSearch.hpp"
#include "AssetsPaths.hpp"
#include "Title.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QPixmap>

OmdbSearch::OmdbSearch(QString query,
                       QString key,
                       QObject *parent)
    : apiKey(key), QObject(parent)
{
    query.replace(' ', '+');
    requestUrl = baseUrl + apiKey + titleSearch + query;
}

const results& OmdbSearch::getResults() const
{
    return searchResults;
}

void OmdbSearch::search()
{
    QNetworkReply *reply =
        networkManager.get(QNetworkRequest(QUrl(requestUrl)));

    connect(reply,
            &QNetworkReply::finished,
            this,
            &OmdbSearch::onReplyFinished);
}

void OmdbSearch::onReplyFinished()
{
    QNetworkReply *reply =
        qobject_cast<QNetworkReply*>(sender());

    searchResults.error.clear();
    searchResults.titles.clear();

    if (!reply)
    {
        searchResults.error = "Internal error";
        emit searchFinished();
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        searchResults.error = reply->errorString();

        reply->deleteLater();
        emit searchFinished();
        return;
    }

    QJsonDocument doc =
        QJsonDocument::fromJson(reply->readAll());

    QJsonObject root = doc.object();

    if (root["Response"].toString() == "False")
    {
        searchResults.error = root["Error"].toString();

        reply->deleteLater();
        emit searchFinished();
        return;
    }

    QJsonArray titles = root["Search"].toArray();

    for (const QJsonValue &value : titles)
    {
        QJsonObject obj = value.toObject();

        resultTitle title;

        title.title  = obj["Title"].toString();
        title.year   = obj["Year"].toString();
        title.imdbId = obj["imdbID"].toString();
        title.type   = obj["Type"].toString();
        title.poster = obj["Poster"].toString();

        searchResults.titles.push_back(title);
    }

    reply->deleteLater();

    pendingPosters = static_cast<int>(searchResults.titles.size());

    if (pendingPosters == 0)
    {
        emit searchFinished();
        return;
    }

    for (int i = 0; i < static_cast<int>(searchResults.titles.size()); ++i)
    {
        if (searchResults.titles[i].poster == "N/A")
        {
            searchResults.titles[i]
                .posterImage
                .load(POSTER_PLACEHOLDER);

            if (--pendingPosters == 0)
                emit searchFinished();

            continue;
        }

        QNetworkReply *posterReply =
            networkManager.get(
                QNetworkRequest(
                    QUrl(searchResults.titles[i].poster)));

        connect(
            posterReply,
            &QNetworkReply::finished,
            this,
            [this, i, posterReply]()
            {
                if (posterReply->error() == QNetworkReply::NoError)
                {
                    if (!searchResults.titles[i]
                            .posterImage
                            .loadFromData(
                                posterReply->readAll()))
                    {
                        searchResults.titles[i]
                            .posterImage
                            .load(POSTER_PLACEHOLDER);
                    }
                }
                else
                {
                    searchResults.titles[i]
                        .posterImage
                        .load(POSTER_PLACEHOLDER);
                }

                posterReply->deleteLater();

                if (--pendingPosters == 0)
                    emit searchFinished();
            }
        );
    }
}

void OmdbSearch::fetchById(const QString &imdbId)
{
    QString url = baseUrl + apiKey + idSearch + imdbId;

    QNetworkReply *reply =
        networkManager.get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, this,
        [this, reply]()
        {
            if (reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            QJsonObject root =
                QJsonDocument::fromJson(reply->readAll()).object();

            reply->deleteLater();

            if (root["Response"].toString() == "False")
                return;

            Title t;
            t.title        = root["Title"].toString();
            t.year         = root["Year"].toString();
            t.imdbId       = root["imdbID"].toString();
            t.type         = root["Type"].toString();
            t.released     = root["Released"].toString();
            t.plot         = root["Plot"].toString();
            t.director     = root["Director"].toString();
            t.actors       = root["Actors"].toString();
            t.posterUrl    = root["Poster"].toString();
            t.totalSeasons = root["totalSeasons"].toString();
            t.isMovie      = t.type == "movie";
            t.isSeries     = t.type == "series";

            emit titleFetched(t);
        }
    );
}