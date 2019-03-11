#![feature(lang_items)]
#![no_std]

#![feature(alloc)]

#![feature(naked_functions)]
#![feature(asm)] 

//#![no_builtins]
#![feature(core_intrinsics)]
//#![feature(compiler_builtins_lib)]

#![feature(alloc_error_handler)] 

#[macro_use]
extern crate alloc;

//extern crate compiler_builtins;

// Required to use the `alloc` crate and its types, the `abort` intrinsic, and a
// custom panic handler.
//#![feature(alloc, core_intrinsics)]

// Use `wee_alloc` as the global allocator.
extern crate wee_alloc;
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

#[alloc_error_handler]
fn foo(_: core::alloc::Layout) -> ! {
    loop {}
}

use core::panic::PanicInfo;
//use libc::{c_char, c_int};

/// This function is called on panic.
#[lang = "panic_impl"]
#[no_mangle]
pub extern "C" fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[naked]
#[no_mangle]
#[cfg(all(target_arch = "x86", not(feature = "mangled-names")))]
pub unsafe extern fn __rust_probestack() {
    // This is the same as x86_64 above, only translated for 32-bit sizes. Note
    // that on Unix we're expected to restore everything as it was, this
    // function basically can't tamper with anything.
    //
    // The ABI here is the same as x86_64, except everything is 32-bits large.
    asm!("
        push   %ecx
        mov    %eax,%ecx
        cmp    $$0x1000,%ecx
        jna    3f
    2:
        sub    $$0x1000,%esp
        test   %esp,8(%esp)
        sub    $$0x1000,%ecx
        cmp    $$0x1000,%ecx
        ja     2b
    3:
        sub    %ecx,%esp
        test   %esp,8(%esp)
        add    %eax,%esp
        pop    %ecx
        ret
    " ::: "memory" : "volatile");
    ::core::intrinsics::unreachable();
}

extern crate wasmi;
use wasmi::{Module, ModuleInstance, RuntimeValue, ImportsBuilder, NopExternals};

extern {
    #[no_mangle]
    #[allow(dead_code)]
    pub fn printk(format: *const u8, ...) -> i32;
    pub fn testprint(format: i32) -> ();
    pub fn printf(format: *const u8, ...) -> i32;  
}

fn interpret(input: i32) -> i32 {

    //Multiplier
    let buf = vec![0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x09, 0x02, 0x60, 0x00, 0x00, 0x60, 0x01, 0x7f, 0x01, 0x7f, 0x03, 0x03, 0x02, 0x00, 0x01, 0x04, 0x05, 0x01, 0x70, 0x01, 0x01, 0x01, 0x05, 0x03, 0x01, 0x00, 0x10, 0x06, 0x19, 0x03, 0x7f, 0x01, 0x41, 0x80, 0x80, 0xc0, 0x00, 0x0b, 0x7f, 0x00, 0x41, 0x80, 0x80, 0xc0, 0x00, 0x0b, 0x7f, 0x00, 0x41, 0x80, 0x80, 0xc0, 0x00, 0x0b, 0x07, 0x2d, 0x04, 0x06, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x02, 0x00, 0x0b, 0x5f, 0x5f, 0x68, 0x65, 0x61, 0x70, 0x5f, 0x62, 0x61, 0x73, 0x65, 0x03, 0x01, 0x0a, 0x5f, 0x5f, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x65, 0x6e, 0x64, 0x03, 0x02, 0x05, 0x5f, 0x63, 0x61, 0x6c, 0x6c, 0x00, 0x01, 0x0a, 0x0c, 0x02, 0x02, 0x00, 0x0b, 0x07, 0x00, 0x20, 0x00, 0x41, 0x01, 0x74, 0x0b];
    let module = Module::from_buffer(buf).unwrap();

    return 1;

    let main = ModuleInstance::new(&module, &ImportsBuilder::default())
        .expect("Failed to instantiate module")
        .run_start(&mut NopExternals)
        .expect("Failed to run start function in module");

    let x = match main.invoke_export("_call", &[RuntimeValue::I32(input)], &mut NopExternals) {
	   Ok(val)  => val,
       Err(_err) => return 200,
	}; 

    return x.unwrap().try_into::<i32>().unwrap();
}

   
#[no_mangle]
pub extern "C" fn hi_rust(x:i32) -> i32 {
    //return x;
    return interpret(x);
}
