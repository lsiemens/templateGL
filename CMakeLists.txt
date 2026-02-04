cmake_minimum_required(VERSION 3.19)
project(__PROJECT__
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# setup OpenGL GLEW and GLFW
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG)

option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(ERROR_ON_WARNING "Treat warnings to errors" OFF)
option(BUILD_DOCS "Build documentation" ON)

add_library(__PROJECT___warnings INTERFACE)
if(ENABLE_WARNINGS)
    target_compile_options(__PROJECT___warnings INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>: -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wundef>
        $<$<AND:$<BOOL:${STRICT_WARNINGS}>,$<CXX_COMPILER_ID:GNU,Clang>>: -Wconversion>
        $<$<CXX_COMPILER_ID:Clang>: -Wdocumentation>

        $<$<AND:$<BOOL:${ERROR_ON_WARNING}>,$<CXX_COMPILER_ID:GNU,Clang>>: -Werror>)
endif()

if(BUILD_DOCS)
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        set(DOXYGEN_PROJECT_NAME "${PROJECT_NAME}")
        set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
        set(DOXYGEN_PROJECT_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINDARY_DIR}/doxygen")
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")

        set(DOXYGEN_PROJECT_GENERATE_HTML YES)
        set(DOXYGEN_RECURSIVE YES)
        doxygen_add_docs(__PROJECT___docs ALL
            "${PROJECT_SOURCE_DIR}/README.md"
            "${PROJECT_SOURCE_DIR}/src"
            COMMENT "Generating Doxygen documentation")
    else()
        message(WARNING "Doxygen not found")
    endif()
endif()

add_subdirectory(src)
add_subdirectory(assets)
