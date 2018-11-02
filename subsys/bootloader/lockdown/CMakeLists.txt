if (CONFIG_SB_FLASH_LOCKDOWN)
  # Find out which lockdown should be used
  if (CONFIG_HAS_HW_NRF_SPU)
    set(LOCKDOWN_SRC lockdown_spu.c)
  elseif (CONFIG_HAS_HW_NRF_ACL)
    set(LOCKDOWN_SRC lockdown_acl.c)
  elseif (CONFIG_HAS_HW_BPROT)
    set(LOCKDOWN_SRC lockdown_bprot.c)
  else()
    message(FATAL_ERROR "No lockdown backend selected.")
  endif()

  add_library(
    lockdown
    ${LOCKDOWN_SRC})
  target_link_libraries(lockdown PRIVATE zephyr_interface)
endif ()
