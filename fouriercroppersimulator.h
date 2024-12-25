#pragma once

#include "fouriercroppersimulatorui.h"

#include <QObject>

class FourierCropperSimulator : public QObject
{
    Q_OBJECT

    public:
        explicit FourierCropperSimulator(QObject *parent = nullptr);
        ~FourierCropperSimulator();

        int init();
        int closeSim();

    private:
        FourierCropperSimulatorUi* mSimulatorUi = nullptr;
};

