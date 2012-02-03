#pragma once

#include <QMainWindow>

class RealTimeReader;
class DataProcessor;
class GLWidget;
namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *mParent = 0);
    ~MainWindow();

	bool aaChecked();

	RealTimeReader* reader() const { return mReader; }
	DataProcessor* processor() const { return mDataProc; }

protected:
	virtual void timerEvent(QTimerEvent* event);

private:
    Ui::MainWindow* ui;
	RealTimeReader* mReader;
	DataProcessor* mDataProc;
	GLWidget* mGLWidget;
	int mTimerId;

private slots:
	void openCloseEvent();
	void loseOneByteEvent();
};
