# Make sure the compiler can find include files from our Apex library. 
include_directories (${APEX_SOURCE_DIR})

# gcc on apple needs the apex library to link the dylib
# if(APPLE)
  link_directories (${APEX_BINARY_DIR})
# endif()

set(ITTNOTIFY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ITTNOTIFY_SOURCE_DIR ${ITTNOTIFY_SOURCE_DIR} PARENT_SCOPE)
set(ITTNOTIFY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ITTNOTIFY_BINARY_DIR ${ITTNOTIFY_BINARY_DIR} PARENT_SCOPE)

# Create a library called "ittnotify" which includes the source file
# "ittnotify.cpp". 
# The extension is already found. Any number of sources could be listed here. 
add_library (ittnotifyStatic STATIC ittnotify.cpp)
add_library (ittnotifyShared SHARED ittnotify.cpp)
set_target_properties (ittnotifyStatic PROPERTIES OUTPUT_NAME ittnotify)
set_target_properties (ittnotifyShared PROPERTIES OUTPUT_NAME ittnotify)

# target_link_libraries (ittnotifyStatic ${LIBS} ${Boost_LIBRARIES})
# gcc on apple needs the apex library to link the dylib
# if(APPLE)
target_link_libraries (ittnotifyShared Apex ${LIBS} ${Boost_LIBRARIES})
add_dependencies (ittnotifyShared ApexShared)
add_dependencies (ittnotifyStatic ApexStatic)
# else()
# target_link_libraries (ittnotifyShared ${LIBS})
# endif()

# add the binary tree to the search path for include files
# so that we will find ittnotify.h
include_directories ("${PROJECT_BINARY_DIR}/Apex")

INSTALL(FILES ittnotify.h DESTINATION include)
INSTALL(FILES legacy/ittnotify.h DESTINATION include/legacy)

INSTALL(TARGETS ittnotifyShared ittnotifyStatic
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib64
)

if(NOT AMPLIFIER_ROOT)
  set(AMPLIFIER_ROOT ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
  set(HPX_HAVE_ITTNOTIFY ON PARENT_SCOPE)
endif()
