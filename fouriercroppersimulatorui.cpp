#include "fouriercroppersimulatorui.h"
#include "ui_fouriercroppersimulatorui.h"
#include "fourier.h"

DrawWidget::DrawWidget(QWidget *parent) : QLabel(parent), drawing(false), pencilSize(10)
{
    setFixedSize(1280, 720);
    setFocusPolicy(Qt::StrongFocus);
}

void DrawWidget::setImage(const cv::Mat &image)
{
    originalImage = image.clone();
    inputImage = image.clone();
    cv::resize(inputImage, inputImage, cv::Size(1280, 720));

    cv::Mat displayImage;
    cv::cvtColor(inputImage, displayImage, cv::COLOR_GRAY2BGR);
    cv::normalize(displayImage, displayImage, 0, 255, cv::NORM_MINMAX);
    cv::convertScaleAbs(displayImage, displayImage);

    QImage qImage(displayImage.data, displayImage.cols, displayImage.rows, displayImage.step, QImage::Format_RGB888);
    inputImageQImage = qImage.copy();

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

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        drawing = true;
        lastPoint = event->pos();
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (drawing && event->buttons() & Qt::LeftButton)
    {
        QPainter painter(&mask);
        painter.setPen(QPen(Qt::red, pencilSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        update();
    }
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        drawing = false;
}

void DrawWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.drawImage(0, 0, inputImageQImage);
    painter.drawImage(0, 0, mask);

}

void DrawWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_R)
        resetDrawing();
}

void DrawWidget::updateDisplay()
{
    setPixmap(QPixmap::fromImage(inputImageQImage));
}

void DrawWidget::resetDrawing()
{
    inputImage = originalImage.clone();
    cv::resize(inputImage, inputImage, cv::Size(1920, 1080));
    mask.fill(Qt::transparent);
    updateDisplay();
    update();
}

ScrollableMessageBox::ScrollableMessageBox(const QString &message, QWidget *parent)
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
}

void FourierCropperSimulatorUi::initializeSimulator()
{

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

void FourierCropperSimulatorUi::on_pencilSizeSlider_valueChanged(int value)
{
    mUi->drawWidget->setPencilSize(value);
    mUi->pencilSizeValue->setText(QString::number(value));
}

void FourierCropperSimulatorUi::on_saveMaskButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Guardar Máscara"), "", tr("PNG Files (*.png);;All Files (*)"));

    if (filePath.isEmpty() || QFileInfo(filePath).suffix().isEmpty())
        filePath += ".png";

    QImage mask = mUi->drawWidget->getMask();
    mask = mask.convertToFormat(QImage::Format_Grayscale8);

    for (int y = 0; y < mask.height(); ++y)
    {
        for (int x = 0; x < mask.width(); ++x)
        {
            QRgb pixel = mask.pixel(x, y);
            if (qRed(pixel) > 0)
                mask.setPixel(x, y, qRgb(0, 0, 0)); // Blanco
            else
                mask.setPixel(x, y, qRgb(255, 255, 255)); // Negro
        }
    }

    cv::Mat maskMat(mask.height(), mask.width(), CV_8UC1, (void*)mask.bits(), mask.bytesPerLine());
    cv::resize(maskMat, maskMat, cv::Size(mInputImage.cols, mInputImage.rows), 0, 0, cv::INTER_NEAREST);

    if (!cv::imwrite(filePath.toStdString(), maskMat))
        QMessageBox::warning(this, tr("Guardar Máscara"), tr("No se pudo guardar la máscara en %1").arg(filePath));
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
            qWarning() << QObject::tr("%1 - %2 - Error al abrir la hoja de ayuda.").arg(this->metaObject()->className()).arg(__func__ );

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
    QString filePath = QFileDialog::getOpenFileName(this, tr("Abrir Imagen"), "", tr("Imágenes (*.png *.jpg *.bmp *.jpeg *.tif *.tiff)"));

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
        }
        else
            QMessageBox::warning(this, tr("Error"), tr("La imagen está vacía"));
    }
    else
        QMessageBox::warning(this, tr("Error"), tr("No se pudo abrir la imagen: %1").arg(filePath));
}


void FourierCropperSimulatorUi::on_radioButtonDrawMode_toggled(bool checked)
{
    mUi->radioButtonDrawMode->setText(checked ? "Crop Mode ON" : "Crop Mode OFF");
}

