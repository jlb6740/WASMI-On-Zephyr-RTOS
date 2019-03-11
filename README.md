# WASMI-On-Zephyr-RTOS
Demonstration of WASMI running on various targets for Zephyr RTOS

(1) The driver for WASMI (lib.rs). Currently this only calls APIs to create
    a wasmi module. It does not create an instance or attempt to run. Doing
    that has been accidently broken but will be fixed hopefully soon.

(2) Code in the host directory (main.c). This is for building and running
    on host. It also includes targets for creating WASM programs that can
    be embeeded into lib.rs as a byte array.

(3) Makefile. This supports a few targets, one for host and a couple for
    the zephry (qemu and arduino). It also includes

(4) Zephyr_sample_app. This is a modification of a zephyr sample app that
    includes a cmakelist for building. This file will likely need modification
    to fit your dev enviornment.

(5) C files for generating WASM (lib directory). These are fib, hello, and
    multiply programs inspired by some samples found in the life secure WASM VM
    project

(6) i586-unknown-elfiamcu-eabi.json ... target file for currently unsupported
    rust target.