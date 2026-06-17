#pragma once

#include <QLineEdit>

class SearchBar : public QLineEdit
{
	Q_OBJECT

  public:
	explicit SearchBar(QWidget *parent = nullptr);

  signals:
	void escapePressed();

  protected:
	void keyPressEvent(QKeyEvent *event) override;
};
