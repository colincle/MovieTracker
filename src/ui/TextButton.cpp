#include "TextButton.hpp"
#include "ColorPalette.hpp"
#include <QFont>

TextButton::TextButton(const QString &text,
                       int size,
                       QWidget *parent)
    : QPushButton(text, parent)
{
    setCursor(Qt::PointingHandCursor);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setFixedHeight(size);

    setContentsMargins(0, 0, 0, 0);

    QFont f = font();
    f.setPixelSize(static_cast<int>(size * 0.4));
    setFont(f);

    normalStyle =
        "QPushButton {"
        "   background-color: " COLOR_SURFACE ";"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 0px 14px;"
        "   color: " COLOR_ACCENT ";"
        "}";

    hoverStyle =
        "QPushButton {"
        "   background-color: " COLOR_ACCENT ";"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 0px 14px;"
        "   color: " COLOR_SURFACE ";"
        "}";

    setStyleSheet(normalStyle);
}

void TextButton::enterEvent(QEnterEvent *event)
{
    setStyleSheet(hoverStyle);
    QPushButton::enterEvent(event);
}

void TextButton::leaveEvent(QEvent *event)
{
    setStyleSheet(normalStyle);
    QPushButton::leaveEvent(event);
}