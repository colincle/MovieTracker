#pragma once

#include "AppStorage.hpp"

#include <QDialog>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>

class SettingsWindow : public QDialog
{
	Q_OBJECT

  public:
	explicit SettingsWindow(AppStorage &appStorage, QWidget *parent = nullptr);

  private:
	AppStorage &appStorage;
	QPushButton *lightTab;
	QPushButton *darkTab;
	QLineEdit *apiKeyEdit;
	QPushButton *applyButton;

	void setupUi();
	QWidget *makeThemeSection();
	QWidget *makeApiKeySection();
	QFrame *makeSeparator();

	void onApplyClicked();
	void refreshStyle();

  signals:
	void themeChanged(const QString &theme);
};
