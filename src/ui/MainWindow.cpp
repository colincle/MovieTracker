#include "MainWindow.hpp"
#include "TopBar.hpp"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupMenuBar();

    setMinimumSize(800, 600);
    resize(1200, 800);
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    topBar = new TopBar;
    addBar = new AddBar;
    searchResults = new SearchResults(appStorage);
    addBar->hide();
    searchResults->hide();
    searchResults->hide();

    connectMainWindow();

    topBar->setFixedHeight(70);
    addBar->setFixedHeight(70);

    layout->addWidget(topBar);
    layout->addWidget(addBar);
    layout->addWidget(searchResults);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    QMenu *fileMenu = menuBar->addMenu("Omdb API key");
    QAction *setKey = new QAction("Set API Key", this);
    fileMenu->addAction(setKey);

    connect(setKey, &QAction::triggered, this, [this]() {

        bool ok = false;

        QString key = QInputDialog::getText(
            this,
            "OMDb API Key",
            "Enter your API key:",
            QLineEdit::Normal,
            "",
            &ok
        );

        if (ok && !key.isEmpty())
        {
            appStorage.setOmdbApiKey(key);
        }
    });
}

void MainWindow::connectMainWindow()
{
    connect(topBar, &TopBar::requestAddMode, this, [this]() {
        topBar->hide();
        addBar->show();
        searchResults->show();

    });
    connect(addBar, &AddBar::requestNormalMode, this, [this]() {
        topBar->show();
        addBar->hide();
        searchResults->hide();
    });
    connect(addBar, &AddBar::searchRequested, this, [this](const QString &query) {
        searchResults->search(query);
    });
}