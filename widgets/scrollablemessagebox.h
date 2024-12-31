#pragma once

#include <QDialog>
#include <QVBoxLayout>

class ScrollableMessageBox : public QDialog
{
    public:
        explicit ScrollableMessageBox(const QString& message, QWidget* parent = nullptr);

    private:
        QVBoxLayout* layout = nullptr;
};

