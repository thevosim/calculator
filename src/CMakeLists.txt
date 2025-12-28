set(LIB_SRC
    lexer.cpp
    parser.cpp
    eval.cpp
)

add_library(${LIBRARY} STATIC ${LIB_SRC})
target_include_directories(${LIBRARY} PUBLIC ${INCLUDE})

add_subdirectory(vendor/replxx)

add_executable(calc app/main.cpp)
target_link_libraries(calc PRIVATE ${LIBRARY} replxx)