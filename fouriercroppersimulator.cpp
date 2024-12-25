#include "fouriercroppersimulator.h"

FourierCropperSimulator::FourierCropperSimulator(QObject *parent)
    : QObject{parent}
{
}

FourierCropperSimulator::~FourierCropperSimulator()
{
    if (mSimulatorUi)
    {
        delete mSimulatorUi;
        mSimulatorUi = nullptr;
    }
}

int FourierCropperSimulator::init()
{
    mSimulatorUi = new FourierCropperSimulatorUi();
    mSimulatorUi->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mSimulatorUi->initializeSimulator();
    mSimulatorUi->show();
    mSimulatorUi->adjustSize();

    qDebug() << QObject::tr("%1 - %2 - Simulator created").arg(this->metaObject()->className()).arg(__func__);
}

int FourierCropperSimulator::closeSim()
{
    if (mSimulatorUi)
    {
        delete mSimulatorUi;
        mSimulatorUi = nullptr;
    }

    return 0;
}
