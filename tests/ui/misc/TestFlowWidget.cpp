#include <QCoreApplication>
#include <QPushButton>
#include <QTest>

#include "FlowWidget.hpp"

class TestFlowWidget : public QObject
{
	Q_OBJECT

  private slots:

	void addedWidgetBecomesChild()
	{
		FlowWidget  fw(8);
		QPushButton btn;
		fw.addWidget(&btn);
		QCOMPARE(btn.parent(), &fw);
	}

	void showWithWidgetsDoesNotCrash()
	{
		FlowWidget fw(8);
		fw.setFixedWidth(300);
		auto *btn1 = new QPushButton("One");
		auto *btn2 = new QPushButton("Two");
		fw.addWidget(btn1);
		fw.addWidget(btn2);
		fw.show();
		QCoreApplication::processEvents();
	}
};

#include "TestFlowWidget.moc"

QObject *createTestFlowWidget()
{
	return new TestFlowWidget();
}
