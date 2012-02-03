#include "pch.h"
#include "glwidget.h"
#include "mainwindow.h"
#include "dataprocessor.h"

#include <QMouseEvent>

const int timerInterval = 20;

//
// Jak dotad uzywany jest OpenGL w wersji 3.0/2.1 z deprecation model
//

GLWidget::GLWidget(MainWindow* parent)
	: mGraphWidth(600.0f), mGraphHeight(300.0f),
	mCam_xpos(-301.0f), mCam_ypos(0.0f), mCam_zpos(-575.0f)
{
	startTimer(timerInterval); //64-65fps
	mParent = parent;

	mGraphData = new float[static_cast<int>(mGraphWidth)];
	clearGraphData();

	a_coeff = 1.0f;
	b_coeff = 0.0;
}

GLWidget::~GLWidget()
{
	delete mGraphData;
}

void GLWidget::clearGraphData()
{
	for(int i = 0; i < static_cast<int>(mGraphWidth); ++i)
		mGraphData[i] = 0.0f;
}

void GLWidget::initializeGL()
{	
	// Taka wartoscia bedzie czyszczony bufor koloru
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Taka wartoscia bedzie czyszczony bufor glebi
	glClearDepth(1.0f);
	 
	//glDepthFunc(GL_LESS);
	// Test glebi wylaczony - mieszanie kolorow
	glDisable(GL_DEPTH_TEST);

	// Wlacz mieszanie kolorow
	glEnable(GL_BLEND);
	// Ustaw funkcje mieszajaca kolory
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glShadeModel(GL_SMOOTH); // DEPRECATED

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Wyczysc bufory glebi i koloru
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); // DEPRECATED
	glLoadIdentity(); // DEPRECATED
}

void GLWidget::resizeGL(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION); // DEPRECATED
	glLoadIdentity(); // DEPRECATED
	gluPerspective(30.0, (GLfloat)width/GLfloat(height), 0.1f, 1800.0f); // DEPRECATED
	glMatrixMode(GL_MODELVIEW); // DEPRECATED
}

void GLWidget::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); // DEPRECATED
	glLoadIdentity(); // DEPRECATED

	glTranslatef(mCam_xpos, mCam_ypos, mCam_zpos); // DEPRECATED
	glColor3f(0.0f, 1.0f, 0.0f); // DEPRECATED

	// rysuj obramowanie (gorna i dolna granice
	glBegin(GL_LINES); // DEPRECATED
		glColor3f(1.0f, 0.0f, 0.0f); // DEPRECATED
		glVertex3f(0.0f, -mGraphHeight/2.0f, 2.0f); // DEPRECATED
		glVertex3f(mGraphWidth, -mGraphHeight/2.0f, 2.0f); // DEPRECATED
		glVertex3f(0.0f, mGraphHeight/2.0f, 2.0f); // DEPRECATED
		glVertex3f(mGraphWidth, mGraphHeight/2.0f, 2.0f); // DEPRECATED
	glEnd(); // DEPRECATED

	// rysuj przebieg
	drawGraph();
}

void GLWidget::calcDataConstraints(float ymin, float ymax)
{
	float x1 = ymin;
	float x2 = ymax;
	float y1 = -mGraphHeight/2.0f;
	float y2 = mGraphHeight/2.0f;

	// skalowanie danych z [nMin,nMax] do [-150, 150]: y = a*x+b
	// a = (y1-y2)/(x1-x2)
	// b = (x1*y2 - x2*y1)/(x1-x2)

	a_coeff = (y1 - y2) / (x1 - x2);
	b_coeff = (x1*y2 - x2*y1) / (x1 - x2);
}

void GLWidget::drawGraph()
{
	bool antiAliasing = mParent->aaChecked();
	float cursor = static_cast<float>(mParent->processor()->graphCursor());

	if(antiAliasing)
	{
		glEnable(GL_BLEND);
		glColor4f(0.0, 1.0f, 0.0f, 0.5f); // DEPRECATED
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.0f);

		glBegin(GL_LINES); // DEPRECATED
		for(int i = 1; i < static_cast<int>(mGraphWidth); ++i)
		{
			glVertex3f(static_cast<float>(i-1), mGraphData[i-1] * a_coeff + b_coeff, 0.0f); // DEPRECATED
			glVertex3f(static_cast<float>(i), mGraphData[i] * a_coeff + b_coeff, 0.0f); // DEPRECATED
		}
		glEnd(); // DEPRECATED
		glLineWidth(1.0);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
		glColor3f(0.0f, 1.0f, 0.0f); // DEPRECATED
		glDisable(GL_LINE_SMOOTH);
		glLineWidth(1.0f);

		glBegin(GL_LINES); // DEPRECATED
		for(int i = 1; i < static_cast<int>(mGraphWidth); ++i)
		{
			glVertex3f(static_cast<float>(i-1), mGraphData[i-1] * a_coeff + b_coeff, 0.0f); // DEPRECATED
			glVertex3f(static_cast<float>(i), mGraphData[i] * a_coeff + b_coeff, 0.0f); // DEPRECATED
		}
		glEnd(); // DEPRECATED
		glLineWidth(1.0);
	}
	// Rysuj pasek "kasujacy" pare pikseli przed wykresem
	// by bylo widac gdzie dokladnie jest wykres rysowany
	if(cursor + 5.0f < mGraphWidth)
	{
		glColor4f(0.0f, 0.0f, 0.0f, 0.0f); // DEPRECATED
		glBegin(GL_TRIANGLE_STRIP); // DEPRECATED
			glVertex3f(cursor - 5.0f,  mGraphHeight * 0.5f - 5.0f, 1.0f); // DEPRECATED
			glVertex3f(cursor - 5.0f, -mGraphHeight * 0.5f + 5.0f, 1.0f); // DEPRECATED
			glVertex3f(cursor + 5.0f,  mGraphHeight * 0.5f - 5.0f, 1.0f); // DEPRECATED
			glVertex3f(cursor + 5.0f, -mGraphHeight * 0.5f + 5.0f, 1.0f); // DEPRECATED
		glEnd(); // DEPRECATED
	}
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	mAnchor = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPoint diff = event->pos() - mAnchor;

	mCam_xpos += diff.x();
	mCam_ypos -= diff.y();

	mAnchor = event->pos();
	updateGL();
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	if(event->delta() > 0)
		mCam_zpos += 10.0f;
	else
		mCam_zpos -= 10.0f;
	updateGL();
}

void GLWidget::timerEvent(QTimerEvent* event)
{
	updateGL();
}
