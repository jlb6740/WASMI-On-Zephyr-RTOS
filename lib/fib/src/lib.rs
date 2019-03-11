#[cfg(feature = "bindgen")]
use wasm_bindgen::prelude::*;

#[cfg_attr(feature = "bindgen", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn _call(n: i32) -> i32 {
	if n == 1 || n == 2 {
		1
	} else {
		_call(n - 1) + _call(n - 2)
	}
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}

