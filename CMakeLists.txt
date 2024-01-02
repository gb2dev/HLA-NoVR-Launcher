cmake_minimum_required(VERSION 3.19)

project(HLA-NoVR-Launcher VERSION 2.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Network Multimedia Concurrent QuickControls2)

qt_standard_project_setup()

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")

qt_add_executable(HLA-NoVR-Launcher
    ${app_icon_resource_windows}
    main.cpp
    launcher.h
    launcher.cpp
    gamemenu.h
    gamemenu.cpp
    networkhandler.h
)

qt_add_qml_module(HLA-NoVR-Launcher
    URI HLA-NoVR-Launcher
    VERSION 1.0
    QML_FILES Launcher.qml GameMenu.qml Updating.qml
)

# Avoid collision between executable name and QML module directory (QTBUG-98568).
if(WIN32)
    set_target_properties(HLA-NoVR-Launcher PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS")
else()
    target_link_libraries(HLA-NoVR-Launcher
    PRIVATE X11)
    set_target_properties(HLA-NoVR-Launcher PROPERTIES
        OUTPUT_NAME HLA-NoVR-Launcher-Linux
    )
    add_custom_target(PatchedInstall
        COMMAND "${CMAKE_COMMAND}" --build . --target install
        COMMAND sh -c "patchelf --set-rpath '\\$$ORIGIN' ${CMAKE_INSTALL_PREFIX}/lib/*"
    )
endif()

set_target_properties(HLA-NoVR-Launcher PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(HLA-NoVR-Launcher
    PRIVATE Qt6::Quick Qt6::Network Qt6::Multimedia Qt6::Concurrent Qt6::QuickControls2
)

# Install the executable into "${CMAKE_INSTALL_PREFIX}/bin".
install(TARGETS HLA-NoVR-Launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Generate the deployment script for the target MyApp.
qt_generate_deploy_qml_app_script(
    TARGET HLA-NoVR-Launcher
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

# Call the deployment script on "cmake --install".
install(SCRIPT ${deploy_script})

target_compile_definitions(HLA-NoVR-Launcher PRIVATE VERSION="${PROJECT_VERSION}")
