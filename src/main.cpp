#include "MainWindow.hpp"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// Pin the name so the data dir (and thus backup zips) is "ReWatch" on every
	// platform, instead of defaulting to the executable/AppImage filename.
	QCoreApplication::setApplicationName("ReWatch");

	QDir::setCurrent(QCoreApplication::applicationDirPath());

	MainWindow window;
	window.show();

	return app.exec();
}
