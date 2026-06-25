QT += core gui widgets

CONFIG += c++17 console debug

TARGET = CircuitSim
TEMPLATE = app

SOURCES += \
    UI/main.cpp \
    UI/mainwindow.cpp \
    UI/graphcomponent.cpp \
    UI/circuitscene.cpp \
    UI/wireitem.cpp \
    src/Component.cpp \
    src/Resistor.cpp \
    src/PowerSource.cpp \
    src/Bulb.cpp \
    src/Ammeter.cpp \
    src/Voltmeter.cpp \
    src/Switch.cpp \
    src/Capacitor.cpp \
    src/Inductor.cpp \
    src/Circuit.cpp \
    src/MNASolver.cpp \
    src/LinearSolver.cpp \
    src/Node.cpp \
    src/Port.cpp \
    src/Branch.cpp \
    src/ComponentFactory.cpp \
    src/CircuitValidator.cpp \
    src/SimulationLogger.cpp \
    Serialize/CircuitJsonSerializer.cpp \
    Serialize/CircuitFileValidator.cpp

HEADERS += \
    UI/mainwindow.h \
    UI/graphcomponent.h \
    UI/circuitscene.h \
    UI/wireitem.h \
    UI/componentitems.h \
    include/Component.h \
    include/Resistor.h \
    include/PowerSource.h \
    include/Bulb.h \
    include/Ammeter.h \
    include/Voltmeter.h \
    include/Switch.h \
    include/Capacitor.h \
    include/Inductor.h \
    include/Circuit.h \
    include/MNASolver.h \
    include/LinearSolver.h \
    include/Node.h \
    include/Port.h \
    include/Branch.h \
    include/ComponentFactory.h \
    include/CircuitValidator.h \
    include/CircuitException.h \
    include/SimulationLogger.h \
    include/SimulationResult.h \
    Serialize/CircuitJsonSerializer.h \
    Serialize/CircuitFileValidator.h

FORMS += \
    UI/mainwindow.ui

INCLUDEPATH += \
    include \
    lib/nlohmann \
    Serialize \
    UI

DEPENDPATH += \
    include \
    lib/nlohmann \
    Serialize \
    UI

win32 {
    QTDEPLOY_BIN = $$replace(QMAKE_QMAKE, /qmake.exe$, /windeployqt.exe)
    exists($$QTDEPLOY_BIN) {
        QMAKE_POST_LINK += $$quote($$QTDEPLOY_BIN --compiler-runtime --no-translations --dir $$OUT_PWD $$OUT_PWD/$$TARGET.exe)
    }
}
