target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codeeditortab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codeeditortab.h
)

add_subdirectory(QCodeEditor)
target_link_libraries(${PROJECT_NAME} PRIVATE QCodeEditor)