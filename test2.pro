QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    barrel.cpp \
    boss.cpp \
    bossfight.cpp \
    ladder.cpp \
    main.cpp \
    mainwindow.cpp \
    platform.cpp \
    player.cpp \
    artifact.cpp \
    retrybutton.cpp \
    startmenu.cpp

HEADERS += \
    barrel.h \
    boss.h \
    bossfight.h \
    ladder.h \
    mainwindow.h \
    platform.h \
    player.h \
    artifact.h \
    retrybutton.h \
    startmenu.h

FORMS += \
    mainwindow.ui \
    startmenu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
