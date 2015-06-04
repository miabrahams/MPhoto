
QT       += core gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

DEFINES += VERSION=1.12
VERSION = 1.12

TARGET = MihPhoto
DESTDIR = ../bin

SOURCES += \
    TouchUI.cpp \
    TouchMouseControl.cpp \
    ScreenViewer.cpp \
    ScreenDirectory.cpp \
    ScreenBase.cpp \
    MainWindow.cpp \
    main.cpp \
    ImageWithInfo.cpp \
    ImageLoadThread.cpp \
    ImageLoadQueue.cpp \
    ImageArea.cpp \
    ConfigDialog.cpp \
    Config.cpp \
    Trashcan.cpp \
    ScreenSettings.cpp

HEADERS  += \
    TouchUI.h \
    TouchMouseControl.h \
    ScreenViewer.h \
    ScreenDirectory.h \
    ScreenBase.h \
    MainWindow.h \
    ImageWithInfo.h \
    ImageLoadThread.h \
    ImageLoadQueue.h \
    ImageLoadItem.h \
    ImageArea.h \
    ConfigDialog.h \
    Config.h \
    Trashcan.h \
    ScreenSettings.h

OTHER_FILES += \
    MihPhoto.rc

FORMS += \
    ConfigDialog.ui

RESOURCES += \
    mihphoto.qrc

RC_FILE = MihPhoto.rc

symbian {
	RESOURCES = mihphoto.qrc
	ICON = icon_symb.svg
	#QT += network
	#CONFIG += mobility
	#MOBILITY = systeminfo
	#symbian:TARGET.CAPABILITY = ReadDeviceData
}
#win32:CONFIG += console
#DEFINES += DATADIR=\\\"@DATADIR@\\\"
