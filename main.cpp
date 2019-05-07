#include "ImageViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

	ImageViewer imageViewer;

	imageViewer.show();
	return app.exec();
}