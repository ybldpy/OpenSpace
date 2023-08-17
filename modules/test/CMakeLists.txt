include(${PROJECT_SOURCE_DIR}/support/cmake/module_definition.cmake)

set(HEADER_FILES
  testmodule.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  testmodule.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})



create_new_module(
  "Test"
  test_module
  STATIC
  ${HEADER_FILES} ${SOURCE_FILES}
)
