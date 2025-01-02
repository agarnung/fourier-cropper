#pragma once

#include <QWidget>
#include <QFile>
#include <QPushButton>
#include <QFileDialog>
#include <QImage>
#include <QSlider>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>

#include <opencv4/opencv2/core.hpp>

#include "./widgets/beforeafterfilteringwidget.h"
#include "./widgets/scrollablemessagebox.h"

namespace Ui {
class FourierCropperSimulatorUi;
}

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

        bool mImageIsColored;

    private slots:
        void on_pencilSizeSlider_valueChanged(int value);
        void on_saveMaskButton_clicked();
        void on_helpPushButton_released();
        void on_loadImagePushButton_released();
        void on_radioButtonCropMode_toggled(bool checked);
        void on_filterPushButton_released();
        void on_fullscreenButton_released();
};
