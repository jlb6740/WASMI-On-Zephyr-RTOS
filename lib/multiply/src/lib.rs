#[cfg(feature = "bindgen")]
use wasm_bindgen::prelude::*;

const MULTIPLIER: i32 = 2;

#[cfg_attr(feature = "bindgen", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn _call(n: i32) -> i32 {
	n * MULTIPLIER
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
