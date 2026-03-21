target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/hexviewtab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hexviewtab.h
)

add_subdirectory(QHexView)
target_link_libraries(${PROJECT_NAME} PRIVATE QHexView)