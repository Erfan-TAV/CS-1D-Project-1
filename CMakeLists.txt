cmake_minimum_required(VERSION 3.16)

project(collegeItemDetails VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --- THE MAGIC LINE FOR YOUR FORMS FOLDER ---
# This tells Qt to look inside "forms" for the .ui files
# even if the .cpp files are somewhere else.
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Cleaned up list: No duplicates, organized by location
set(PROJECT_SOURCES
        # Root files
        source/main.cpp
        source/collegescreen.cpp
        headers/collegescreen.h
        source/itemlistwidget.cpp
        headers/itemlistwidget.h
        headers/tempInfo.h

        # Form files (explicit paths)
        forms/collegescreen.ui
        forms/itemlistwidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(collegeItemDetails
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
else()
    add_executable(collegeItemDetails ${PROJECT_SOURCES})
endif()

target_link_libraries(collegeItemDetails PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Keep your bundle/properties settings as they were...
set_target_properties(collegeItemDetails PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(collegeItemDetails)
endif()