#include "fouriercroppersimulatorui.h"
#include "ui_fouriercroppersimulatorui.h"
#include "./libs/fourier.h"

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

    cv::Mat displayImage;
    cv::cvtColor(inputImage, displayImage, cv::COLOR_GRAY2BGR);
    // cv::normalize(displayImage, displayImage, 0, 255, cv::NORM_MINMAX);
    // cv::convertScaleAbs(displayImage, displayImage);

    QImage qImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
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
    // cv::resize(inputImage, inputImage, cv::Size(1920, 1080));
    mask.fill(Qt::transparent);
    updateDisplay();
    this->update();
}

ScrollableMessageBox::ScrollableMessageBox(const QString& message, QWidget* parent)
    : QDialog(parent, Qt::WindowTitleHint)
{
    this->setWindowTitle("Ayuda");
    this->setMinimumSize(100, 100);
    this->setGeometry(parent->geometry().x() + 200, parent->geometry().y() + 100, 1000, 500);
    this->adjustSize();

    setWindowFlag(Qt::WindowStaysOnTopHint, false);

    layout = new QVBoxLayout(this);

    QTextEdit* textEdit = new QTextEdit(this);
    textEdit->setHtml(message);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QString styleSheet =
        "body { font-size: 13px; font-family: Arial; color: #FFF; text-align: justify; margin-right: 10px; }"
        "h2 { font-size: 15px; font-weight: bold; color: #007bff; }"
        "li { margin-bottom: 8px; }"
        "p { color: #FF0000; }";

    textEdit->document()->setDefaultStyleSheet(styleSheet);

    layout->addWidget(textEdit);
}

FourierCropperSimulatorUi::FourierCropperSimulatorUi(QWidget *parent)
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
        mInputImage = cv::imread(filePath.toStdString(), cv::IMREAD_GRAYSCALE);

        if (!mInputImage.empty())
        {
            cv::Mat mInputImage_64F = universalConvertTo(mInputImage, CV_64FC1);
            transformadaFourier2D(mInputImage_64F, &mMagnitude);
            cv::add(mMagnitude, cv::Mat::ones(mMagnitude.rows, mMagnitude.cols, mMagnitude.type()) / 1e9, mMagnitude_vis);
            cv::log(mMagnitude_vis, mMagnitude_vis);
            cv::normalize(mMagnitude_vis, mMagnitude_vis, 0.0, 1.0, cv::NORM_MINMAX, CV_64F);
            mMagnitude_vis = universalConvertTo(fftshift(mMagnitude_vis), CV_8UC1);

            mUi->drawWidget->setImage(mMagnitude_vis);

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
    filter2DFreq(universalConvertTo(mInputImage, CV_64FC1), filtered, filter, false, true);

    if (filtered.empty())
    {
        showErrorMessage("There was an error filtering the image");
        return;
    }

    filtered.convertTo(filtered, CV_8UC1, 255.0);

    msgBox.close();

    {
        cv::Mat displayImage;
        cv::cvtColor(universalConvertTo(mInputImage, CV_8UC1), displayImage, cv::COLOR_GRAY2BGR);
        mBeforeQImage = QImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
        mBeforeQImage.save("/home/alejandro/Pictures/mBeforeQImage.png", 0, 100);
    }

    {
        cv::Mat displayImage;
        cv::cvtColor(universalConvertTo(filtered, CV_8UC1), displayImage, cv::COLOR_GRAY2BGR);
        mAfterQImage = QImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
        mAfterQImage.save("/home/alejandro/Pictures/mAfterQImage.png", 0, 100);
    }

    if (!mBeforeAfterWidget)
    {
        showErrorMessage("There was an error filtering the image");
        return;
    }

    mBeforeAfterWidget->setBeforeImage(mBeforeQImage);
    mBeforeAfterWidget->setAfterImage(mAfterQImage);

    if (mBeforeAfterWidget->isHidden())
        mBeforeAfterWidget->show();
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

