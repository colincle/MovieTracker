#include "MainWindow.hpp"
#include "TopBar.hpp"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(800, 600);
    resize(1200, 800);
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *top = new TopBar;
    auto *bottom = new QWidget;

    top->setFixedHeight(70);

    layout->addWidget(top);
    layout->addWidget(bottom);
}