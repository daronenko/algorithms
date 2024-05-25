add_library(${PROJECT_NAME}_objs OBJECT
        graph/base.hpp
        graph/list_graph.hpp
        graph/list_graph.cpp
        graph/matrix_graph.hpp
        graph/matrix_graph.cpp
        graph/set_graph.hpp
        graph/set_graph.cpp
        graph/arc_graph.hpp
        graph/arc_graph.cpp
)
target_include_directories(${PROJECT_NAME}_objs PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)