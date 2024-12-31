#include "scrollablemessagebox.h"

#include <QTextEdit>

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
