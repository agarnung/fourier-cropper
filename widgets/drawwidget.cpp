#include "drawwidget.h"

#include <QPainter>

#include <opencv4/opencv2/imgproc.hpp>

DrawWidget::DrawWidget(QWidget* parent)
    : QLabel(parent)
    , pencilSize(10)
    , drawing(false)
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFocusPolicy(Qt::StrongFocus);

    maxHeight = qApp->screens()[0]->size().height() * 0.8;
    maxWidth = qApp->screens()[0]->size().width() * 0.8;
}

void DrawWidget::setImage(const cv::Mat& image)
{
    inputImage = image.clone();

    if (originalImage.size().height > maxHeight)
        cv::resize(originalImage, originalImage, cv::Size(originalImage.size().width, maxHeight));

    if (originalImage.size().width > maxWidth)
        cv::resize(originalImage, originalImage, cv::Size(maxWidth, originalImage.size().height));

    originalImage = inputImage.clone();

    this->setFixedSize(QSize(inputImage.cols, inputImage.rows));

    QImage qImage(inputImage.data, inputImage.cols, inputImage.rows, inputImage.step, QImage::Format_Grayscale8);
    mInputImageQImage = qImage.copy();

    mask = QImage(inputImage.cols, inputImage.rows, QImage::Format_ARGB32);
    mask.fill(Qt::transparent);

    updateDisplay();
}

QImage DrawWidget::getMask()
{
    return mask;
}

void DrawWidget::setPencilSize(int size)
{
    pencilSize = size;
}

void DrawWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        drawing = true;
        lastPoint = event->pos();
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (drawing && event->buttons() & Qt::LeftButton)
    {
        QPainter painter(&mask);
        painter.setPen(QPen(Qt::red, pencilSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        this->update();
    }
}

void DrawWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        drawing = false;
}

void DrawWidget::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.drawImage(0, 0, mInputImageQImage);
    painter.drawImage(0, 0, mask);
}

void DrawWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_R)
        resetDrawing();
}

void DrawWidget::updateDisplay()
{
    this->setPixmap(QPixmap::fromImage(mInputImageQImage));
}

void DrawWidget::resetDrawing()
{
    inputImage = originalImage.clone();
    mask.fill(Qt::transparent);
    updateDisplay();
    this->update();
}
