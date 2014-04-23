#  Copyright (c) 2014 University of Oregon
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)



# Create a library called "Apex" which includes the source file "apex.cxx".
# The extension is already found. Any number of sources could be listed here.

hpx_info("apex" "Will build APEX")

set (APEX_VERSION_MAJOR 0)
set (APEX_VERSION_MINOR 1)

set(APEX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(APEX_SOURCE_DIR ${APEX_SOURCE_DIR} PARENT_SCOPE)
set(APEX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(APEX_BINARY_DIR ${APEX_BINARY_DIR} PARENT_SCOPE)

configure_file (
    "${APEX_SOURCE_DIR}/apex_config.h.in"
    "${APEX_BINARY_DIR}/apex_config.h"
)

if(TAU_FOUND)
    hpx_info("apex" "Building APEX with TAU support")
    add_library (apexStatic STATIC apex.cpp thread_instance.cpp event_listener.cpp handler.cpp concurrency_handler.cpp policy_handler.cpp tau_listener.cpp)
    add_library (apexShared SHARED apex.cpp thread_instance.cpp event_listener.cpp handler.cpp concurrency_handler.cpp policy_handler.cpp tau_listener.cpp)
    add_definitions( -DAPEX_HAVE_TAU )
else()
    hpx_info("apex" "Building APEX without TAU support")
    add_library (apexStatic STATIC apex.cpp thread_instance.cpp event_listener.cpp handler.cpp concurrency_handler.cpp policy_handler.cpp)
    add_library (apexShared SHARED apex.cpp thread_instance.cpp event_listener.cpp handler.cpp concurrency_handler.cpp policy_handler.cpp)
endif()
set_target_properties(apexStatic PROPERTIES OUTPUT_NAME apex)
set_target_properties(apexShared PROPERTIES OUTPUT_NAME apex)

target_link_libraries(apexStatic ${LIBS})
target_link_libraries(apexShared ${LIBS})

# add the binary tree to the search path for include files
# so that we will find ApexConfig.h
include_directories("${PROJECT_BINARY_DIR}/apex")


INSTALL(FILES apex.h apex.hpp handler.hpp event_listener.hpp DESTINATION include)

INSTALL(TARGETS apexShared apexStatic
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

add_subdirectory(ITTNotify)

set(AMPLIFIER_ROOT ${AMPLIFIER_ROOT} PARENT_SCOPE)
set(HPX_HAVE_ITTNOTIFY ${HPX_HAVE_ITTNOTIFY} PARENT_SCOPE)
set(ITTNOTIFY_SOURCE_DIR ${ITTNOTIFY_SOURCE_DIR} PARENT_SCOPE)
set(ITTNOTIFY_BINARY_DIR ${ITTNOTIFY_BINARY_DIR} PARENT_SCOPE)
