add_subdirectory(src)

set(AST_SOURCES
    src/driver.cpp
)

add_library(ast ${AST_SOURCES})

target_include_directories(ast
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
