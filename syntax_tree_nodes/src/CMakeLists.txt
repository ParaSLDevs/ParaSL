include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_executable(driver driver.cpp ../include/declarations.h ../include/expressions.h ../include/other_nodes.h
        ../include/statements.h ../include/syntax_node.h ../include/types.h)

