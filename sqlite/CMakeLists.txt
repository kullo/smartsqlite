set(CMAKE_C_FLAGS "-O2 ${CMAKE_C_FLAGS} -DHAVE_USLEEP -g")
add_library(sqlite STATIC sqlite3.c)
