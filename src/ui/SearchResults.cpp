#include "SearchResults.hpp"
#include "OmdbSearch.hpp"
#include "AppStorage.hpp"
#include "ColorPalette.hpp"
#include "Spinner.hpp"
#include "IconButton.hpp"
#include "AssetsPaths.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QFrame>
#include <QScrollBar>

void SearchResults::setFullPageState(const QString &imagePath)
{
    while (QLayoutItem *item = resultsLayout->takeAt(0))
    {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    scrollArea->hide();

    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem *item = layout->itemAt(i);
        if (item->widget() && item->widget() != spinner && item->widget() != scrollArea)
        {
            item->widget()->deleteLater();
            layout->removeItem(item);
        }
    }

    auto *label = new QLabel(this);
    label->setPixmap(QPixmap(imagePath));
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setMinimumSize(1, 1);
    label->setScaledContents(false);

    layout->addWidget(label, 1);
}

SearchResults::SearchResults(AppStorage &storage, QWidget *parent)
    : appStorage(storage)
    , QWidget(parent)
{
    setStyleSheet(
        "background-color: " COLOR_BG_PRIMARY ";"
    );

    setAttribute(Qt::WA_StyledBackground, true);

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    spinner = new Spinner(COLOR_ACCENT, 8, this);
    spinner->setFixedSize(48, 48);
    spinner->hide();

    layout->addWidget(spinner, 0, Qt::AlignCenter);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->hide();

    resultsContainer = new QWidget;
    resultsLayout = new QVBoxLayout(resultsContainer);

    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->setSpacing(12);

    scrollArea->setWidget(resultsContainer);

    layout->addWidget(scrollArea);
}

void SearchResults::search(QString query)
{
    spinner->show();
    scrollArea->hide();

    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem *item = layout->itemAt(i);
        if (item->widget() && item->widget() != spinner && item->widget() != scrollArea)
        {
            item->widget()->deleteLater();
            layout->removeItem(item);
        }
    }

    scrollArea->verticalScrollBar()->setValue(0);

    while (QLayoutItem *item = resultsLayout->takeAt(0))
    {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    auto *search =
        new OmdbSearch(
            query,
            appStorage.getKey(),
            this
        );

    connect(search, &OmdbSearch::searchFinished, this,
        [this, search]()
        {
            spinner->hide();

            const results &r = search->getResults();

            if (!r.error.isEmpty())
            {
                if (r.error == "Host requires authentication")
                    setFullPageState(API_KEY_ERROR);
                else
                    setFullPageState(NO_MOVIES_FOUND);

                search->deleteLater();
                return;
            }

            for (const resultTitle &title : r.titles)
            {
                auto *row = new QWidget;

                row->setStyleSheet(
                    "background-color: " COLOR_BG_SECONDARY ";"
                    "border: 1px solid " COLOR_BORDER ";"
                    "border-radius: 10px;"
                );

                auto *rowLayout = new QHBoxLayout(row);

                rowLayout->setContentsMargins(12, 12, 12, 12);
                rowLayout->setSpacing(20);

                auto *poster = new QLabel;
                poster->setFixedSize(100, 150);
                poster->setStyleSheet("border: none; background: transparent;");

                poster->setPixmap(
                    title.posterImage.scaled(
                        poster->size(),
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                    )
                );

                auto *name = new QLabel(title.title);
                name->setStyleSheet(
                    "color: " COLOR_TEXT_PRIMARY ";"
                    "font-size: 24px;"
                    "font-weight: bold;"
                    "border: none;"
                    "background: transparent;"
                );
                name->setWordWrap(true);

                auto *addButton =
                    new IconButton(ADD_ICON, 40, COLOR_ACCENT, COLOR_SURFACE, row);

                connect(addButton, &QPushButton::clicked, this,
                    [this, title, addButton, row]()
                    {
                        auto *spinner = new Spinner(COLOR_ACCENT, 6, row);
                        spinner->setFixedSize(40, 40);

                        QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(row->layout());
                        rowLayout->replaceWidget(addButton, spinner);
                        addButton->hide();

                        auto *fetch = new OmdbSearch("", appStorage.getKey(), this);
                        fetch->fetchById(title.imdbId);

                        connect(fetch, &OmdbSearch::titleFetched, this,
                            [this, spinner, addButton, row, fetch](const Title &t)
                            {
                                QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(row->layout());

                                auto *doneButton = new IconButton(ADDED_ICON, 40, COLOR_SUCCESS, COLOR_SURFACE, row);

                                rowLayout->replaceWidget(spinner, doneButton);
                                spinner->deleteLater();
                                addButton->deleteLater();

                                fetch->deleteLater();

                                qDebug() << "Fetched:" << t.title;
                            }
                        );
                    }
                );

                rowLayout->addWidget(poster);
                rowLayout->addWidget(name);
                rowLayout->addStretch();
                rowLayout->addWidget(addButton);

                resultsLayout->addWidget(row);
            }

            resultsLayout->addStretch();

            scrollArea->show();

            search->deleteLater();
        }
    );

    search->search();
}