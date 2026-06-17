#include "MainWindow.hpp"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QDir::setCurrent(QCoreApplication::applicationDirPath());

	MainWindow window;
	window.show();

	return app.exec();
}
