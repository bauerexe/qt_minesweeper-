#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTranslator>

class Start : public QDialog
{
	Q_OBJECT

  public:
	Start(QWidget *parent = nullptr, bool showLanguageSelection = true);

	int rows() const;
	int cols() const;
	int mineCount() const;
	void setLanguageSelectionEnabled(bool enabled);
	QComboBox *languageComboBox;
  signals:
	void languageChanged(const QString &languageCode);

  private slots:
	void changeLanguage(int index);

  private:
	void validateInputs();
	void retranslateUi();

	QSpinBox *rowsSpinBox;
	QSpinBox *colsSpinBox;
	QSpinBox *mineCountSpinBox;
	QPushButton *startButton;

	QTranslator translator;

	QLabel *rowsLabel;
	QLabel *colsLabel;
	QLabel *minesLabel;
	QLabel *languageLabel;
};

#endif	  // STARTDIALOG_H
