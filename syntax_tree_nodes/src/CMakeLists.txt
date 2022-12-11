include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_executable(driver driver.cpp ../include/expressions.h
        ../include/statements.h ../include/syntax_node.h ../include/types.h)

