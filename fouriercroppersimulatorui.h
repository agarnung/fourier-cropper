#pragma once

#include <QWidget>

namespace Ui {
class FourierCropperSimulatorUi;
}

class FourierCropperSimulatorUi : public QWidget
{
    Q_OBJECT

    public:
        explicit FourierCropperSimulatorUi(QWidget *parent = nullptr);
        ~FourierCropperSimulatorUi();

        void initializeSimulator();

    private:
        Ui::FourierCropperSimulatorUi* mUi = nullptr;
};
