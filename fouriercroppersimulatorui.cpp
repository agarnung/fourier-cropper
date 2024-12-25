#include "fouriercroppersimulatorui.h"
#include "ui_fouriercroppersimulatorui.h"

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

}
