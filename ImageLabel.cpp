#include "ImageLabel.h"

#include <QPainter>
#include <QDebug>

ImageLabel::ImageLabel(QWidget *parent)
	: QLabel(parent),
	isSelectIn(false),
	isSelectCorner(-1),
	isSelectSide(-1),
	isLBtnDown(false)
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

	painter.setPen(QPen(Qt::red,2));
	painter.drawRect(myRect);
}

void ImageLabel::mousePressEvent(QMouseEvent * event)
{
	qDebug() << "1.pressEvent";
	if (event->button() == Qt::LeftButton)
	{
		isLBtnDown = true;
		pressPoint = event->pos();

		//假如选中myrect
		recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());
		pathfinderRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());

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
			
		}
	}

	QWidget::mousePressEvent(event);
}

void ImageLabel::mouseMoveEvent(QMouseEvent * event)
{
	//qDebug() << "2.moveEvent";
	if (isLBtnDown)
	{
		qDebug() << "move event and isLbtnDown";

		////Boundary detection
		////cursor cannot leave the label when isLBtnDown
		//QSize lableSize = size();
		////QPoint position = mapFromGlobal(QCursor::pos()); //bug：QPoint movePoint = event->pos()可能为负
		//QPoint position = event->pos();

		//qint32 x = qBound(0, position.x(), lableSize.width());
		//qint32 y = qBound(0, position.y(), lableSize.height());

		//// Adjust the cursor
		//if (x != position.x() || y != position.y())
		//{
		//	qDebug() << "the cursor leave the lable:";
		//	qDebug() << position.x()<< position.y();


		//	QCursor::setPos(mapToGlobal(QPoint(x, y)));
		//	qDebug() << "adjust the cursor:";

		//	QPoint position2 = mapFromGlobal(QCursor::pos());
		//	qDebug() << position2.x() << position2.y();
		//	
		//	event->accept();
		//	QLabel::mouseMoveEvent(event);
		//	return;
		//}

		QPoint movePoint = event->pos();
		qDebug() << "2.1:movePoint:";
		qDebug() << movePoint.x() << movePoint.y();

		int moveW = movePoint.x() - pressPoint.x();
		int moveH = movePoint.y() - pressPoint.y();


		if (isSelectIn)
		{
			setCursor(Qt::SizeAllCursor);
			int moveX = recordRect.x() + moveW;
			int moveY = recordRect.y() + moveH;


			//1.我们先移动探路先锋，允许其出界 2.如果出界将其移回/重构 -> myRect 3.绘制...

			//pathfinderRect.moveTopLeft(QPoint(moveX, moveY));
			pathfinderRect.setRect(moveX,moveY,recordRect.width(),recordRect.height());
			qDebug() << "pathfinderRect:";
			qDebug() << pathfinderRect.x() << pathfinderRect.y();

			if (rectInLabel(pathfinderRect))
			{
				myRect = pathfinderRect;
			}
			else
			{
				myRect = movebackRect(pathfinderRect);

				if (pointInLabel(event->pos()))
				{
					recordRect.setRect(myRect.x(), myRect.y(), myRect.width(), myRect.height());
					pressPoint = event->pos();
					qDebug() << "======================================change perssPoint:";
					qDebug() << pressPoint.x() << pressPoint.y();
				}
			}
		}
		else if (isSelectCorner > 0)
		{
			int corner = isSelectCorner;
			moveCorner(corner, moveW, moveH);
			if(rectInLabel(pathfinderRect))
			{
				myRect = pathfinderRect.normalized();
			}
			else
			{
				myRect = correctRect(pathfinderRect);
			}
			
		}
		else if (isSelectSide > 0)
		{
			int side = isSelectSide;
			moveSide(side, moveW, moveH);
			if (rectInLabel(pathfinderRect))
			{
				myRect = pathfinderRect.normalized();
			}
			else
			{
				myRect = correctRect(pathfinderRect);
			}
		}
		else
		{
			//select black
			int w = movePoint.x() - pressPoint.x();
			int h = movePoint.y() - pressPoint.y();

			pathfinderRect.setRect(pressPoint.x(), pressPoint.y(),w,h);
			if (!pathfinderRect.isNull())
			{
				if (rectInLabel(pathfinderRect))
				{
					myRect = pathfinderRect.normalized();
				}
				else
				{
					myRect = correctRect(pathfinderRect);
				}
			}
		}

		update();
	}
	else
	{
	qDebug() << "=====mouseEvent and not down";
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
	qDebug() << "3.release";
	if (event->button() == Qt::LeftButton)
	{
		
		myRect = myRect.normalized();

		//update();

		isLBtnDown = false;
		isSelectIn = false;
		isSelectCorner = -1;
		isSelectSide = -1;

		qDebug() << "release after:";
		qDebug() << myRect.x() << myRect.y() << myRect.width() << myRect.height();
	}

	QWidget::mouseReleaseEvent(event);
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
		pathfinderRect.setTopLeft(QPoint(recordRect.x() + moveW, recordRect.y() + moveH));
		break;
	case 2:
		pathfinderRect.setTopRight(QPoint(
			recordRect.x() + moveW + recordRect.width(),
			recordRect.y() + moveH));
		break;
	case 3:
		pathfinderRect.setBottomRight(QPoint(
			recordRect.x() + moveW + recordRect.width(),
			recordRect.y() + moveH + recordRect.height()));
		break;
	case 4:
		pathfinderRect.setBottomLeft(QPoint(
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
		pathfinderRect.setTop(recordRect.top() + moveH);
		break;
	case 2:
		pathfinderRect.setRight(recordRect.right() + moveW);
		break;
	case 3:
		pathfinderRect.setBottom(recordRect.bottom() + moveH);
		break;
	case 4:
		pathfinderRect.setLeft(recordRect.left() + moveW);
		break;
	default:
		break;
	}
}

QRect ImageLabel::correctRect(const QRect & rect) const
{
	//We recommend that you use x() + width() and y() + height() 
	//to find the true bottom-right corner, and avoid right() and bottom().
	qDebug() << "correct";
	qDebug() << rect.x() << rect.y() << rect.width() << rect.height();

	QRect resultRect = rect.normalized();

	if (resultRect.x() < 0)
	{
		resultRect.setLeft(0);
	}
	if (resultRect.y() < 0)
	{
		resultRect.setTop(0);
	}
	if (resultRect.x() + resultRect.width() > size().width())
	{
		resultRect.setRight(size().width()-1);
	}
	if (resultRect.y() + resultRect.height() > size().height())
	{
		resultRect.setBottom(size().height()-1);
	}

	return resultRect;
	

	qDebug() << "correct after:";
	qDebug() << rect.x() << rect.y() << rect.width() << rect.height();
}

bool ImageLabel::pointInLabel(const QPoint & point)
{
	QRect labelRect(0, 0, size().width(), size().height());
	return labelRect.contains(point,false);
}

bool ImageLabel::rectInLabel(const QRect & rect)
{
	QRect labelRect(0, 0, size().width(), size().height());
	return labelRect.contains(rect, false);
}

QRect ImageLabel::movebackRect(const QRect & rect) const
{
	QRect resultRect = rect.normalized();

	if (resultRect.x() < 0)
	{
		resultRect.translate(0 - resultRect.x(),0);
	}
	if (resultRect.y() < 0)
	{
		resultRect.translate(0, 0 - resultRect.y());

	}
	if (resultRect.x()+resultRect.width() > size().width())
	{
		resultRect.translate(size().width() - (resultRect.x() + resultRect.width()), 0);
	}
	if (resultRect.y()+resultRect.height() > size().height())
	{
		resultRect.translate(0, size().height() - (resultRect.y() + resultRect.height()));

	}
	
	return resultRect;
}
