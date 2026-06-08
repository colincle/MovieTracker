#pragma once

#include <QPushButton>
#include <QEvent>

class TextButton : public QPushButton
{
public:
    explicit TextButton(const QString &text,
                        int size,
                        QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString normalStyle;
    QString hoverStyle;
};