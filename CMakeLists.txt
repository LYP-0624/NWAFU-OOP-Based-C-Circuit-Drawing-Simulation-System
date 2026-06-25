cmake_minimum_required(VERSION 3.16)
project(CircuitSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/nlohmann
    ${CMAKE_SOURCE_DIR}/Serialize
    ${CMAKE_SOURCE_DIR}/UI
)

set(SOURCES
    UI/main.cpp
    UI/mainwindow.cpp
    UI/graphcomponent.cpp
    UI/circuitscene.cpp
    UI/wireitem.cpp
    src/Component.cpp
    src/Resistor.cpp
    src/PowerSource.cpp
    src/Bulb.cpp
    src/Ammeter.cpp
    src/Voltmeter.cpp
    src/Switch.cpp
    src/Capacitor.cpp
    src/Inductor.cpp
    src/Circuit.cpp
    src/MNASolver.cpp
    src/LinearSolver.cpp
    src/Node.cpp
    src/Port.cpp
    src/Branch.cpp
    src/ComponentFactory.cpp
    src/CircuitValidator.cpp
    Serialize/SimulationLogger.cpp
    Serialize/CircuitJsonSerializer.cpp
    Serialize/CircuitFileValidator.cpp
)

set(HEADERS
    UI/mainwindow.h
    UI/graphcomponent.h
    UI/circuitscene.h
    UI/wireitem.h
    UI/componentitems.h
    include/Component.h
    include/Resistor.h
    include/PowerSource.h
    include/Bulb.h
    include/Ammeter.h
    include/Voltmeter.h
    include/Switch.h
    include/Capacitor.h
    include/Inductor.h
    include/Circuit.h
    include/MNASolver.h
    include/LinearSolver.h
    include/Node.h
    include/Port.h
    include/Branch.h
    include/ComponentFactory.h
    include/CircuitValidator.h
    include/CircuitException.h
    include/SimulationResult.h
    Serialize/SimulationLogger.h
    Serialize/CircuitJsonSerializer.h
    Serialize/CircuitFileValidator.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
)

# On Windows, deploy Qt runtime beside the executable so it can be launched directly.
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        HINTS
            "${Qt6_DIR}/../../../bin"
            "E:/qt/6.11.1/mingw_64/bin"
        REQUIRED
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --compiler-runtime
            --no-translations
            --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt runtime"
    )
endif()
