#pragma once

#include "ImageLabel.h"

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QImage>
#ifndef QT_NO_PRINTER
#include <QPrinter> //Qt 5中将所有打印的相关类都放到了Qt Print Support模块造成的。
#endif

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer();
	bool loadFile(const QString &);

private slots:
	void open();
	void saveAs();
	void print();
	void copy();
	void paste();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow();
	void about();

private:
	void createActions();
	void createMenus();
	void updateActions();
	bool saveFile(const QString &fileName);
	void setImage(const QImage &newImage);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);

	QImage image;
	ImageLabel *imageLabel;
	QScrollArea *scrollArea;
	double scaleFactor;

#ifndef QT_NO_PRINTER
	QPrinter printer;
#endif

	QAction *saveAsAct;
	QAction *printAct;
	QAction *copyAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;
};

#endif
