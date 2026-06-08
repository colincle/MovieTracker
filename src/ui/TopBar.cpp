#include "TopBar.hpp"
#include "IconButton.hpp"
#include "TextButton.hpp"
#include "ColorPalette.hpp"
#include "AssetsPaths.hpp"

#include <QHBoxLayout>

TopBar::TopBar(QWidget *parent)
    : QWidget(parent)
{
    int buttonsHeight = 40;

    setStyleSheet(
        "background-color: " COLOR_BG_SECONDARY ";"
        "border-bottom: 1px solid " COLOR_BORDER ";"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // LEFT (text buttons)
    QWidget *left = new QWidget(this);
    auto *leftLayout = new QHBoxLayout(left);
    leftLayout->setContentsMargins(20, 0, 0, 0);
    leftLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftLayout->setSpacing(25);

    MoviesButton = new TextButton("Movies", buttonsHeight, this);
    TvShowsButton = new TextButton("TV shows", buttonsHeight, this);

    leftLayout->addWidget(MoviesButton);
    leftLayout->addWidget(TvShowsButton);

    layout->addWidget(left, 1);

    // MIDDLE (spacer)
    QWidget *middle = new QWidget(this);
    layout->addWidget(middle, 1);

    // RIGHT (add button)
    QWidget *right = new QWidget(this);
    auto *rightLayout = new QHBoxLayout(right);
    rightLayout->setContentsMargins(0, 0, 20, 0);
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    sortButton = new IconButton(SORT_ICON, buttonsHeight, this);
    rankButton = new IconButton(RANK_ICON, buttonsHeight, this);
    addButton = new IconButton(ADD_ICON, buttonsHeight, this);
    rightLayout->addWidget(sortButton);
    rightLayout->addWidget(rankButton);
    rightLayout->addWidget(addButton);

    layout->addWidget(right, 1);
}