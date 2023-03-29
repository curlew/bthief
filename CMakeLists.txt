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
    GIT_TAG 5f4caba4e7a9017816e47becdd918fcc872039ba # v1.0.220914.1
)
set(WIL_BUILD_TESTS OFF)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d # v3.11.2
)

FetchContent_Declare(
    sqlite
    URL https://sqlite.org/2022/sqlite-amalgamation-3400100.zip # v3.40.1
    URL_HASH SHA3_256=2618a7f311ce3f8307c45035bce31805185d632241b2af6c250b4531f09edccb
)

message(STATUS "Getting dependencies...")
FetchContent_MakeAvailable(wil json sqlite)


add_executable(bthief
    src/main.cxx
    src/crypt.cxx
    src/nowide.cxx
    src/browsers/chrome/chrome.cxx
    src/browsers/firefox/firefox.cxx
    src/browsers/firefox/library.cxx
    ${sqlite_SOURCE_DIR}/sqlite3.c
)
set_property(TARGET bthief PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
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
