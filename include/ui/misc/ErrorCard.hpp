#pragma once

#include "IconButton.hpp"

#include <QLabel>
#include <QWidget>

class ErrorCard : public QWidget
{
	Q_OBJECT

public:
	explicit ErrorCard(QWidget *parent, const QString &message);

	void show();
	void setMessage(const QString &message);

private:
	QLabel *label;
};
