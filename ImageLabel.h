#pragma once

#include <QLabel>
#include <QMouseEvent>

class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	ImageLabel(QWidget *parent = nullptr);
	~ImageLabel();

protected:
	void paintEvent(QPaintEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	//void leaveEvent(QEvent *event) override;


private:
	bool isInMyRect(QPoint point);
	int atCorner(QPoint point);
	int atSide(QPoint point);

	void moveCorner(int corner, int w, int h);
	//QRect moveCorner(const QRect & rect,int corner, int w, int h);
	void moveSide(int side, int w, int h);

	bool pointInLabel(const QPoint &point);
	bool rectInLabel(const QRect &rect);

	QRect movebackRect(const QRect &rect) const;
	QRect correctRect(const QRect &rect) const;

	QRect myRect;   //绘制的，真实的rect数据
	QRect recordRect; //记录移动的rect最初的模样
	QRect pathfinderRect;

	bool isSelectIn;
	int isSelectCorner;
	int isSelectSide;


	bool isLBtnDown;

	QPoint pressPoint;
	QPoint releasePoint;
};
