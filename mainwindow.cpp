#include "pch.h"
#include <QMessageBox>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dataprocessor.h"
#include "realtimereader.h"
#include "glwidget.h"
#include "exception.h"

MainWindow::MainWindow(QWidget *mParent)
	: QMainWindow(mParent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	// wycentruj
	QSize size = this->geometry().size();
	QRect screen = QDesktopWidget().screenGeometry();
	this->move((screen.width()-size.width())/2,
				(screen.height()-size.height())/4);

	// kontrolka gl - qtdesigner jej nie obsluguje
	mGLWidget = new GLWidget(this);
	ui->glLayout->addWidget(mGLWidget);

	ui->dataTypeCombo->addItem("unsigned 8bits");
	ui->dataTypeCombo->addItem("signed 8bits");
	ui->dataTypeCombo->addItem("unsigned 10bits");
	ui->dataTypeCombo->addItem("signed 10bits");
	ui->dataTypeCombo->addItem("unsigned 12bits");
	ui->dataTypeCombo->addItem("signed 12bits");
	ui->dataTypeCombo->addItem("unsigned 16bits");
	ui->dataTypeCombo->addItem("signed 16bits");

	ui->dataTypeCombo->setCurrentIndex(2);

	// zdarzenia
	connect(ui->openBtn, SIGNAL(clicked()), this, SLOT(openCloseEvent()));
	connect(ui->loseOneByteBtn, SIGNAL(clicked()), this, SLOT(loseOneByteEvent()));

	// timer
	mTimerId = startTimer(20);

	// czytanie z portu COM
	mDataProc = new DataProcessor();
	mDataProc->setGraphBuffer(mGLWidget->graphData(), mGLWidget->graphWidth());
	mReader = new RealTimeReader();
}

MainWindow::~MainWindow()
{
	delete mReader;
	delete mDataProc;
    delete ui;
}

bool MainWindow::aaChecked()
{
	return ui->aaCheck->isChecked();
}

void MainWindow::openCloseEvent()
{
	if(!mReader->opened())
	{
		try
		{
			mReader->setReadingParams(ui->sampleRateEdit->text().toInt(), // ile bajtow na sekunde (dla typu short 2x)
								  ui->timeWindowEdit->text().toInt()); // 'widzialny' przedzial czasowy

			mReader->open(ui->portNameEdit->text(), // nazwa portu ("COM3")
					  ui->baudRateEdit->text().toInt()); // baudrate (250000)

			// dla odpowiedniego skalowania danych
			mDataProc->setDataType((DataProcessor::DataType)ui->dataTypeCombo->currentIndex());
			mGLWidget->calcDataConstraints(static_cast<float>(mDataProc->minValue()),
					static_cast<float>(mDataProc->maxValue()));
			mGLWidget->clearGraphData();

			ui->openBtn->setText("Close");
			ui->loseOneByteBtn->setEnabled(true);
		}
		catch(Exception& e)
		{
			QMessageBox::critical(this, "Exception occured!", e.message());
		}
	} 
	else
	{
		mReader->close();
		ui->openBtn->setText("Open");
		ui->loseOneByteBtn->setEnabled(false);
	}
}

void MainWindow::loseOneByteEvent()
{
	mReader->loseOneByte();
}

void MainWindow::timerEvent(QTimerEvent* event)
{
	if(event->timerId() == mTimerId)
	{
		if(mReader->opened())
		{
			mReader->read();
			mDataProc->calc(mReader->buffer(), mReader->bufferCursor(), ui->fltCheck->isChecked());
		}
	}
	QMainWindow::timerEvent(event);
}
