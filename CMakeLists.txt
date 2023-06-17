cmake_minimum_required(VERSION 3.5)

project(Messenger VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        forms/authenticationform.cpp
        forms/authenticationform.h
        forms/authenticationform.ui

        forms/chatform.cpp
        forms/chatform.h
        forms/chatform.ui
        forms/form.cpp
        forms/form.h
        forms/form.ui

        client.h
        client.cpp
        serverinterface.cpp
        serverinterface.h
        loaderthread.cpp
        loaderthread.h

        chat/contact.h
        chat/contact.cpp
        chat/user.cpp
        chat/user.h
        chat/group.h
        chat/group.cpp
        chat/channel.h
        chat/channel.cpp
)

set(INCLUDE_DIRS windows chat)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Messenger
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Messenger APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Messenger SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Messenger
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Messenger PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(Messenger PUBLIC ${INCLUDE_DIRS})
set_target_properties(Messenger PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Messenger
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Messenger)
endif()
