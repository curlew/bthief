cmake_minimum_required(VERSION 3.22)

# allow options to be set from variables
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# enable setting MSVC_RUNTIME_LIBRARY property
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(bthief LANGUAGES CXX C)

include(FetchContent)

FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG f0c6a81c0c9a4b23b6801f40554b8bec425a83b4 # v1.0.240803.1
)
set(WIL_BUILD_TESTS OFF)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03 # v3.11.3
)

FetchContent_Declare(
    sqlite
    URL https://sqlite.org/2024/sqlite-amalgamation-3470100.zip # 3.47.1
    URL_HASH SHA3_256=71c08f4c890000094a6781169927de8f87ad8569410d9a4310c07dbca1f14b37
    DOWNLOAD_EXTRACT_TIMESTAMP 1
)

message(STATUS "Getting dependencies...")
FetchContent_MakeAvailable(wil json sqlite)

set(BTHIEF_SOURCES
    main.cxx
    utils.cxx
    crypt.cxx
    browsers/browser.cxx
    browsers/chrome/chrome.cxx
    browsers/firefox/firefox.cxx
    browsers/firefox/library.cxx
)
list(TRANSFORM BTHIEF_SOURCES PREPEND "src/")

add_executable(bthief
    ${BTHIEF_SOURCES}
    ${sqlite_SOURCE_DIR}/sqlite3.c
)
set_property(TARGET bthief PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
target_compile_definitions(bthief PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_compile_features(bthief PRIVATE cxx_std_23)
target_compile_options(bthief PRIVATE /W4)
target_include_directories(bthief PRIVATE
    src
    ${sqlite_SOURCE_DIR}
)
target_link_libraries(bthief
    Bcrypt
    Crypt32
    WIL::WIL
    nlohmann_json::nlohmann_json
)
