cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(hello_world)

target_sources(app PRIVATE src/main.c)

# The external static library that we are linking with does not know
# how to build for this platform so we export all the flags used in
# this zephyr build to the external build system.
#
# Other external build systems may be self-contained enough that they
# do not need any build information from zephyr. Or they may be
# incompatible with certain zephyr options and need them to be
# filtered out.
zephyr_get_include_directories_for_lang_as_string(       C includes)
zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(       C definitions)
zephyr_get_compile_options_for_lang_as_string(           C options)


set(external_project_cflags
  "${includes} ${definitions} ${options} ${system_includes}"
  )

include(ExternalProject)

# Add an external project to be able download and build the third
# party library. In this case downloading is not necessary as it has
# been committed to the repository.

if("${BOARD}" STREQUAL "arduino_101")
    set(mylib_src_dir   $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-elfiamcu-eabi/release)
    set(mylib_build_dir $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-elfiamcu-eabi/release)
    message("BOARD: ardunio_101 **************")
elseif("${BOARD}" STREQUAL "qemu_x86_iamcu")
    set(mylib_src_dir   $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-elfiamcu-eabi/release)
    set(mylib_build_dir $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-elfiamcu-eabi/release)
    #set(mylib_src_dir   $ENV{HOME}/wasmi_cdriver/target/i586-zephyr-arduino_101-elf/release)
    #set(mylib_build_dir $ENV{HOME}/wasmi_cdriver/target/i586-zephyr-arduino_101-elf/release)
    message("BOARD: qemu_x86_iamcu **************")
else("${BOARD}" STREQUAL "qemu_x86")
    set(mylib_src_dir   $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-linux-gnu/release)
    set(mylib_build_dir $ENV{HOME}/github_dev/WASMI-On-Zephyr-RTOS/target/i586-unknown-linux-gnu/release)
    message("BOARD: qemu_x86 **************")
endif()


set(MYLIB_LIB_DIR     ${mylib_build_dir}/)
set(MYLIB_INCLUDE_DIR ${mylib_src_dir}/)

# Create a wrapper CMake library that our app can link with
add_library(mylib_lib STATIC IMPORTED GLOBAL)

set_target_properties(mylib_lib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MYLIB_INCLUDE_DIR})
set_target_properties(mylib_lib PROPERTIES IMPORTED_LOCATION ${MYLIB_LIB_DIR}/libwasmi_zephyr.a)
#set_target_properties(mylib_lib PROPERTIES IMPORTED_LOCATION ${MYLIB_LIB_DIR}/deps/wasmi_zephyr-a5214f9ca6841f55.o)
#set_target_properties(mylib_lib PROPERTIES IMPORTED_LOCATION ${MYLIB_LIB_DIR}/deps/wasmi_cdriver-16c7666ac85bee76.o)

target_link_libraries(app PUBLIC mylib_lib)