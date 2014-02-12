#include <algorithm>

#include <QLayout>
#include <QScrollBar>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QDebug>

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"

#include <ZLTextView.h>

class MyQScrollBar : public QScrollBar {

public:
	MyQScrollBar(Qt::Orientation orientation, QWidget *parent) : QScrollBar(orientation, parent) {
	}

private:
	void mouseMoveEvent(QMouseEvent *event) {
		if (event->type() == QEvent::MouseMove) {
			return;
		}
//		if (orientation() == Qt::Vertical) {
//			const int y = event->y();
//			if ((y <= 0) || (y >= height())) {
//				return;
//			}
//		} else {
//			const int x = event->x();
//			if ((x <= 0) || (x >= width())) {
//				return;
//			}
//		}
		QScrollBar::mouseMoveEvent(event);
	}
};


ZLQtViewWidget::Widget::Widget(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
	//setBackgroundMode(NoBackground);
}

QScrollBar *ZLQtViewWidget::addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y) {
	QScrollBar *scrollBar = new MyQScrollBar(orientation, myFrame);
	layout->addWidget(scrollBar, x, y);
	scrollBar->hide();
	if (orientation == Qt::Vertical) {
		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onVerticalSliderMoved(int)));
		connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(onVerticalSliderClicked(int)));
	} else {
		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSliderMoved(int)));
		connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(onHorizontalSliderClicked(int)));
	}
	return scrollBar;
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLApplication *application) : ZLViewWidget((ZLView::Angle)application->AngleStateOption.value()), myApplication(application) {
	myFrame = new QWidget(parent);
	QGridLayout *layout = new QGridLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	myFrame->setLayout(layout);
	myQWidget = new Widget(myFrame, *this);
	layout->addWidget(myQWidget, 1, 1);

	myRightScrollBar = addScrollBar(layout, Qt::Vertical, 1, 2);
	myLeftScrollBar = addScrollBar(layout, Qt::Vertical, 1, 0);
	myShowScrollBarAtRight = true;

	myBottomScrollBar = addScrollBar(layout, Qt::Horizontal, 2, 1);
	myTopScrollBar = addScrollBar(layout, Qt::Horizontal, 0, 1);
	myShowScrollBarAtBottom = true;

	myRightScrollBar->setDisabled(true);
	myLeftScrollBar->setDisabled(true);
	myTopScrollBar->setDisabled(true);
	myBottomScrollBar->setDisabled(true);
}

void ZLQtViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::Widget::paintEvent(QPaintEvent*) {
	ZLQtPaintContext &context = (ZLQtPaintContext&)myHolder.view()->context();
	switch (myHolder.rotation()) {
		default:
			context.setSize(width(), height());
			break;
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			context.setSize(height(), width());
			break;
	}
	myHolder.view()->paint();
	QPainter realPainter(this);
	switch (myHolder.rotation()) {
		default:
			realPainter.drawPixmap(0, 0, context.pixmap());
			break;
		case ZLView::DEGREES90:
			realPainter.rotate(270);
			realPainter.drawPixmap(1 - height(), -1, context.pixmap());
			break;
		case ZLView::DEGREES180:
			realPainter.rotate(180);
			realPainter.drawPixmap(1 - width(), 1 - height(), context.pixmap());
			break;
		case ZLView::DEGREES270:
			realPainter.rotate(90);
			realPainter.drawPixmap(-1, 1 - width(), context.pixmap());
			break;
	}
}

class ZLTextViewHook : public ZLTextView {
public:
	using ZLTextView::onStylusClick;
};

void ZLQtViewWidget::Widget::mousePressEvent(QMouseEvent *event) {
    //Implementation of this method is kind of hack, because
    //it doesn't use internal code of ZLView for recognizing mouse's click
    //it just calls onFingerTap and onStylusClick directly
    //TODO reimplement using just onStylusPressed and onStylusReleased methods of ZLView class
    bool isLink = false;   
    if (ZLTextViewHook* bookTextView = zlobject_cast<ZLTextViewHook*>(&*myHolder.view())) {
        // count == 0 means that it returns false in case it's not a link in text
        isLink = bookTextView->onStylusClick(x(event), y(event), 0);
    }
    if (!isLink) {
        if (isTapBottomZone(QPoint(event->pos()))) {
            emit myHolder.tapBottomZoneClicked();
            return;
        }
        // if it's not a link in BookTextView, do a finger tap for scrolling on prev/next page
        myHolder.view()->onFingerTap(x(event), y(event));
    }
}

void ZLQtViewWidget::Widget::mouseReleaseEvent(QMouseEvent *event) {
    //why empty implentation? see comment in mousePressentEvent method
}

bool ZLQtViewWidget::Widget::isTapBottomZone(QPoint coords) {
//    return coords.y() >= (this->size().height() - MenuItemParameters::getTapBottomZoneSize());
	return false;
}

int ZLQtViewWidget::Widget::x(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES90:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES180:
			return maxX - std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES270:
			return std::min(std::max(event->y(), 0), maxY);
	}
}

int ZLQtViewWidget::Widget::y(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES90:
			return std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES180:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES270:
			return maxX - std::min(std::max(event->x(), 0), maxX);
	}
}

void ZLQtViewWidget::repaint()	{
	myQWidget->repaint();
}

void ZLQtViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
	if (direction == ZLView::VERTICAL) {
		myRightScrollBar->setShown(enabled && myShowScrollBarAtRight);
		myLeftScrollBar->setShown(enabled && !myShowScrollBarAtRight);
	} else {
		myBottomScrollBar->setShown(enabled && myShowScrollBarAtBottom);
		myTopScrollBar->setShown(enabled && !myShowScrollBarAtBottom);
	}
}

void ZLQtViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
	if ((rotation() == ZLView::DEGREES90) || (rotation() == ZLView::DEGREES270)) {
		if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
			standard = !standard;
		}
	}
	if (direction == ZLView::VERTICAL) {
		if (standard != myShowScrollBarAtRight) {
			myShowScrollBarAtRight = standard;
			QScrollBar *old = standard ? myLeftScrollBar : myRightScrollBar;
			QScrollBar *current = standard ? myRightScrollBar : myLeftScrollBar;
			if (old->isVisible()) {
				old->hide();
                                current->show();
			}
		}
	} else {
		if (standard != myShowScrollBarAtBottom) {
			myShowScrollBarAtBottom = standard;
			QScrollBar *old = standard ? myTopScrollBar : myBottomScrollBar;
			QScrollBar *current = standard ? myBottomScrollBar : myTopScrollBar;
                        if (old->isVisible()) {
				old->hide();
                                current->show();
			}
		}
	}
}

void ZLQtViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
	QScrollBar *bar =
		(direction == ZLView::VERTICAL) ?
			(myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar) :
			(myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar);
	bar->setMinimum(0);
	bar->setMaximum(full + from - to);
	bar->setValue(from);
	bar->setPageStep(to - from);
}

void ZLQtViewWidget::onVerticalSliderMoved(int value) {
	QScrollBar *bar =
		myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar;
	int maxValue = bar->maximum();
	int pageStep = bar->pageStep();
	value = std::max(std::min(value, maxValue), 0);
	onScrollbarMoved(
		ZLView::VERTICAL,
		maxValue + pageStep,
		value,
		value + pageStep
	);
}

void ZLQtViewWidget::onHorizontalSliderMoved(int value) {
	QScrollBar *bar =
		myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar;
	int maxValue = bar->maximum();
	int pageStep = bar->pageStep();
	value = std::max(std::min(value, maxValue), 0);
	onScrollbarMoved(
		ZLView::HORIZONTAL,
		maxValue + pageStep,
		value,
		value + pageStep
	);
}

void ZLQtViewWidget::onVerticalSliderClicked(int value) {
	switch (value) {
		case QScrollBar::SliderSingleStepAdd:
			onScrollbarStep(ZLView::VERTICAL, 1);
			break;
		case QScrollBar::SliderSingleStepSub:
			onScrollbarStep(ZLView::VERTICAL, -1);
			break;
		case QScrollBar::SliderPageStepAdd:
			onScrollbarPageStep(ZLView::VERTICAL, 1);
			break;
		case QScrollBar::SliderPageStepSub:
			onScrollbarPageStep(ZLView::VERTICAL, -1);
			break;
	}
}

void ZLQtViewWidget::onHorizontalSliderClicked(int value) {
	switch (value) {
		case QScrollBar::SliderSingleStepAdd:
			onScrollbarStep(ZLView::HORIZONTAL, 1);
			break;
		case QScrollBar::SliderSingleStepSub:
			onScrollbarStep(ZLView::HORIZONTAL, -1);
			break;
		case QScrollBar::SliderPageStepAdd:
			onScrollbarPageStep(ZLView::HORIZONTAL, 1);
			break;
		case QScrollBar::SliderPageStepSub:
			onScrollbarPageStep(ZLView::HORIZONTAL, -1);
			break;
	}
}

QWidget *ZLQtViewWidget::widget() {
	return myFrame;
}
