QT += core gui widgets

TEMPLATE = app

CONFIG += c++20

TARGET = FourierCropper

SOURCES += \
    ./widgets/beforeafterfilteringwidget.cpp \
    ./widgets/drawwidget.cpp \
    ./libs/fourier.cpp \
    ./app/fouriercroppersimulator.cpp \
    ./app/fouriercroppersimulatorui.cpp \
    main.cpp \
    ./widgets/scrollablemessagebox.cpp

HEADERS += \
    ./widgets/beforeafterfilteringwidget.h \
    ./widgets/drawwidget.h \
    ./libs/fourier.h \
    ./app/fouriercroppersimulator.h \
    ./app/fouriercroppersimulatorui.h \
    ./widgets/scrollablemessagebox.h

FORMS += \
    ./widgets/beforeafterfilteringwidget.ui \
    ./app/fouriercroppersimulatorui.ui \

RESOURCES += \
    ./assets/configsFC.qrc

# Enable pkg-config
CONFIG += link_pkgconfig

# Specify the OpenCV package
PKGCONFIG += opencv4

INCLUDEPATH += /usr/local/include/opencv4

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

