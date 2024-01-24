# cmake_minimum_required(VERSION 3.20.0)
# project(orcalang)

# find_package(LLVM REQUIRED CONFIG)

# message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(${LLVM_INCLUDE_DIRS})
# separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
# add_definitions(${LLVM_DEFINITIONS_LIST})

# # Build source
# add_executable(orca src/main.cpp)

# # Find the libraries that correspond to the LLVM components
# llvm_map_components_to_libnames(llvm_libs support core irreader)

# # Link against LLVM libraries
# target_link_libraries(orca ${llvm_libs})

# ---------------------------------------------------------------

# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

project(orcalang)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
set(ANTLR4_TAG 4.13.0)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.13.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/vendor/antlr/antlr-4.13.0-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(OrcaLexer OrcaLexer.g4 LEXER
             PACKAGE orcagrammar)
antlr_target(OrcaParser OrcaParser.g4 PARSER VISITOR
             PACKAGE orcagrammar
             DEPENDS_ANTLR OrcaLexer
             COMPILE_FLAGS -lib ${ANTLR_OrcaLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_OrcaLexer_OUTPUT_DIR})
include_directories(${ANTLR_OrcaParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_executable(orca 
              src/main.cpp
              src/OrcaLexerErrorListener.cpp
              src/OrcaParserErrorListener.cpp
              src/OrcaError.cpp
              src/TypeChecker.cpp
              src/OrcaContext.cpp
              ${ANTLR_OrcaLexer_CXX_OUTPUTS}
              ${ANTLR_OrcaParser_CXX_OUTPUTS}
)
target_link_libraries(orca antlr4_static)

enable_testing()

add_executable(orca_test test/orca_test.cpp
               ${ANTLR_OrcaLexer_CXX_OUTPUTS}
               ${ANTLR_OrcaParser_CXX_OUTPUTS})

target_link_libraries(orca_test GTest::gtest_main)
target_link_libraries(orca_test antlr4_static)

include(GoogleTest)
gtest_discover_tests(orca_test)