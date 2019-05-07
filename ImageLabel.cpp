#include "ImageLabel.h"

#include <QPainter>
#include <QDebug>

ImageLabel::ImageLabel(QWidget *parent)
	: QLabel(parent),
	isSelectIn(false),
	isSelectCorner(-1),
	isSelectSide(-1)
{
	setMouseTracking(true);
	myRect.setRect(50, 10, 200, 200);
}

ImageLabel::~ImageLabel()
{
}


void ImageLabel::paintEvent(QPaintEvent * event)
{
	QLabel::paintEvent(event);

	QPainter painter(this);

	painter.setPen(QPen(Qt::red));
	painter.drawRect(myRect);
}

void ImageLabel::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		isLBtnDown = true;
		pressPoint = event->pos();

		recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());

		if (isInMyRect(pressPoint))
		{
			//setCursor(Qt::ClosedHandCursor);
			isSelectIn = true;
			qDebug() << "in the rect";
		}
		else if (atCorner(pressPoint) > 0)
		{
			isSelectCorner = atCorner(pressPoint);
			qDebug() << "select the corner:" << isSelectCorner;

		}
		else if (atSide(pressPoint) > 0)
		{
			isSelectSide = atSide(pressPoint);
			qDebug() << "select the side:" << isSelectSide;
		}
		else
		{
			//空白区域
			//recordRect.setX(event->pos().x());
			//recordRect.setY(event->pos().y());

			//recordRect.setRect(event->pos().x(), event->pos().y(), 0, 0);
			//qDebug() << recordRect.height();
		}
	}

	QWidget::mousePressEvent(event);
}

void ImageLabel::mouseMoveEvent(QMouseEvent * event)
{

	if (isLBtnDown)
	{
		//Boundary detection
		//cursor cannot leave the label when isLBtnDown
		QSize lableSize = size();
		//QPoint position = mapFromGlobal(QCursor::pos()); //bug：QPoint movePoint = event->pos()可能为负
		QPoint position = event->pos();

		qint32 x = qBound(0, position.x(), lableSize.width());
		qint32 y = qBound(0, position.y(), lableSize.height());

		// Adjust the cursor
		if (x != position.x() || y != position.y())
		{
			qDebug() << "the cursor leave the lable:";
			qDebug() << position.x()<< position.y();


			QCursor::setPos(mapToGlobal(QPoint(x, y)));
			qDebug() << "adjust the cursor:";

			QPoint position2 = mapFromGlobal(QCursor::pos());
			qDebug() << position2.x() << position2.y();
			
			event->accept();
			QLabel::mouseMoveEvent(event);
			return;
		}

		//按道理movePoint不应该为负了啊，上面已经处理
		QPoint movePoint = event->pos();
		qDebug() << "movePoint:";
		qDebug() << movePoint.x() << movePoint.y();

		int moveW = movePoint.x() - pressPoint.x();
		int moveH = movePoint.y() - pressPoint.y();


		if (isSelectIn)
		{
			setCursor(Qt::SizeAllCursor);
			int moveX = recordRect.x() + moveW;
			int moveY = recordRect.y() + moveH;

			//qDebug() << "press and move";
			//myRect.setRect(moveX, moveY, myRect.width(), myRect.height());
			myRect.moveTopLeft(QPoint(moveX, moveY));
			qDebug() << "moveTopLeft:";
			qDebug() << myRect.x() << myRect.y();


			if (myRect.width() < 0 || myRect.height()<0 || myRect.x()<0 ||myRect.y()<0)
			{
				correctRect(myRect);
				recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());
				pressPoint = event->pos();
				qDebug() << "change perssPoint:";
				qDebug() << pressPoint.x() << pressPoint.y();
			}
		}
		else if (isSelectCorner > 0)
		{
			int corner = isSelectCorner;
			moveCorner(corner, moveW, moveH);

			if (myRect.width() < 0 || myRect.height() < 0 || myRect.x() < 0 || myRect.y() < 0)
			{
				correctRect(myRect);
				//recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());
				//pressPoint = event->pos();
				qDebug() << "corner change perssPoint:";
				qDebug() << pressPoint.x() << pressPoint.y();
			}

		}
		else if (isSelectSide > 0)
		{
			int side = isSelectSide;
			moveSide(side, moveW, moveH);

			if (myRect.width() < 0 || myRect.height() < 0 || myRect.x() < 0 || myRect.y() < 0)
			{
				correctRect(myRect);
				//recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());
				//pressPoint = event->pos();
				qDebug() << "side change perssPoint:";
				qDebug() << pressPoint.x() << pressPoint.y();
			}
		}
		else
		{
			//select black
			

		}

		update();
	}
	else
	{
		if (isInMyRect(event->pos()))
		{
			setCursor(Qt::SizeAllCursor);
		}
		else if (atCorner(event->pos()) > 0)
		{
			if (atCorner(event->pos()) == 2 || atCorner(event->pos()) == 4)
				setCursor(Qt::SizeBDiagCursor);
			else
				setCursor(Qt::SizeFDiagCursor);
		}
		else if (atSide(event->pos()) > 0)
		{
			if (atSide(event->pos()) == 2 || atSide(event->pos()) == 4)
				setCursor(Qt::SizeHorCursor);
			else
				setCursor(Qt::SizeVerCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}

	QWidget::mouseMoveEvent(event);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (!myRect.isValid() || myRect.x()<0 || myRect.y()<0)
		{
			correctRect(myRect);
		}
		update();

		isLBtnDown = false;
		isSelectIn = false;
		isSelectCorner = -1;
		isSelectSide = -1;

		qDebug() << "release after:";
		qDebug() << myRect.x() << myRect.y() << myRect.width() << myRect.height();
	}

	QWidget::mouseReleaseEvent(event);
}

void ImageLabel::leaveEvent(QEvent * event)
{
	qDebug() << "leaveEvent";
	if (isLBtnDown)
	{
		qDebug() << "isbtndown leaveEvent";

		// Get the window geometry & cursor position

		QSize lableSize = size();
		QPoint position = mapFromGlobal(QCursor::pos());

		qint32 x = qBound(0, position.x(), lableSize.width());
		qint32 y = qBound(0, position.y(), lableSize.height());

		// Adjust the cursor
		if (x != position.x() || y != position.y())
			QCursor::setPos(mapToGlobal(QPoint(x, y)));
		event->accept();
	}
	QLabel::leaveEvent(event);

	//// Get the window geometry & cursor position
	//const QRect & rect = geometry();
	//QPoint position = QCursor::pos();

	//// Check the bounds
	//qint32 x = qBound(rect.left(), position.x(), rect.right());
	//qint32 y = qBound(rect.top(), position.y(), rect.bottom());

	//// Adjust the cursor
	//if (x != position.x() || y != position.y())
	//	QCursor::setPos(x, y);

	//event->accept();
	//QMainWindow::leaveEvent(event);
}


bool ImageLabel::isInMyRect(QPoint point)
{
	int X = point.x();
	int Y = point.y();

	if (X > myRect.x() && X < myRect.x() + myRect.width())
	{
		if (Y > myRect.y() && Y < myRect.y() + myRect.height())
		{
			return true;
		}
	}
	return false;
}

int ImageLabel::atCorner(QPoint point)
{
	QPoint lefttop(myRect.x(), myRect.y());
	QPoint leftbottom(myRect.x(), myRect.y() + myRect.height());
	QPoint righttop(myRect.x() + myRect.width(), myRect.y());
	QPoint rightbottom(myRect.x() + myRect.width(), myRect.y() + myRect.height());

	if (qAbs((point - lefttop).x()) < 3 &&
		qAbs((point - lefttop).y()) < 3)
	{
		return 1;
	}
	else if (qAbs((point - leftbottom).x()) < 3 &&
		qAbs((point - leftbottom).y()) < 3)
	{
		return 4;
	}
	else if (qAbs((point - righttop).x()) < 3 &&
		qAbs((point - righttop).y()) < 3)
	{
		return 2;
	}
	else if (qAbs((point - rightbottom).x()) < 3 &&
		qAbs((point - rightbottom).y()) < 3)
	{
		return 3;
	}
	return -1;
}

int ImageLabel::atSide(QPoint point)
{
	if (qAbs(point.y() - myRect.top()) < 3 && point.x()>myRect.left() &&point.x() <myRect.right())
		return 1;
	else if (qAbs(point.x() - myRect.right()) < 3 && point.y() > myRect.top() && point.y() < myRect.bottom())
		return 2;
	else if (qAbs(point.y() - myRect.bottom()) < 3 && point.x() > myRect.left() && point.x() < myRect.right())
		return 3;
	else if (qAbs(point.x() - myRect.left()) < 3 && point.y() > myRect.top() && point.y() < myRect.bottom())
		return 4;
	return -1;
}

void ImageLabel::moveCorner(int corner, int moveW, int moveH)
{
	switch (corner)
	{
	case 1:
		myRect.setTopLeft(QPoint(recordRect.x() + moveW, recordRect.y() + moveH));
		break;
	case 2:
		myRect.setTopRight(QPoint(
			recordRect.x() + moveW + recordRect.width(),
			recordRect.y() + moveH));
		break;
	case 3:
		myRect.setBottomRight(QPoint(
			recordRect.x() + moveW + recordRect.width(),
			recordRect.y() + moveH + recordRect.height()));
		break;
	case 4:
		myRect.setBottomLeft(QPoint(
			recordRect.x() + moveW,
			recordRect.y() + moveH + recordRect.height()));
		break;
	default:
		break;
	}
}

void ImageLabel::moveSide(int side, int moveW, int moveH)
{
	switch (side)
	{
	case 1:
		myRect.setTop(recordRect.top() + moveH);
		break;
	case 2:
		myRect.setRight(recordRect.right() + moveW);
		break;
	case 3:
		myRect.setBottom(recordRect.bottom() + moveH);
		break;
	case 4:
		myRect.setLeft(recordRect.left() + moveW);
		break;
	default:
		break;
	}
}

void ImageLabel::correctRect(QRect & rect)
{
	qDebug() << "correct";
	qDebug() << rect.x() << rect.y() << rect.width() << rect.height();

	myRect.normalized();

	if (rect.width() < 0)
	{
		int tempW = rect.width();
		rect.setX(rect.x() + rect.width());
		rect.setWidth(-tempW);
	}
	if (rect.height() < 0)
	{
		int tempH = rect.height();
		rect.setY(rect.y() + rect.height());
		rect.setHeight(-tempH);
	}

	if (rect.x() < 0)
	{
		rect.moveLeft(0);
	}

	if (rect.y() < 0)
	{
		rect.moveTop(0);
	}
	qDebug() << "correct after:";
	qDebug() << rect.x() << rect.y() << rect.width() << rect.height();
}
