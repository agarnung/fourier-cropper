#include "beforeafterfilteringwidget.h"
#include "ui_beforeafterfilteringwidget.h"

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

BeforeAfterFilteringWidget::BeforeAfterFilteringWidget(QWidget *parent)
    : QWidget(parent)
    , mUi(new Ui::BeforeAfterFilteringWidget)
{
    mUi->setupUi(this);

    mUi->beforeLabel->setScaledContents(true);
    mUi->afterLabel->setScaledContents(true);
}

BeforeAfterFilteringWidget::~BeforeAfterFilteringWidget()
{
    if (mUi)
    {
        delete mUi;
        mUi = nullptr;
    }
}

void BeforeAfterFilteringWidget::setBeforeImage(const QImage& img)
{
    mUi->beforeLabel->clear();
    mUi->beforeLabel->setPixmap(QPixmap::fromImage(img));
    mUi->beforeLabel->update();
}

void BeforeAfterFilteringWidget::setAfterImage(const QImage& img)
{
    mUi->afterLabel->clear();
    mUi->afterLabel->setPixmap(QPixmap::fromImage(img));
    mUi->afterLabel->update();
}

void BeforeAfterFilteringWidget::on_fullscreenButton_released()
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

void BeforeAfterFilteringWidget::on_savePushButton_released()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Seleccionar Carpeta"), QDir::homePath());

    if (directory.isEmpty())
        return;

    if (!directory.isEmpty())
    {
        QDir dir = directory;
        if (!dir.isEmpty() && !mFiltered.isNull())
        {
            mFiltered.save(QDir(dir).filePath("filtered.png"), 0, 100);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Filtered image could not be saved");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Image saved");
    msgBox.setText("Filtered image saved in " + directory);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


void BeforeAfterFilteringWidget::on_helpPushButton_released()
{

}

