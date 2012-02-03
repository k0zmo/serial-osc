#pragma once

#include <QGLWidget>

class MainWindow;

class GLWidget : public QGLWidget
{
public:
	GLWidget(MainWindow* parent);
	virtual ~GLWidget();

	void resizeGL(int width, int height);
	void calcDataConstraints(float ymin, float ymax);
	void clearGraphData();

	float graphWidth() const { return mGraphWidth; }
	float graphHeight() const { return mGraphHeight; }
	float* graphData() const { return mGraphData; }

protected:
	void initializeGL();
	void paintGL();
	void drawGraph();

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void timerEvent(QTimerEvent* event);

private:
	const float mGraphWidth;
	const float mGraphHeight;
	float mCam_xpos;
	float mCam_ypos;
	float mCam_zpos;

	MainWindow* mParent;
	float* mGraphData;
	float a_coeff, b_coeff;

	QPoint mAnchor;
};

