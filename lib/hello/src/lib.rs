#[cfg(feature = "bindgen")]
use wasm_bindgen::prelude::*;

extern {
    #[no_mangle]
    #[allow(dead_code)]
    pub fn printf(format: *const u8, ...) -> i32;  
}

#[cfg(feature = "bindgen")]
#[wasm_bindgen]
#[no_mangle]
pub extern "C" fn _call(_n: i32) -> i32 {
    unsafe {
	    printf(b"Hello From WASM!\n" as *const u8)
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
