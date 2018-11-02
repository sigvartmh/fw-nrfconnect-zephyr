set(EXTS ${ZEPHYR_BASE}/ext)
set(SEGGER ${EXTS}/debug/segger)

add_library(segger_rtt STATIC
  ${SEGGER}/rtt/SEGGER_RTT.c
  ${SEGGER}/rtt/SEGGER_RTT_printf.c
)

target_link_libraries(segger_rtt PRIVATE libc)
target_link_libraries(segger_rtt PRIVATE zephyr_interface)


