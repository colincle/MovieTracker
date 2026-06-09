#pragma once

#include "AppStorage.hpp"
#include "Spinner.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QString>
#include <QScrollArea>

class SearchResults : public QWidget
{
    Q_OBJECT
public:
    explicit SearchResults(AppStorage &storage, QWidget *parent = nullptr);
    void search(QString query);

private:

    void setFullPageState(const QString &imagePath);

    AppStorage &appStorage;
    QVBoxLayout *layout;
    Spinner *spinner;
    QWidget *resultsContainer;
    QVBoxLayout *resultsLayout;
    QScrollArea *scrollArea;
};
