#pragma once

#include "AppStorage.hpp"
#include "AddBar.hpp"
#include "AppMenuBar.hpp"
#include "LibraryView.hpp"
#include "SearchResults.hpp"
#include "TopBar.hpp"
#include "ErrorCard.hpp"
#include "SeasonUpdate.hpp"

#include <QMainWindow>
#include <QTimer>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

private:
	AppStorage appStorage;
	AppMenuBar *appMenuBar;
	ErrorCard *errorCard;
	TopBar *topBar;
	AddBar *addBar;
	SearchResults *searchResults;
	LibraryView *libraryView;
	QTimer *seasonRetryTimer;

	QWidget *makeSeasonOverlay();
	void setupLayout();
	void setupErrorCard();
	void setupMenuBar();
	void setupShortcuts();
	void connectSignals();
	void setupSeasonRetryTimer();
	void checkConnectivityAndRetry();
	void runSeasonUpdate();

	void enterAddMode();
	void enterNormalMode();
};
