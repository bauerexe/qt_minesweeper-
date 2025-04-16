#include "numberbutton.h"

#include "mainwindow.h"

#include <QTranslator>
#include <QWidgetAction>

NumberButton::NumberButton(int number, int index, QWidget *parent) :
	QPushButton(parent), m_number(number), m_index(index), m_isMine(false), m_isFlagged(false), m_isRevealed(false),
	m_isQuestionMarked(false)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	close_tile();

	connect(this, &QPushButton::clicked, this, &NumberButton::onButtonClicked);
}

void NumberButton::mousePressEvent(QMouseEvent *event)
{
	MainWindow *mainWindow = qobject_cast< MainWindow * >(window());
	if (!mainWindow)
		return;
	bool isLeftHandedMode = mainWindow->leftHandedMode;
	if (!mainWindow->peekMode)
	{
		if ((event->button() == Qt::RightButton && !isLeftHandedMode) || (event->button() == Qt::LeftButton && isLeftHandedMode))
		{
			if (!m_isRevealed)
			{
				if (!m_isFlagged && !m_isQuestionMarked)
				{
					m_isFlagged = true;
					mainWindow->flagCount++;
					close_tile();
					QIcon buttonIcon(":/images/flag.png");
					this->setIcon(buttonIcon);
					this->setIconSize(this->size());
				}
				else if (m_isFlagged)
				{
					m_isFlagged = false;
					m_isQuestionMarked = true;
					mainWindow->flagCount--;
					close_tile();
					QIcon buttonIcon(":/images/question.png");
					this->setIcon(buttonIcon);
					this->setIconSize(this->size());
				}
				else if (m_isQuestionMarked)
				{
					m_isQuestionMarked = false;
					this->setIcon(QIcon());
					close_tile();
				}

				mainWindow->checkVictory(m_index);
			}
			mainWindow->updateMinesLeftLabel();
		}
		else if ((event->button() == Qt::LeftButton && !isLeftHandedMode) || (event->button() == Qt::RightButton && isLeftHandedMode))
		{
			if (!m_isFlagged && !m_isQuestionMarked)
			{
				emit clicked();
			}
		}
		else if (event->button() == Qt::MiddleButton && m_isRevealed && m_number > 0)
		{
			mainWindow->handleMiddleClick(m_index);
		}
	}
}

void NumberButton::onButtonClicked()
{
	if (!m_isFlagged && !m_isRevealed)
	{
		if (m_isMine)
		{
			MainWindow *mainWindow = qobject_cast< MainWindow * >(window());
			mainWindow->showAllMines(m_index);
			mainWindow->setStyleSheet("background-color: #800000; border: none;");
			setStyleSheet("background-color: #800000; border: none;");
			QIcon buttonIcon(":/images/Boom.png");
			this->setRevealed(true);
			this->setIcon(buttonIcon);
			this->setIconSize(this->size());
			QMessageBox msgBox;
			msgBox.setStyleSheet("background-color: #800000; border: none;");
			msgBox.setWindowTitle(tr("Game Lose"));
			msgBox.setText(tr("Game over. You've lost"));
			msgBox.setIcon(QMessageBox::Information);
			msgBox.exec();
			mainWindow->game_end();
		}
	}
}

void NumberButton::close_tile()
{
	this->setStyleSheet(
		"background-color: #A9A9A9;"
		"border: 3px solid #696969;"
		"border-radius: 4px;");
	this->setIcon(QIcon());
}

void NumberButton::open_tile()
{
	this->setStyleSheet(
		"background-color: #B5B8B1;"
		"border: 3px solid #A0A59C;"
		"border-radius: 4px;");
	this->setIcon(QIcon());
}
void NumberButton::reset()
{
	this->close_tile();
	this->setMine(false);
	this->setFlagged(false);
	this->setRevealed(false);
	this->setQuestionMarked(false);
	this->setIcon(QIcon());
	this->setNumber(0);
	this->setEnabled(true);
}

int NumberButton::number() const
{
	return m_number;
}

void NumberButton::setNumber(int number)
{
	m_number = number;
}

bool NumberButton::isMine() const
{
	return m_isMine;
}

void NumberButton::setMine(bool mine)
{
	m_isMine = mine;
}

bool NumberButton::isRevealed() const
{
	return m_isRevealed;
}

void NumberButton::setRevealed(bool revealed)
{
	m_isRevealed = revealed;
}

bool NumberButton::isFlagged() const
{
	return m_isFlagged;
}

void NumberButton::setFlagged(bool flagged)
{
	m_isFlagged = flagged;
}

bool NumberButton::isQuestionMarked() const
{
	return m_isQuestionMarked;
}

void NumberButton::setQuestionMarked(bool questionMarked)
{
	m_isQuestionMarked = questionMarked;
}
//
