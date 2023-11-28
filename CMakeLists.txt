cmake_minimum_required(VERSION 3.16)

project(HLA-NoVR-Launcher VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Network Multimedia Concurrent)

qt_standard_project_setup()

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")

qt_add_executable(HLA-NoVR-Launcher
    ${app_icon_resource_windows}
    main.cpp
    launcher.h
    launcher.cpp
    gamemenu.h
    gamemenu.cpp
)

qt_add_qml_module(HLA-NoVR-Launcher
    URI HLA-NoVR-Launcher
    VERSION 1.0
    QML_FILES Launcher.qml GameMenu.qml Updating.qml
)

set_target_properties(HLA-NoVR-Launcher PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(HLA-NoVR-Launcher
    PRIVATE Qt6::Quick Qt6::Network Qt6::Multimedia Qt6::Concurrent
)

install(TARGETS HLA-NoVR-Launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_compile_definitions(HLA-NoVR-Launcher PRIVATE VERSION="${PROJECT_VERSION}")
