cmake_minimum_required(VERSION 3.19)
project(CollegeTour LANGUAGES CXX)

# --- 1. Qt Initialization ---
find_package(Qt6 6.10.2 REQUIRED COMPONENTS Core Widgets Sql)

# Enable automatic handling of Qt Meta-Object System and UI files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# This tells CMake where to look for .ui files when you #include "ui_xxxx.h"
# It allows you to keep .ui files in /forms while .cpp files are in /sources
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms")

qt_standard_project_setup()

# --- 2. Scalable File Discovery ---
# This automatically finds all relevant files so you don't have to list them manually
file(GLOB_RECURSE SOURCES "sources/*.cpp")
file(GLOB_RECURSE HEADERS "headers/*.h")
file(GLOB_RECURSE FORMS   "forms/*.ui")

# --- 3. Executable Definition ---
qt_add_executable(CollegeTour
        WIN32 MACOSX_BUNDLE
        ${SOURCES}
        ${HEADERS}
        ${FORMS}
        # Add SQLite C files explicitly if they aren't caught by the glob
        "SQLite stuff/sqlite3.c"
        "SQLite stuff/shell.c"
        headers/campusStructs.h
)

# --- 4. Include Directories ---
# Ensures the compiler can find your headers and SQLite files
target_include_directories(CollegeTour PRIVATE
        "headers"
        "SQLite stuff"
)

# --- 5. Linking & Deployment ---
target_link_libraries(CollegeTour
        PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

include(GNUInstallDirs)
install(TARGETS CollegeTour
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
        TARGET CollegeTour
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
