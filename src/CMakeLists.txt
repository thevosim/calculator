set(LIB_SRC
    lexer.cpp
    parser.cpp
    eval.cpp
)

add_library(${LIBRARY} STATIC ${LIB_SRC})
target_include_directories(${LIBRARY} PUBLIC ${INCLUDE})

# приложение — отдельно, добавляем linenoise.c как исходник C
add_executable(calc app/main.cpp vendor/linenoise/linenoise.c)
target_include_directories(calc PRIVATE ${INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/linenoise)
target_link_libraries(calc PRIVATE ${LIBRARY})
