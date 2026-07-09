cmake_minimum_required(VERSION 3.20)

project(signal_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile_commands.json for clangd, static analysis, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Options
# ------------------------------------------------------------

set(BUILD_MODE "example" CACHE STRING "Build mode: main or example")
set_property(CACHE BUILD_MODE PROPERTY STRINGS main example)

set(EXAMPLE_SOURCE "" CACHE STRING
    "Specific example source file under ./example, e.g. example_plotgnuplot/example_plotgnuplot.c"
)

# Select one example file:
# cmake -S . -B build -DEXAMPLE_SOURCE=fir_demo.c
#
# If empty, CMake builds all .c files inside ./example
set(EXAMPLE_SOURCE "" CACHE STRING "Specific example source file to build from ./example")

# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
)

# Optional: stricter debug build behavior
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
else()
    add_compile_options(-O2)
endif()

# ------------------------------------------------------------
# utils library
# ------------------------------------------------------------

file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/utils/src/*.c"
)

add_library(utils STATIC ${UTILS_SOURCES})

target_include_directories(utils
    PUBLIC
        "${CMAKE_SOURCE_DIR}/utils/include"
)

# ------------------------------------------------------------
# main project library
# ------------------------------------------------------------

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/lib_signal_process/src/*.c"
)

add_library(signal_lib STATIC ${LIB_SOURCES})

target_include_directories(signal_lib
    PUBLIC
        "${CMAKE_SOURCE_DIR}/lib_signal_process/include"
)

target_link_libraries(signal_lib
    PUBLIC
        utils
)

# ------------------------------------------------------------
# main project real-time deployment library
# ------------------------------------------------------------

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/real_time_process/src/*.c"
)

add_library(signal_real_time STATIC ${LIB_SOURCES})

target_include_directories(signal_real_time
    PUBLIC
        "${CMAKE_SOURCE_DIR}/real_time_process/include"
)

target_link_libraries(signal_real_time
    PUBLIC
        utils
)


# ------------------------------------------------------------
# Main executable
# ------------------------------------------------------------

if(BUILD_MODE STREQUAL "main")
    add_executable(main_app
        "${CMAKE_SOURCE_DIR}/main/src/main.c"
    )

    target_link_libraries(main_app
        PRIVATE
            signal_lib
            signal_real_time
            utils
    )

# ------------------------------------------------------------
# Examples
# ------------------------------------------------------------

elseif(BUILD_MODE STREQUAL "example")

    if(EXAMPLE_SOURCE)
        set(EXAMPLE_PATH "${CMAKE_SOURCE_DIR}/example/${EXAMPLE_SOURCE}")

        if(IS_DIRECTORY "${EXAMPLE_PATH}")
            message(FATAL_ERROR
                "EXAMPLE_SOURCE is a directory, not a .c file: ${EXAMPLE_PATH}"
            )
        endif()

        if(NOT EXISTS "${EXAMPLE_PATH}")
            message(FATAL_ERROR
                "Example source does not exist: ${EXAMPLE_PATH}"
            )
        endif()

        get_filename_component(EXAMPLE_EXT "${EXAMPLE_PATH}" EXT)

        if(NOT EXAMPLE_EXT STREQUAL ".c")
            message(FATAL_ERROR
                "Example source must be a .c file: ${EXAMPLE_PATH}"
            )
        endif()

        get_filename_component(EXAMPLE_NAME "${EXAMPLE_PATH}" NAME_WE)

        add_executable(${EXAMPLE_NAME}
            "${EXAMPLE_PATH}"
        )

        target_link_libraries(${EXAMPLE_NAME}
            PRIVATE
                signal_lib
                signal_real_time
                utils
        )

    else()
        file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/example/*.c"
        )

        foreach(EXAMPLE_FILE ${EXAMPLE_SOURCES})
            get_filename_component(EXAMPLE_NAME "${EXAMPLE_FILE}" NAME_WE)

            add_executable(${EXAMPLE_NAME}
                "${EXAMPLE_FILE}"
            )

            target_link_libraries(${EXAMPLE_NAME}
                PRIVATE
                    signal_lib
                    signal_real_time
                    utils
            )
        endforeach()
    endif()

else()
    message(FATAL_ERROR
        "Invalid BUILD_MODE='${BUILD_MODE}'. Use either main or example."
    )
endif()