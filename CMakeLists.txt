cmake_minimum_required(VERSION 3.19)
project(CollegeTour LANGUAGES CXX)

# --- 1. Qt Initialization ---
find_package(Qt6 6.10.2 REQUIRED COMPONENTS Core Widgets Sql)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms")

qt_standard_project_setup()

# --- 2. File Discovery ---
file(GLOB_RECURSE SOURCES "sources/*.cpp")
file(GLOB_RECURSE HEADERS "headers/*.h")
file(GLOB_RECURSE FORMS   "forms/*.ui")

# --- 3. Executable Definition ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    qt_add_executable(CollegeTour
            ${SOURCES}
            ${HEADERS}
            ${FORMS}
            "SQLite stuff/sqlite3.c"
            "SQLite stuff/shell.c"
            resources.qrc
    )
else()
    qt_add_executable(CollegeTour
            WIN32
            ${SOURCES}
            ${HEADERS}
            ${FORMS}
            "SQLite stuff/sqlite3.c"
            "SQLite stuff/shell.c"
            resources.qrc
    )
endif()

message(STATUS "FORMS = ${FORMS}")

add_subdirectory(res/QXlsx/QXlsx)

# --- 4. Include & Linking ---
target_include_directories(CollegeTour PRIVATE "headers" "SQLite stuff")
target_link_libraries(CollegeTour PRIVATE Qt::Core Qt::Widgets Qt::Sql QXlsx::QXlsx)

# --- 5. THE "FORCE COPY" FIX ---
set(SOURCE_DB "${CMAKE_CURRENT_SOURCE_DIR}/res/project1.db")

if(APPLE)
    set(DEST_DIR "$<TARGET_FILE_DIR:CollegeTour>/../Resources")
else()
    set(DEST_DIR "$<TARGET_FILE_DIR:CollegeTour>")
endif()

# We create a separate target that is ALWAYS considered out-of-date.
# Because it has the 'ALL' keyword, it runs every single time you build.
add_custom_target(AlwaysCopyDB ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE_DB}" "${DEST_DIR}/project1.db"
        COMMENT "Verifying/Restoring database file..."
        VERBATIM
)

# Make your app depend on this copy task so it happens before the app starts
add_dependencies(CollegeTour AlwaysCopyDB)

# --- 6. Installation ---
include(GNUInstallDirs)
install(TARGETS CollegeTour
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
