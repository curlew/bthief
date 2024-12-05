find_program(CLANG_FORMAT NAMES clang-format)

if(CLANG_FORMAT)
    file(GLOB_RECURSE CLANG_FORMAT_FILES src/*.cxx src/*.hxx)

    add_custom_target(format COMMAND clang-format -i -style=file ${CLANG_FORMAT_FILES})
endif()
