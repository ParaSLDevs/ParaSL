add_subdirectory(src)

set(AST_SOURCES
    src/driver.cpp include/ast_builder.h include/expressions.h include/other_nodes.h
        include/statements.h include/syntax_node.h include/types.h src/ast_builder.cpp
        include/ast_printer.h src/ast_printer.cpp include/ast_visitor.h
)

add_library(ast ${AST_SOURCES})

target_include_directories(ast
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
