project(parasl)

set(PARSER_SOURCES
    parser.cpp
)

add_library(parser ${PARSER_SOURCES})

target_include_directories(parser
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/syntax_tree_nodes/include
)
target_link_libraries(parser ast)
