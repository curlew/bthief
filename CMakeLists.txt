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

FetchContent_MakeAvailable(wil json)

add_executable(${PROJECT_NAME} src/main.cxx)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

target_link_libraries(${PROJECT_NAME} WIL nlohmann_json::nlohmann_json)