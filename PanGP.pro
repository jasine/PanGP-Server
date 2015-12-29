#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T17:24:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PanGP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    panwindow.cpp \
    samplethread.cpp \
    panprofiledata.cpp \
    panwindowdialogsetcolor.cpp \
    panwindowdialogsetaxis.cpp \
    aboutwindow.cpp \
    drawnewgenecurve.cpp \
    newgenewindow.cpp \
    newgenewindowdialogsetaxis.cpp \
    newgenewindowdialogsetcolor.cpp \
    curveparameterfornewgenedata.cpp \
    curveparameterforpancoredata.cpp \
    drawpancorecurve.cpp \
    datafittingthread.cpp \
    panfittingwindow.cpp \
    fittingresultdata.cpp \
    newfittingwindow.cpp \
    sampleparameterdata.cpp

HEADERS  += mainwindow.h \
    panwindow.h \
    samplethread.h \
    panprofiledata.h \
    panwindowdialogsetcolor.h \
    panwindowdialogsetaxis.h \
    aboutwindow.h \
    drawnewgenecurve.h \
    newgenewindow.h \
    newgenewindowdialogsetaxis.h \
    newgenewindowdialogsetcolor.h \
    curveparameterfornewgenedata.h \
    curveparameterforpancoredata.h \
    drawpancorecurve.h \
    datafittingthread.h \
    panfittingwindow.h \
    fittingresultdata.h \
    newfittingwindow.h \
    sampleparameterdata.h

FORMS    += mainwindow.ui \
    panwindow.ui \
    panwindowdialogsetcolor.ui \
    panwindowdialogsetaxis.ui \
    aboutwindow.ui \
    newgenewindow.ui \
    newgenewindowdialogsetaxis.ui \
    newgenewindowdialogsetcolor.ui \
    panfittingwindow.ui \
    newfittingwindow.ui

RESOURCES += \
    img/img.qrc

RC_FILE += \
    PanGP.rc

OTHER_FILES += \
    PanGP.rc
