#include "./app/fouriercroppersimulator.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("FourierCropper");
    a.setApplicationVersion("0.0");
    a.setStyle("fusion");

    FourierCropperSimulator sim;
    sim.init();

    return a.exec();
}
