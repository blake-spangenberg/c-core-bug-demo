cmake_minimum_required(VERSION 3.16)
execute_process(
  COMMAND bash -c "sed -i 's/num_option(MBEDTLS \"Use mbedTLS\" OFF)/num_option(MBEDTLS \"Use mbedTLS\" ON)/g' ${CMAKE_CURRENT_SOURCE_DIR}/components/c-core/CMakeLists.txt"
)
set(EXTRA_COMPONENT_DIRS "components")
set(ENV{IDF_TARGET} "esp32")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project("c_core_bug_demo")
