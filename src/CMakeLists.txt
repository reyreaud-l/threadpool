add_library(threadpool SHARED
  single_queue.cpp
  threadpool_base.cpp
)
target_include_directories(threadpool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Install
install(FILES threadpool.hpp DESTINATION include/threadpool)
install(FILES single_queue.hpp DESTINATION include/threadpool)
install(FILES hooks.hpp DESTINATION include/threadpool)
install(TARGETS threadpool DESTINATION lib)

# Uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_MODULE_PATH}/uninstall.cmake.in"
    "${CMAKE_MODULE_PATH}/uninstall.cmake"
    IMMEDIATE @ONLY
  )
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_MODULE_PATH}/uninstall.cmake)
endif()
