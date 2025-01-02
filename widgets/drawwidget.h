#pragma once

#include <QLabel>
#include <QPaintEvent>

#include <opencv4/opencv2/core.hpp>

class DrawWidget : public QLabel
{
    Q_OBJECT

    public:
        DrawWidget(QWidget *parent = nullptr);

        void setImage(const cv::Mat &image);
        QImage getMask();
        void setPencilSize(int size);

    protected:
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void keyPressEvent(QKeyEvent *event) override;

    private:
        void updateDisplay();
        void resetDrawing();

        cv::Mat originalImage;
        cv::Mat inputImage;
        QImage mask;
        QImage mInputImageQImage;
        QPoint lastPoint;
        int pencilSize;
        double maxHeight;
        double maxWidth;
        bool drawing;
};

