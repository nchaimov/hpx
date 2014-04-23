# Create a library called "Apex" which includes the source file "apex.cxx".
# The extension is already found. Any number of sources could be listed here.

hpx_info("apex" "Will build APEX")

set (Apex_VERSION_MAJOR 0)
set (Apex_VERSION_MINOR 1)

set(APEX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(APEX_SOURCE_DIR ${APEX_SOURCE_DIR} PARENT_SCOPE)
set(APEX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(APEX_BINARY_DIR ${APEX_BINARY_DIR} PARENT_SCOPE)

configure_file (
    "${APEX_SOURCE_DIR}/ApexConfig.h.in"
    "${APEX_BINARY_DIR}/ApexConfig.h"
)

if(TAU_FOUND)
    hpx_info("apex" "Building APEX with TAU support")
    add_library (ApexStatic STATIC Apex.cpp ThreadInstance.cpp EventListener.cpp Handler.cpp ConcurrencyHandler.cpp PolicyHandler.cpp TauListener.cpp)
    add_library (ApexShared SHARED Apex.cpp ThreadInstance.cpp EventListener.cpp Handler.cpp ConcurrencyHandler.cpp PolicyHandler.cpp TauListener.cpp)
    add_definitions( -DAPEX_HAVE_TAU )
else()
    hpx_info("apex" "Building APEX without TAU support")
    add_library (ApexStatic STATIC Apex.cpp ThreadInstance.cpp EventListener.cpp Handler.cpp ConcurrencyHandler.cpp PolicyHandler.cpp)
    add_library (ApexShared SHARED Apex.cpp ThreadInstance.cpp EventListener.cpp Handler.cpp ConcurrencyHandler.cpp PolicyHandler.cpp)
endif()
set_target_properties(ApexStatic PROPERTIES OUTPUT_NAME Apex)
set_target_properties(ApexShared PROPERTIES OUTPUT_NAME Apex)

target_link_libraries(ApexStatic ${LIBS})
target_link_libraries(ApexShared ${LIBS})

# add the binary tree to the search path for include files
# so that we will find ApexConfig.h
include_directories("${PROJECT_BINARY_DIR}/Apex")


INSTALL(FILES apex.h apex.hpp Apex.hpp Handler.hpp EventListener.hpp DESTINATION include)

INSTALL(TARGETS ApexShared ApexStatic
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

add_subdirectory(ITTNotify)

set(AMPLIFIER_ROOT ${AMPLIFIER_ROOT} PARENT_SCOPE)
set(HPX_HAVE_ITTNOTIFY ${HPX_HAVE_ITTNOTIFY} PARENT_SCOPE)
set(ITTNOTIFY_SOURCE_DIR ${ITTNOTIFY_SOURCE_DIR} PARENT_SCOPE)
set(ITTNOTIFY_BINARY_DIR ${ITTNOTIFY_BINARY_DIR} PARENT_SCOPE)
