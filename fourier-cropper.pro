QT += core gui widgets

TEMPLATE = app

CONFIG += c++20

TARGET = FourierCropper

SOURCES += \
    fourier.cpp \
    fouriercroppersimulator.cpp \
    fouriercroppersimulatorui.cpp \
    main.cpp \

HEADERS += \
    fourier.h \
    fouriercroppersimulator.h \
    fouriercroppersimulatorui.h \

FORMS += \
    fouriercroppersimulatorui.ui \

RESOURCES += \
    configsFC.qrc

# Enable pkg-config
CONFIG += link_pkgconfig

# Specify the OpenCV package
PKGCONFIG += opencv4

INCLUDEPATH += /usr/local/include/opencv4

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

