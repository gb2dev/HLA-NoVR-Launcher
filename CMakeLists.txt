cmake_minimum_required(VERSION 3.16)

project(HLA-NoVR-Launcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup()

qt_add_executable(HLA-NoVR-Launcher
    main.cpp
    launcher.h
    launcher.cpp
    downloadmanager.h
)

qt_add_qml_module(HLA-NoVR-Launcher
    URI HLA-NoVR-Launcher
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(HLA-NoVR-Launcher PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(HLA-NoVR-Launcher
    PRIVATE Qt6::Quick Qt6::Network
)

install(TARGETS HLA-NoVR-Launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
