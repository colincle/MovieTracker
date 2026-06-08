#include "IconButton.hpp"
#include "ColorPalette.hpp"
#include "SvgUtils.hpp"

#include <QEvent>

IconButton::IconButton(const QString &iconPath, int size,  QWidget *parent)
    : QPushButton(parent)
{
    int buttonSize = size;
    int iconSize = size / 1.5;

    normalIcon = loadColoredSvg(
        iconPath,
        COLOR_ACCENT,
        iconSize
    );

    hoverIcon = loadColoredSvg(
        iconPath,
        COLOR_SURFACE,
        iconSize
    );

    setIcon(normalIcon);

    setFixedSize(buttonSize, buttonSize);
    setIconSize(QSize(iconSize, iconSize));

    setCursor(Qt::PointingHandCursor);

    setStyleSheet(
        "QPushButton {"
        "    background-color: " COLOR_SURFACE ";"
        "    border: none;"
        "    border-radius: 6px;"

        "}"
        "QPushButton:hover {"
        "    background-color: " COLOR_ACCENT ";"
        "    border-radius: 6px;"
        "}"
    );
}

void IconButton::enterEvent(QEnterEvent *event)
{
    setIcon(hoverIcon);
    QPushButton::enterEvent(event);
}

void IconButton::leaveEvent(QEvent *event)
{
    setIcon(normalIcon);
    QPushButton::leaveEvent(event);
}