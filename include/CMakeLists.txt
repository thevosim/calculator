# include/CMakeLists.txt

file(GLOB HDRS "*.h*")

# Цель только для отображения заголовков в IDE
add_custom_target(ProjectHeaders SOURCES ${HDRS})