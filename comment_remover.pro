TARGET = comment_remover

# Qt
CONFIG += qt
QT += core

# Конфигурация
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++11 -fPIC

# Дата сборки
win32:COMPILE_DATE = $$system(date /T)
unix: COMPILE_DATE = $$system(date +%d.%m.%Y)
VERSION = $$sprintf("%1.%2.%3", $$section(COMPILE_DATE, ., 0, 0), $$section(COMPILE_DATE, ., 1, 1), $$section(COMPILE_DATE, ., 2, 2))
DEFINES += PSLVERSION=\\\"$$VERSION\\\"

# Каталог сборки
CONFIG(release, debug|release) {
    DESTDIR = release
    MOC_DIR = release/moc
    OBJECTS_DIR = release/obj
    DEFINES += QT_NO_DEBUG_OUTPUT
} else {
    DESTDIR = debug
    MOC_DIR = debug/moc
    OBJECTS_DIR = debug/obj
    DEFINES += PSL_DEBUG
}

INCLUDEPATH += $$PWD/clang-c
LIBS += -L/usr/lib/llvm-14/lib -lclang
LIBS += -lpthread

SOURCES += \
    functions.cpp \
    main.cpp \
    traverser.cpp

HEADERS += \
    functions.h \
    traverser.h
