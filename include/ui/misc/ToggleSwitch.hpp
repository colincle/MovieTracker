#pragma once

#include <QAbstractButton>
#include <QPropertyAnimation>

class ToggleSwitch : public QAbstractButton
{
	Q_OBJECT
	Q_PROPERTY(qreal thumbPos READ thumbPos WRITE setThumbPos)

public:
	explicit ToggleSwitch(QWidget *parent = nullptr);

	QSize sizeHint() const override;

	qreal thumbPos() const { return m_thumbPos; }
	void setThumbPos(qreal pos) { m_thumbPos = pos; update(); }

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
	qreal m_thumbPos = 0.0;
	QPropertyAnimation *m_animation;
};
