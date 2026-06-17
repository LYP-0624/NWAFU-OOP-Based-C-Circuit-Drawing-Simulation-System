cmake_minimum_required(VERSION 3.16)

project(CircuitSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 设置头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../Serialize
    ${PROJECT_SOURCE_DIR}/../third_party
)

# 后端核心源文件
set(BACKEND_SOURCES
    ../src/Component.cpp
    ../src/Resistor.cpp
    ../src/PowerSource.cpp
    ../src/Bulb.cpp
    ../src/Ammeter.cpp
    ../src/Voltmeter.cpp
    ../src/Switch.cpp
    ../src/Capacitor.cpp
    ../src/Inductor.cpp
    ../src/Ground.cpp
    ../src/Port.cpp
    ../src/Node.cpp
    ../src/Branch.cpp
    ../src/Circuit.cpp
    ../src/MNASolver.cpp
    ../src/LinearSolver.cpp
    ../src/CircuitValidator.cpp
    ../src/ComponentFactory.cpp
    ../src/SimulationLogger.cpp
)

# 序列化模块源文件
set(SERIALIZE_SOURCES
    ../Serialize/CircuitJsonSerializer.cpp
    ../Serialize/CircuitFileValidator.cpp
)

# UI层源文件
set(UI_SOURCES
    main.cpp
    mainwindow.cpp
    graphcomponent.cpp
)

# 头文件（用于MOC）
set(HEADERS
    mainwindow.h
    graphcomponent.h
    resistoritem.h
    capacitoritem.h
    inductoritem.h
    switchitem.h
    grounditem.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CircuitSim
        MANUAL_FINALIZATION
        ${UI_SOURCES}
        ${HEADERS}
        ${BACKEND_SOURCES}
        ${SERIALIZE_SOURCES}
        mainwindow.ui
    )
else()
    if(ANDROID)
        add_library(CircuitSim SHARED
            ${UI_SOURCES}
            ${HEADERS}
            ${BACKEND_SOURCES}
            ${SERIALIZE_SOURCES}
            mainwindow.ui
        )
    else()
        add_executable(CircuitSim
            ${UI_SOURCES}
            ${HEADERS}
            ${BACKEND_SOURCES}
            ${SERIALIZE_SOURCES}
            mainwindow.ui
        )
    endif()
endif()

target_link_libraries(CircuitSim PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(CircuitSim PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CircuitSim
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CircuitSim)
endif()