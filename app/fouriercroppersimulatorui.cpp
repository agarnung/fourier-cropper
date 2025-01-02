#include "fouriercroppersimulatorui.h"
#include "ui_fouriercroppersimulatorui.h"
#include "./libs/fourier.h"

#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

FourierCropperSimulatorUi::FourierCropperSimulatorUi(QWidget* parent)
    : QWidget(parent)
    , mUi(new Ui::FourierCropperSimulatorUi)
{
    mUi->setupUi(this);
}

FourierCropperSimulatorUi::~FourierCropperSimulatorUi()
{
    if (mUi)
    {
        delete mUi;
        mUi = nullptr;
    }

    if (mBeforeAfterWidget)
    {
        delete mBeforeAfterWidget;
        mBeforeAfterWidget = nullptr;
    }
}

void FourierCropperSimulatorUi::initializeSimulator()
{
    mBeforeAfterWidget = new BeforeAfterFilteringWidget(nullptr);

    mUi->pencilSizeSlider->setRange(1, 100);
    mUi->pencilSizeSlider->setValue(50);

    QFile styleFile(":/configsFC/configsFC/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        this->setStyleSheet(styleSheet);

        styleFile.close();
    }
    else
        qWarning() << QObject::tr("%1 - %2 - Could not open stylesheet").arg(this->metaObject()->className()).arg(__func__);

    mUi->pencilSizeSlider->setValue(25);
    emit mUi->pencilSizeSlider->valueChanged(25);

    this->update();
}

void FourierCropperSimulatorUi::generateMask()
{
    QImage mask = mUi->drawWidget->getMask();
    mask = mask.convertToFormat(QImage::Format_Grayscale8);

    for (int y = 0; y < mask.height(); ++y)
    {
        for (int x = 0; x < mask.width(); ++x)
        {
            QRgb pixel = mask.pixel(x, y);
            if (qRed(pixel) > 0)
                mask.setPixel(x, y, mUi->radioButtonCropMode->isChecked() ? qRgb(0, 0, 0) : qRgb(255, 255, 255)); // Black
            else
                mask.setPixel(x, y, mUi->radioButtonCropMode->isChecked() ? qRgb(255, 255, 255) : qRgb(0, 0, 0)); // White
        }
    }

    mMask = cv::Mat(mask.height(), mask.width(), CV_8UC1, (void*)mask.bits(), mask.bytesPerLine());
    cv::resize(mMask, mMask, cv::Size(mInputImage.cols, mInputImage.rows), 0, 0, cv::INTER_NEAREST);
}

void FourierCropperSimulatorUi::on_pencilSizeSlider_valueChanged(int value)
{
    mUi->drawWidget->setPencilSize(value);
    mUi->pencilSizeValue->setText(QString::number(value));
}

void FourierCropperSimulatorUi::on_saveMaskButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save mask"), "", tr("PNG Files (*.png);;All Files (*)"));

    if (filePath.isEmpty() || QFileInfo(filePath).suffix().isEmpty())
        filePath += ".png";

    generateMask();

    if (!cv::imwrite(filePath.toStdString(), mMask))
        QMessageBox::warning(this, tr("Save mask"), tr("Could not find mask in %1").arg(filePath));
}

void FourierCropperSimulatorUi::on_helpPushButton_released()
{
    if (!mHelpDialog)
    {
        QString helpMessage;
        QFile file(":/configsFC/configsFC/help.html");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            helpMessage += stream.readAll();
            file.close();
        }
        else
            qWarning() << QObject::tr("%1 - %2 - Error opening style sheet.").arg(this->metaObject()->className()).arg(__func__ );

        mHelpDialog = new ScrollableMessageBox(helpMessage, this);
        mHelpDialog->hide();
    }

    if (mHelpDialog->isHidden())
    {
        QScreen* currentScreen = nullptr;

        QPoint widgetGlobalPos = mHelpDialog->mapToGlobal(QPoint(0, 0));

        for (QScreen* screen : QGuiApplication::screens())
        {
            if (screen->geometry().contains(widgetGlobalPos))
                currentScreen = screen;
        }

        QRect availableGeometry = currentScreen->availableGeometry();
        mHelpDialog->show();
        mHelpDialog->move(availableGeometry.center() - QPoint(mHelpDialog->width() / 2, mHelpDialog->height() / 2));
    }
}

void FourierCropperSimulatorUi::on_loadImagePushButton_released()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.bmp *.jpeg *.tif *.tiff)"));

    if (!filePath.isEmpty())
    {
        mInputImage = cv::imread(filePath.toStdString(), cv::IMREAD_UNCHANGED);
        if (mInputImage.channels() == 4)
            cv::cvtColor(mInputImage, mInputImage, cv::COLOR_BGRA2BGR);

        mImageIsColored = mInputImage.channels() == 3 ? true : false;

        if (!mInputImage.empty())
        {
            cv::Mat mInputImage_64F = universalConvertTo(mInputImage, CV_64FC1);
            transformadaFourier2D(mInputImage_64F, &mMagnitude);
            cv::add(mMagnitude, cv::Mat::ones(mMagnitude.rows, mMagnitude.cols, mMagnitude.type()) / 1e9, mMagnitude_vis);
            cv::log(mMagnitude_vis, mMagnitude_vis);
            cv::normalize(mMagnitude_vis, mMagnitude_vis, 0.0, 1.0, cv::NORM_MINMAX, CV_64F);
            mMagnitude_vis = universalConvertTo(fftshift(mMagnitude_vis), CV_8UC1);

            mUi->drawWidget->setImage(mMagnitude_vis);

            cv::Mat displayImage = universalConvertTo(mInputImage, CV_8UC1);

            QImage qImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_Grayscale8);
            mBeforeQImage = qImage.copy();

            this->update();
            this->updateGeometry();
        }
        else
            QMessageBox::warning(this, tr("Error"), tr("The image could not be opened"));
    }
}

void FourierCropperSimulatorUi::on_radioButtonCropMode_toggled(bool checked)
{
    mUi->radioButtonCropMode->setText(checked ? "Crop Mode ON " : "Crop Mode OFF");
}

void FourierCropperSimulatorUi::on_filterPushButton_released()
{
    auto showErrorMessage = [](const QString& txt){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Fourier Cropper");
        msgBox.setText(txt);
        msgBox.exec();
        return;
    };

    if (mInputImage.empty())
    {
        showErrorMessage("The input image must exist");
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Processing...");
    msgBox.show();

    generateMask(); // with whatever is drawn at the moment

    cv::Mat filter, filtered;
    mMask.convertTo(filter, CV_64FC1, 1.0 / 255.0);
    cv::resize(filter, filter, mInputImage.size(), 0.0, 0.0, cv::INTER_NEAREST_EXACT);
    if (mImageIsColored)
    {
        std::vector<cv::Mat> channels(3);
        std::vector<cv::Mat> filtered_channels(3);
        cv::split(mInputImage, channels);
        foreach (const cv::Mat& ch, channels)
        {
            cv::Mat filtered_channel(mInputImage.size(), CV_64FC1);
            filter2DFreq(universalConvertTo(ch, CV_64FC1), filtered, filter, false, true);
            filtered_channels.emplace_back(filtered);
        }
        cv::merge(filtered_channels, filtered);
    }
    else
        filter2DFreq(universalConvertTo(mInputImage, CV_64FC1), filtered, filter, false, true);

    if (filtered.empty())
    {
        showErrorMessage("There was an error filtering the image");
        return;
    }

    msgBox.close();

    filtered.convertTo(filtered, mImageIsColored ? CV_8UC3 : CV_8UC1, 255.0);

    if (!mBeforeAfterWidget)
    {
        showErrorMessage("The filtered image could not be shown");
        return;
    }

    {
        QImage qImage(filtered.data, filtered.cols, filtered.rows, filtered.step, mImageIsColored ? QImage::Format_RGB888 : QImage::Format_Grayscale8);
        mAfterQImage = qImage.copy();
    }

    mBeforeAfterWidget->setBeforeImage(mBeforeQImage);
    mBeforeAfterWidget->setAfterImage(mAfterQImage);
    mBeforeAfterWidget->setFilteredImage(mAfterQImage);

    if (mBeforeAfterWidget->isHidden())
        mBeforeAfterWidget->show();

    if (!mBeforeAfterWidget->isVisible())
        mBeforeAfterWidget->setVisible(true);
}

void FourierCropperSimulatorUi::on_fullscreenButton_released()
{
    this->isFullScreen() ? this->setWindowState(Qt::WindowNoState) :
                           this->setWindowState(Qt::WindowFullScreen);

    QIcon icon1;
    this->isFullScreen() ? icon1.addFile(QString::fromUtf8(":/iconosFC/iconosFC/minimize-compress-svgrepo-com.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off) :
        icon1.addFile(QString::fromUtf8(":/iconosFC/iconosFC/maximize-expand-svgrepo-com.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    mUi->fullscreenButton->setIcon(icon1);
    mUi->fullscreenButton->setIconSize(QSize(25, 25));

    this->update();
}

