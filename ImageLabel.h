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
	void leaveEvent(QEvent *event) override;


private:
	bool isInMyRect(QPoint point);
	int atCorner(QPoint point);
	int atSide(QPoint point);

	void moveCorner(int corner, int w, int h);
	void moveSide(int side, int w, int h);

	void correctRect(QRect &rect);

	QRect myRect;
	QRect recordRect;

	bool isSelectIn;
	int isSelectCorner;
	int isSelectSide;


	bool isLBtnDown;

	QPoint pressPoint;
	QPoint releasePoint;
};
