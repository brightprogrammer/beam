add_library(beam 
  http.c html.c log.c file.c
  container/vec.c container/list.c container/string.c 
  frontend/base.c
)

add_executable(server main.c)
target_link_libraries(server beam)

add_subdirectory(aoc)

