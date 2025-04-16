#include "start.h"

#include <QApplication>
#include <QFormLayout>
#include <QMessageBox>
#include <QVBoxLayout>

Start::Start(QWidget *parent, bool showLanguageSelection) :
	QDialog(parent), languageComboBox(showLanguageSelection ? new QComboBox(this) : nullptr), rowsSpinBox(new QSpinBox),
	colsSpinBox(new QSpinBox), mineCountSpinBox(new QSpinBox), startButton(new QPushButton(tr("Start Game"))),
	rowsLabel(new QLabel(tr("Rows:"))), colsLabel(new QLabel(tr("Columns:"))), minesLabel(new QLabel(tr("Mines:"))),
	languageLabel(showLanguageSelection ? new QLabel(tr("Language:")) : nullptr)
{
	setWindowTitle(tr("Start"));
	rowsSpinBox->setRange(4, 20);
	rowsSpinBox->setValue(10);
	rowsSpinBox->setMinimumSize(100, 30);

	colsSpinBox->setRange(4, 20);
	colsSpinBox->setValue(10);
	colsSpinBox->setMinimumSize(100, 30);

	mineCountSpinBox->setRange(1, 100);
	mineCountSpinBox->setValue(10);
	mineCountSpinBox->setMinimumSize(100, 30);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(rowsLabel, rowsSpinBox);
	formLayout->addRow(colsLabel, colsSpinBox);
	formLayout->addRow(minesLabel, mineCountSpinBox);

	if (showLanguageSelection)
	{
		languageComboBox->addItem("English", "en");
		languageComboBox->addItem("Русский", "ru");
		languageComboBox->addItem("日本語", "ja");
		formLayout->addRow(languageLabel, languageComboBox);

		connect(languageComboBox, QOverload< int >::of(&QComboBox::currentIndexChanged), this, &Start::changeLanguage);
	}

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(formLayout);
	layout->addWidget(startButton);
	setLayout(layout);

	setFixedSize(200, 220);

	setStyleSheet("background-color: #B5B8B1;");

	connect(startButton, &QPushButton::clicked, this, &Start::validateInputs);
}

void Start::retranslateUi()
{
	setWindowTitle(tr("Start"));
	rowsLabel->setText(tr("Rows:"));
	colsLabel->setText(tr("Columns:"));
	minesLabel->setText(tr("Mines:"));
	if (languageLabel)
	{
		languageLabel->setText(tr("Language:"));
	}
	startButton->setText(tr("Start Game"));

	if (languageComboBox)
	{
		languageComboBox->setItemText(0, tr("English"));
		languageComboBox->setItemText(1, tr("Русский"));
		languageComboBox->setItemText(2, tr("日本語"));
	}
}

int Start::rows() const
{
	return rowsSpinBox->value();
}

int Start::cols() const
{
	return colsSpinBox->value();
}

int Start::mineCount() const
{
	return mineCountSpinBox->value();
}

void Start::validateInputs()
{
	int totalCells = rows() * cols();
	int mines = mineCount();
	if (mines >= totalCells)
		QMessageBox::warning(this, tr("Invalid Input"), tr("The number of mines must be less than the total number of cells."));
	else
		accept();
}

void Start::changeLanguage(int index)
{
	QString languageCode = languageComboBox->itemData(index).toString();
	qApp->removeTranslator(&translator);
	if (translator.load(":/translations/qt_" + languageCode + ".qm"))
	{
		qApp->installTranslator(&translator);
	}
	retranslateUi();

	emit languageChanged(languageCode);
}
