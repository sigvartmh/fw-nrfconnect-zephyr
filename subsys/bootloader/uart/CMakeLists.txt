
add_library(uart uart.c)
target_link_libraries(uart PRIVATE zephyr_interface)
target_link_libraries(uart PRIVATE libc)
target_include_directories(uart
  PRIVATE 
  .
  )
