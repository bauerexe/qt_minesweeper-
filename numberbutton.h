#ifndef NUMBERBUTTON_H
#define NUMBERBUTTON_H

#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QTranslator>

class NumberButton : public QPushButton
{
	Q_OBJECT

  public:
	NumberButton(int number, int index, QWidget *parent = nullptr);
	int number() const;
	void setNumber(int number);
	bool isMine() const;
	void setMine(bool mine);
	bool isRevealed() const;
	void setRevealed(bool revealed);
	bool isFlagged() const;
	void setFlagged(bool flagged);
	void setQuestionMarked(bool questionMarked);

	void reset();
	void close_tile();
	void open_tile();
	bool isQuestionMarked() const;

  private slots:
	void mousePressEvent(QMouseEvent *event) override;

  private:
	int m_number;
	int m_index;
	bool m_isMine;
	bool m_isFlagged;
	bool m_isRevealed;
	bool m_isQuestionMarked;

	void handleMiddleClick(int index);
	void onButtonClicked();
};

#endif	  // NUMBERBUTTON_H
//
