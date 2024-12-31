#pragma once

#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QSlider>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QTextEdit>

#include <opencv4/opencv2/opencv.hpp>

#include "./widgets/beforeafterfilteringwidget.h"

namespace Ui {
class FourierCropperSimulatorUi;
}

class DrawWidget;
class ScrollableMessageBox;

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
        cv::Mat originalResizedImage;
        cv::Mat inputImage;
        QImage mask;
        QImage mInputImageQImage;
        QPoint lastPoint;
        int pencilSize;
        int maxHeight;
        int maxWidth;
        bool drawing;
};

class ScrollableMessageBox : public QDialog
{
    public:
        explicit ScrollableMessageBox(const QString& message, QWidget* parent = nullptr);

    private:
        QVBoxLayout* layout = nullptr;
};

class FourierCropperSimulatorUi : public QWidget
{
    Q_OBJECT

    public:
        explicit FourierCropperSimulatorUi(QWidget* parent = nullptr);
        ~FourierCropperSimulatorUi();

        void initializeSimulator();

    private:
        Ui::FourierCropperSimulatorUi* mUi = nullptr;

        ScrollableMessageBox* mHelpDialog = nullptr;

        BeforeAfterFilteringWidget* mBeforeAfterWidget = nullptr;

        cv::Mat mInputImage;
        cv::Mat mMagnitude;
        cv::Mat mMagnitude_vis;
        cv::Mat mMask;

        cv::Mat mOriginalImage;

        QImage mBeforeQImage;
        QImage mAfterQImage;

        void generateMask();

    private slots:
        void on_pencilSizeSlider_valueChanged(int value);
        void on_saveMaskButton_clicked();
        void on_helpPushButton_released();
        void on_loadImagePushButton_released();
        void on_radioButtonCropMode_toggled(bool checked);
        void on_filterPushButton_released();
        void on_fullscreenButton_released();
};
