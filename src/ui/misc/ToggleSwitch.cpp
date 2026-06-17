#include "ToggleSwitch.hpp"
#include "Palette.hpp"

#include <QMouseEvent>
#include <QPainter>

static constexpr int TRACK_W = 48;
static constexpr int TRACK_H = 26;
static constexpr int THUMB_D = 20;
static constexpr int THUMB_MARGIN = 3;

ToggleSwitch::ToggleSwitch(QWidget *parent)
	: QAbstractButton(parent)
{
	setCheckable(true);
	setFixedSize(sizeHint());
	setCursor(Qt::PointingHandCursor);

	m_animation = new QPropertyAnimation(this, "thumbPos", this);
	m_animation->setDuration(150);
	m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

QSize ToggleSwitch::sizeHint() const
{
	return QSize(TRACK_W, TRACK_H);
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	const QColor trackOn(Palette::accent);
	const QColor trackOff(Palette::surface);

	QColor track;
	track.setRed(trackOff.red() + m_thumbPos * (trackOn.red() - trackOff.red()));
	track.setGreen(trackOff.green() + m_thumbPos * (trackOn.green() - trackOff.green()));
	track.setBlue(trackOff.blue() + m_thumbPos * (trackOn.blue() - trackOff.blue()));

	p.setBrush(track);
	p.setPen(QPen(QColor(Palette::border), 1.5));
	p.drawRoundedRect(QRectF(0, 0, TRACK_W, TRACK_H), TRACK_H / 2.0, TRACK_H / 2.0);

	const qreal travel = TRACK_W - THUMB_MARGIN * 2 - THUMB_D;
	const qreal thumbX = THUMB_MARGIN + m_thumbPos * travel;
	p.setBrush(QColor(Palette::textPrimary));
	p.setPen(Qt::NoPen);
	p.drawEllipse(QRectF(thumbX, THUMB_MARGIN, THUMB_D, THUMB_D));
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event)
{
	if(event->button() != Qt::LeftButton)
	{
		return;
	}

	const bool next = !isChecked();
	setChecked(next);

	m_animation->stop();
	m_animation->setStartValue(m_thumbPos);
	m_animation->setEndValue(next ? 1.0 : 0.0);
	m_animation->start();

	emit clicked(next);
}
