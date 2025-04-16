#include "mainwindow.h"
#include "start.h"

#include <QApplication>
#include <QSettings>
#include <QTranslator>

bool isGameStateSaved()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
	settings.beginGroup("Game");

	bool gameSaved = settings.contains("rows") && settings.contains("cols") && settings.contains("mineCount");

	bool allCellsEmpty = true;
	int size = settings.beginReadArray("buttons");
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		bool isRevealed = settings.value("isRevealed", false).toBool();
		bool isFlagged = settings.value("isFlagged", false).toBool();
		if (isRevealed || isFlagged)
		{
			allCellsEmpty = false;
			break;
		}
	}
	settings.endArray();
	settings.endGroup();

	return gameSaved && !allCellsEmpty;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	bool debugMode = false;
	if (argc > 1 && QString(argv[1]) == "dbg")
		debugMode = true;
	MainWindow *w = nullptr;

	if (isGameStateSaved())
	{
		QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
		settings.beginGroup("Game");
		int rows = settings.value("rows", 10).toInt();
		int cols = settings.value("cols", 10).toInt();
		int mineCount = settings.value("mineCount", 10).toInt();
		settings.endGroup();
		w = new MainWindow(rows, cols, mineCount);
		w->globalDebugMode = debugMode;
		w->loadGameState();
		w->setWindowTitle(QObject::tr("MineSweeper2024"));
	}
	else
	{
		Start startDialog;
		if (startDialog.exec() == QDialog::Accepted)
		{
			w = new MainWindow(startDialog.rows(), startDialog.cols(), startDialog.mineCount());
			w->globalDebugMode = debugMode;
			w->setWindowTitle(QObject::tr("MineSweeper2024"));
			QString initialLang = startDialog.languageComboBox->currentData().toString();
			if (!initialLang.isEmpty())
				w->changeLanguage(initialLang);
		}
	}

	if (w != nullptr)
	{
		w->show();
		int result = a.exec();
		w->saveGameState();
		delete w;
		return result;
	}
	else
		return 0;
}
//
