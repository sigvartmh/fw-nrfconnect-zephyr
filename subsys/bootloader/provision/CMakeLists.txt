if (CONFIG_HAS_HW_NRF_KMU)
  message(FATAL_ERROR "No KMU support added.")
else()
  set(PROVISION_SRC provision_flash.c)
endif()

add_library(
  provision
  ${PROVISION_SRC})

target_link_libraries(provision PRIVATE zephyr_interface)
