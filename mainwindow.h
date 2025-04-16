#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "numberbutton.h"

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <QTranslator>
#include <QVector>

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	bool peekMode = false;
	bool globalDebugMode;
	bool win = false;
	bool leftHandedMode = false;
	int flagCount = 0;

	MainWindow(int rows, int cols, int mines, QWidget *parent = nullptr);
	void click_mine(NumberButton *button);
	void checkVictory(int index);
	void showAllMines(int clickedIndex);
	void handleMiddleClick(int index);
	void changeLanguage(const QString &language);

	void game_end();
	void updateMinesLeftLabel();
	void saveGameState();
	void loadGameState();
	bool isDebugMode();
	~MainWindow();

  private slots:
	void onPeekModeToggled(bool checked);
	void onModeLeftHand(bool checked);

  private:
	QVector< NumberButton * > *buttons;
	QTranslator translator;
	QLabel *minesLeftLabel;
	int n, m;
	int buttonSize = 30;
	int spacing = 5;
	int mineCount = 0;
	bool dbgModeEnabled = false;
	QGridLayout *gridlayout;
	QToolBar *toolBar;
	QWidget *centralWidget;
	bool minesPlaced = false;
	bool flag = true;

	void resizeEvent(QResizeEvent *event) override;
	void handleButtonClick(int index);
	void highlightCells(const QVector< int > &neighbors);
	void setupToolBar(bool debugMode);
	void placeMines(int clickedIndex);
	void revealEmptyCells(int index);
	void click_num(NumberButton *button);
	void gameOver(int index);
	void calculateNumbers();
	int countAdjacentMines(int row, int col);

	void retranslateUi();
	void clearHighlights();
	void newGame();
	void newGameWithSettings();
	void setupCentralWidget();
	void setupGrid();
	void setupMenu();
};
#endif	  // MAINWINDOW_H
//
