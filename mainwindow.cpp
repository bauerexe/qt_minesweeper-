#include "mainwindow.h"
#include "start.h"

#include <QApplication>
#include <QCoreApplication>
#include <QWidgetAction>

QVector< QString > arr = { ":/images/1.png", ":/images/2.png", ":/images/3.png", ":/images/4.png",
						   ":/images/5.png", ":/images/6.png", ":/images/7.png", ":/images/8.png" };

MainWindow::MainWindow(int rows, int cols, int mines, QWidget *parent) :
	QMainWindow(parent), n(rows), m(cols), mineCount(mines)
{
	this->setStyleSheet("background-color: #44C19A;");
	setupMenu();
	setupToolBar(globalDebugMode);
	setupCentralWidget();
	setupGrid();
}

void MainWindow::setupToolBar(bool globalDebugMode)
{
	toolBar = new QToolBar(this);
	addToolBar(toolBar);

	toolBar->setContentsMargins(10, 5, 10, 5);
	toolBar->addSeparator();
	QAction *newGameAction = new QAction(tr("New Game"), this);
	connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);

	toolBar->addAction(newGameAction);
	toolBar->addSeparator();
	QAction *newGameWithSettingsAction = new QAction(tr("New Game with Settings"), this);
	connect(newGameWithSettingsAction, &QAction::triggered, this, &MainWindow::newGameWithSettings);

	toolBar->addAction(newGameWithSettingsAction);
	toolBar->addSeparator();
	QCheckBox *peekCheckBox = new QCheckBox(tr("Left-handed"), this);

	toolBar->addWidget(peekCheckBox);
	connect(peekCheckBox, &QCheckBox::toggled, this, &MainWindow::onModeLeftHand);
	toolBar->addSeparator();
	if (globalDebugMode)
	{
		QCheckBox *peekCheckBox = new QCheckBox(tr("Dbg Mode"), this);
		peekCheckBox->setObjectName("dbgModeCheckBox");
		toolBar->addWidget(peekCheckBox);
		toolBar->addSeparator();
		connect(peekCheckBox, &QCheckBox::toggled, this, &MainWindow::onPeekModeToggled);
	}

	minesLeftLabel = new QLabel(this);
	updateMinesLeftLabel();
	toolBar->addWidget(minesLeftLabel);
}

void MainWindow::setupCentralWidget()
{
	QWidget *outerWidget = new QWidget(this);
	QVBoxLayout *outerLayout = new QVBoxLayout(outerWidget);
	outerLayout->setAlignment(Qt::AlignCenter);
	outerWidget->setStyleSheet("background-color: #2F4F4F;");

	centralWidget = new QWidget(this);
	gridlayout = new QGridLayout(centralWidget);
	gridlayout->setSpacing(spacing);
	gridlayout->setAlignment(Qt::AlignCenter);
	centralWidget->setStyleSheet("background-color: #B5B8B1;");
	centralWidget->setMinimumSize(n * buttonSize + 40, m * buttonSize + 40);
	outerLayout->addWidget(centralWidget);
	setCentralWidget(outerWidget);
}

void MainWindow::setupGrid()
{
	buttons = new QVector< NumberButton * >();
	for (int i = 0; i < n * m; i++)
	{
		NumberButton *button1 = new NumberButton(0, i);
		button1->setIconSize(button1->size());
		button1->setFixedSize(buttonSize, buttonSize);
		buttons->append(button1);
		connect(button1, &QPushButton::clicked, this, [this, i]() { handleButtonClick(i); });
	}
	updateMinesLeftLabel();

	int button_index = 0;
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < m; col++)
		{
			gridlayout->addWidget((*buttons)[button_index], row, col);
			button_index++;
		}
	}

	centralWidget->setLayout(gridlayout);
	centralWidget->setMinimumSize(buttonSize * m + 40, buttonSize * n + 40);
}

void MainWindow::setupMenu()
{
	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *gameMenu = new QMenu(tr("Game"), this);
	menuBar->addMenu(gameMenu);

	QAction *newGameAction = new QAction(tr("New Game"), this);
	connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);
	gameMenu->addAction(newGameAction);

	QAction *newGameWithSettingsAction = new QAction(tr("New Game with Settings"), this);
	connect(newGameWithSettingsAction, &QAction::triggered, this, &MainWindow::newGameWithSettings);
	gameMenu->addAction(newGameWithSettingsAction);

	QAction *exitAction = new QAction(tr("Exit"), this);
	connect(exitAction, &QAction::triggered, this, &MainWindow::close);
	gameMenu->addAction(exitAction);

	QMenu *languageMenu = new QMenu(tr("Language"), this);
	menuBar->addMenu(languageMenu);

	QAction *englishAction = new QAction("English", this);
	connect(englishAction, &QAction::triggered, this, [this]() { changeLanguage("en"); });
	languageMenu->addAction(englishAction);

	QAction *japanAction = new QAction("言語", this);
	connect(japanAction, &QAction::triggered, this, [this]() { changeLanguage("ja"); });
	languageMenu->addAction(japanAction);

	QAction *russianAction = new QAction("Русский", this);
	connect(russianAction, &QAction::triggered, this, [this]() { changeLanguage("ru"); });
	languageMenu->addAction(russianAction);
}

void MainWindow::click_mine(NumberButton *button)
{
	button->setIconSize(button->size());
	QIcon buttonIcon(":/images/mine.png");
	button->setIcon(buttonIcon);
}

void MainWindow::click_num(NumberButton *button)
{
	button->setIconSize(button->size());
	if (button->number() != 0)
	{
		button->open_tile();
		QIcon numberIcon(arr[button->number() - 1]);
		button->setIcon(numberIcon);
	}
	else
		button->open_tile();
}

void MainWindow::onPeekModeToggled(bool checked)
{
	peekMode = checked;
	dbgModeEnabled = true;
	for (NumberButton *button : *buttons)
	{
		if (peekMode)
			button->isMine() ? click_mine(button) : click_num(button);
		else if (!button->isRevealed() && !button->isFlagged())
			button->close_tile();
		else if (button->isFlagged())
		{
			QIcon buttonIcon(":/images/flag.png");
			button->setIcon(buttonIcon);
		}
	}
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);

	int w = width(), h = height();

	int ht = toolBar->height();
	int menuHeight = menuBar()->height();

	int availableWidth = w - 40;
	int availableHeight = h - ht - menuHeight - 40;

	int maxButtonSize = qMax(qMin((availableWidth - (spacing * (m - 1))) / m, (availableHeight - (spacing * (n - 1))) / n), 10);
	for (NumberButton *button : *buttons)
	{
		button->setFixedSize(maxButtonSize, maxButtonSize);
		button->setIconSize(QSize(maxButtonSize, maxButtonSize));
	}

	centralWidget->resize(qMax(maxButtonSize * m + spacing * (m - 1), 40), qMax(maxButtonSize * n + spacing * (n - 1), 40));
	gridlayout->setAlignment(Qt::AlignCenter);
}

void MainWindow::placeMines(int clickedIndex)
{
	srand(time(nullptr));
	int minesPlacedCount = 0;

	while (minesPlacedCount < mineCount)
	{
		int index = rand() % (n * m);
		if (index != clickedIndex && !buttons->at(index)->isMine())
		{
			buttons->at(index)->setMine(true);
			minesPlacedCount++;
		}
	}
}

void MainWindow::calculateNumbers()
{
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < m; col++)
		{
			int index = row * m + col;
			if (!buttons->at(index)->isMine())
				buttons->at(index)->setNumber(countAdjacentMines(row, col));
		}
	}
}

int MainWindow::countAdjacentMines(int row, int col)
{
	int count = 0;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			int r = row + i, c = col + j;
			if (r >= 0 && r < n && c >= 0 && c < m && !(i == 0 && j == 0) && buttons->at(r * m + c)->isMine())
				count++;
		}
	}
	return count;
}

void MainWindow::revealEmptyCells(int index)
{
	if (index < 0 || index >= n * m)
		return;

	NumberButton *button = buttons->at(index);
	if (button->isRevealed() || button->isMine() || button->isFlagged())
		return;

	button->setRevealed(true);
	click_num(button);
	if (button->number() != 0)
		return;

	int row = index / m, col = index % m;
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			if (i == 0 && j == 0)
				continue;
			int newRow = row + i, newCol = col + j;
			if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < m)
				revealEmptyCells(newRow * m + newCol);
		}
	}
}

void MainWindow::showAllMines(int clickedIndex)
{
	for (int i = 0; i < buttons->size(); i++)
	{
		buttons->at(i)->isMine() && i != clickedIndex ? click_mine(buttons->at(i)) : click_num(buttons->at(i));
		buttons->at(clickedIndex)->open_tile();
	}
}

void MainWindow::checkVictory(int index)
{
	bool victory = true;
	int revealedCount = 0;

	for (NumberButton *button : *buttons)
	{
		if (button->isRevealed())
			revealedCount++;
	}
	for (NumberButton *button : *buttons)
	{
		if ((button->isMine() && !button->isFlagged()) || (button->isFlagged() && !button->isMine()))
		{
			victory = false;
			break;
		}
	}

	if (!victory)
		victory = (revealedCount == (n * m - mineCount));

	if (victory && minesPlaced)
	{
		QMessageBox msgBox;
		showAllMines(index);
		if (buttons->at(index)->isFlagged())
			click_mine(buttons->at(index));
		msgBox.setStyleSheet("background-color: #44C19A; border: none;");
		msgBox.setWindowTitle(tr("Game Victory"));
		msgBox.setText(tr("Congratulations! You've won!"));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		game_end();
	}
	win = victory;
}

void MainWindow::handleButtonClick(int index)
{
	if (!minesPlaced)
	{
		placeMines(index);
		calculateNumbers();
		minesPlaced = true;
	}

	NumberButton *button = buttons->at(index);
	if (button->isRevealed())
		return;

	if (button->isMine())
		game_end();
	else if (button->number() == 0)
		revealEmptyCells(index);
	else
		click_num(button);

	button->setRevealed(true);
	checkVictory(index);
}

void MainWindow::handleMiddleClick(int index)
{
	if (index < 0 || index >= n * m)
		return;

	NumberButton *button = buttons->at(index);
	if (!button->isRevealed() || button->number() == 0)
		return;

	int row = index / m, col = index % m, flaggedCount = 0, mineCount = button->number();
	QVector< int > neighbors;

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			int r = row + i, c = col + j;
			if (r >= 0 && r < n && c >= 0 && c < m)
			{
				int neighborIndex = r * m + c;
				neighbors.append(neighborIndex);
				if (buttons->at(neighborIndex)->isFlagged())
					flaggedCount++;
			}
		}
	}

	if (flaggedCount == mineCount)
	{
		for (int neighborIndex : neighbors)
		{
			if (buttons->at(neighborIndex)->isMine() && !buttons->at(neighborIndex)->isFlagged())
			{
				showAllMines(neighborIndex);
				gameOver(neighborIndex);
				return;
			}
			else if (!buttons->at(neighborIndex)->isFlagged() && !buttons->at(neighborIndex)->isRevealed())
				handleButtonClick(neighborIndex);
		}
	}
	else
		highlightCells(neighbors);
}

void MainWindow::gameOver(int index)
{
	setStyleSheet("background-color: #800000; border: none;");
	QIcon buttonIcon(":/images/Boom.png");
	buttons->at(index)->setIcon(buttonIcon);
	buttons->at(index)->setRevealed(true);
	QMessageBox msgBox;
	msgBox.setStyleSheet("background-color: #800000; border: none;");
	msgBox.setWindowTitle(tr("Game Over"));
	msgBox.setText(tr("Game over. You've lost"));
	msgBox.setIcon(QMessageBox::Information);
	msgBox.exec();
	for (NumberButton *btn : *buttons)
		btn->setEnabled(false);
}

void MainWindow::newGame()
{
	flagCount = 0;
	win = false;
	flag = true;
	minesPlaced = false;
	updateMinesLeftLabel();
	for (int i = 0; i < n * m; i++)
		buttons->at(i)->reset();
	this->setStyleSheet("background-color: #44C19A;");
}

void MainWindow::newGameWithSettings()
{
	win = false;
	flag = true;
	minesPlaced = false;

	Start startDialog(this, false);
	this->setStyleSheet("background-color: #44C19A;");

	if (startDialog.exec() == QDialog::Accepted)
	{
		int newRows = startDialog.rows();
		int newCols = startDialog.cols();
		int newMineCount = startDialog.mineCount();

		if (newRows != n || newCols != m)
		{
			n = newRows;
			m = newCols;
			mineCount = newMineCount;
			setupCentralWidget();
			setupGrid();
		}
		else
		{
			mineCount = newMineCount;
			newGame();
		}
	}
}

void MainWindow::changeLanguage(const QString &language)
{
	qApp->removeTranslator(&translator);
	QString translationFile = ":/translations/qt_" + language + ".qm";
	if (translator.load(translationFile))
	{
		qApp->installTranslator(&translator);
		retranslateUi();
	}
	QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
	settings.beginGroup("Game");
	settings.setValue("language", language);
	settings.endGroup();
}

void MainWindow::retranslateUi()
{
	this->setWindowTitle(tr("MineSweeper2024"));
	menuBar()->clear();
	setupMenu();
	removeToolBar(toolBar);
	setupToolBar(globalDebugMode);
	QCheckBox *leftHandedCheckBox = toolBar->findChild< QCheckBox * >();
	if (leftHandedCheckBox)
		leftHandedCheckBox->setChecked(leftHandedMode);

	QCheckBox *dbgModeCheckBox = toolBar->findChild< QCheckBox * >("dbgModeCheckBox");
	if (dbgModeCheckBox && peekMode && globalDebugMode)
		dbgModeCheckBox->setChecked(globalDebugMode);
	updateMinesLeftLabel();
}

void MainWindow::highlightCells(const QVector< int > &neighbors)
{
	for (int neighborIndex : neighbors)
	{
		if (!buttons->at(neighborIndex)->isRevealed() && !buttons->at(neighborIndex)->isFlagged() &&
			!buttons->at(neighborIndex)->isQuestionMarked())
		{
			buttons->at(neighborIndex)
				->setStyleSheet(
					"background-color: #66E1D2;"
					"border: 2px solid #2BCABA;"
					"border-radius: 3px;");
			buttons->at(neighborIndex)->setIcon(QIcon());
		}
	}
	QTimer::singleShot(400, this, &MainWindow::clearHighlights);
}

void MainWindow::clearHighlights()
{
	for (NumberButton *button : *buttons)
		if (!button->isRevealed() && !button->isFlagged() && !button->isQuestionMarked())
			button->close_tile();
}

void MainWindow::updateMinesLeftLabel()
{
	int minesLeft = mineCount - flagCount;
	minesLeftLabel->setText(tr("Mines left: %1").arg(minesLeft));
}

bool MainWindow::isDebugMode()
{
	return dbgModeEnabled;
}

void MainWindow::game_end()
{
	for (NumberButton *btn : *buttons)
		btn->setEnabled(false);
}

void MainWindow::onModeLeftHand(bool checked)
{
	leftHandedMode = checked;
}

void MainWindow::saveGameState()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
	settings.beginGroup("Game");

	bool isGameOver = false;
	for (NumberButton *button : *buttons)
		if ((button->isMine() && button->isRevealed()) || win)
			isGameOver = true;

	if (isGameOver)
		settings.remove("");
	else
	{
		settings.setValue("rows", n);
		settings.setValue("cols", m);
		settings.setValue("mineCount", mineCount);
		settings.setValue("minesPlaced", minesPlaced);
		settings.setValue("peekMode", peekMode);
		settings.setValue("leftHandedMode", leftHandedMode);
		settings.setValue("flagCount", flagCount);

		settings.beginWriteArray("buttons");
		for (int i = 0; i < buttons->size(); ++i)
		{
			settings.setArrayIndex(i);
			settings.setValue("isMine", buttons->at(i)->isMine());
			settings.setValue("isFlagged", buttons->at(i)->isFlagged());
			settings.setValue("isRevealed", buttons->at(i)->isRevealed());
			settings.setValue("number", buttons->at(i)->number());
			settings.setValue("isQuestion", buttons->at(i)->isQuestionMarked());
		}
		settings.endArray();
	}

	settings.endGroup();
}

void MainWindow::loadGameState()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
	settings.beginGroup("Game");

	QString language = settings.value("language", "en").toString();
	changeLanguage(language);

	int rows = settings.value("rows", 10).toInt();
	int cols = settings.value("cols", 10).toInt();
	int mines = settings.value("mineCount", 10).toInt();
	peekMode = settings.value("peekMode", false).toBool();

	if (rows <= 0 || cols <= 0 || mines < 0 || mines > rows * cols)
	{
		rows = 10;
		cols = 10;
		mines = 10;
	}

	n = rows;
	m = cols;
	mineCount = mines;
	minesPlaced = settings.value("minesPlaced", false).toBool();
	leftHandedMode = settings.value("leftHandedMode", false).toBool();
	flagCount = settings.value("flagCount", 0).toInt();

	setupCentralWidget();
	setupGrid();

	int size = settings.beginReadArray("buttons");
	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);

		bool isMine = settings.value("isMine", false).toBool();
		bool isFlagged = settings.value("isFlagged", false).toBool();
		bool isRevealed = settings.value("isRevealed", false).toBool();
		int number = settings.value("number", 0).toInt();
		bool isQuestion = settings.value("isQuestion", false).toBool();
		if (number < 0 || number > 8)
			number = 0;

		buttons->at(i)->setMine(isMine);
		buttons->at(i)->setFlagged(isFlagged);
		buttons->at(i)->setRevealed(isRevealed);
		buttons->at(i)->setNumber(number);
		buttons->at(i)->setQuestionMarked(isQuestion);

		if (buttons->at(i)->isRevealed())
			click_num(buttons->at(i));
		else if (buttons->at(i)->isFlagged())
		{
			QIcon buttonIcon(":/images/flag.png");
			buttons->at(i)->setIcon(buttonIcon);
		}
		else if (buttons->at(i)->isQuestionMarked())
		{
			QIcon buttonIcon(":/images/question.png");
			buttons->at(i)->setIcon(buttonIcon);
		}
	}
	settings.endArray();
	if (toolBar)
	{
		removeToolBar(toolBar);
		delete toolBar;
	}

	setupToolBar(globalDebugMode);

	QCheckBox *leftHandedCheckBox = toolBar->findChild< QCheckBox * >();
	if (leftHandedCheckBox)
		leftHandedCheckBox->setChecked(leftHandedMode);

	QCheckBox *dbgModeCheckBox = toolBar->findChild< QCheckBox * >("dbgModeCheckBox");
	if (dbgModeCheckBox && peekMode && globalDebugMode)
		dbgModeCheckBox->setChecked(globalDebugMode);

	settings.endGroup();
}

MainWindow::~MainWindow()
{
	for (NumberButton *button : *buttons)
		delete button;
	delete buttons;
}
//
