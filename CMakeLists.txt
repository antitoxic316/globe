cmake_minimum_required(VERSION 3.19)
project(globe LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGL Gui OpenGLWidgets)

qt_standard_project_setup()

qt_add_executable(globe
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    canvas.cpp
    canvas.h
    session.h session.cpp
    addmarkerdialog.h addmarkerdialog.cpp addmarkerdialog.ui

)

target_link_libraries(globe
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::Gui
        Qt6::OpenGLWidgets
)

include(GNUInstallDirs)

install(TARGETS globe
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET globe
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
