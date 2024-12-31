#pragma once

#include <QWidget>

namespace Ui {
class BeforeAfterFilteringWidget;
}

class BeforeAfterFilteringWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit BeforeAfterFilteringWidget(QWidget* parent = nullptr);
        ~BeforeAfterFilteringWidget();

        void setBeforeImage(const QImage& img);
        void setAfterImage(const QImage& img);

    private:
        Ui::BeforeAfterFilteringWidget* mUi = nullptr;
        QImage mFiltered;

    private slots:
        void on_fullscreenButton_released();
        void on_savePushButton_released();
};

