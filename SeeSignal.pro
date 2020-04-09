QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fileformatterdialog.cpp \
    fileparser.cpp \
    main.cpp \
    mainwindow.cpp \
    operationsmanager.cpp \
    plottermanager.cpp \
    qcustomplot.cpp \
    signaldb.cpp \
    signalitem.cpp

HEADERS += \
    fileformatterdialog.h \
    fileparser.h \
    mainwindow.h \
    operationsmanager.h \
    plottermanager.h \
    qcustomplot.h \
    signaldb.h \
    signalitem.h

FORMS += \
    fileformatterdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    SeeSigResources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qtcsv/release/ -lqtcsv1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qtcsv/debug/ -lqtcsv1
else:unix: LIBS += -L$$PWD/qtcsv/ -lqtcsv1

INCLUDEPATH += $$PWD/qtcsv/debug
DEPENDPATH += $$PWD/qtcsv/debug

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qtcsv/release/ -lqtcsv1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qtcsv/debug/ -lqtcsv1
else:unix: LIBS += -L$$PWD/qtcsv/ -lqtcsv1

INCLUDEPATH += $$PWD/qtcsv/release
DEPENDPATH += $$PWD/qtcsv/release
