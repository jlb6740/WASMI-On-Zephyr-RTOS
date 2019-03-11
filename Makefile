wasm_binaries: wasm_fib wasm_multiply wasm_hello

wasm_fib:
	mkdir -p wasm_binaries; 
	cd lib/fib; cargo clean -q; cargo build --release --no-default-features -Z unstable-options --out-dir="../../wasm_binaries/"; cd ../../;
	cd wasm_binaries; cp fib.wasm fib.wasm.orig; wasm2wat fib.wasm.orig > fib.wasm; wat2wasm fib.wasm; cd ../;
	#cd wasm_binaries; wasm-opt -Os fib.wasm -o fib.wasm; cd ../;
	echo "fib.wasm: "; xxd -p wasm_binaries/fib.wasm | tr -d '\n' | sed -e 's/.\{2\}/0x&, /g' | sed 's/^/[/' | sed 's/, $$//'  | sed 's/$$/]/' ; echo "";

wasm_multiply:
	mkdir -p wasm_binaries; 
	cd lib/multiply; cargo clean -q; cargo build --release --no-default-features -Z unstable-options --out-dir="../../wasm_binaries/"; cd ../../;
	cd wasm_binaries; cp multiply.wasm multiply.wasm.orig; wasm2wat multiply.wasm.orig > multiply.wasm; wat2wasm multiply.wasm; cd ../;
	#cd wasm_binaries; wasm-opt -Os mulitiply.wasm -o multiply.wasm; cd ../;
	echo "multiply.wasm: "; xxd -p wasm_binaries/multiply.wasm | tr -d '\n' | sed -e 's/.\{2\}/0x&, /g' | sed 's/^/[/' | sed 's/, $$//'  | sed 's/$$/]/' ; echo "";

wasm_hello:
	mkdir -p wasm_binaries; 
	cd lib/hello; cargo clean -q; cargo build --release --no-default-features -Z unstable-options --out-dir="../../wasm_binaries/"; cd ../../;
	cd wasm_binaries; cp hello.wasm hello.wasm.orig; wasm2wat hello.wasm.orig > hello.wasm; wat2wasm hello.wasm; cd ../;
	#cd wasm_binaries; wasm-opt -Os hello.wasm -o hello.wasm; cd ../;
	echo "hello.wasm: "; xxd -p wasm_binaries/hello.wasm | tr -d '\n' | sed -e 's/.\{2\}/0x&, /g' | sed 's/^/[/' | sed 's/, $$//' | sed 's/$$/]/' ; echo "";

host: hostlib host/main.c
	cd host && gcc -o ../a.out main.c ../target/x86_64-unknown-linux-gnu/release/libwasmi_zephyr.a  -lm && cd ..

hostlib:                                       
	WEE_ALLOC_STATIC_ARRAY_BACKEND_BYTES=2048576 RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=x86_64-unknown-linux-gnu -- --emit obj -C link-arg=-no-pie -C relocation-model=static  -Z print-link-args -C link-args=-static -C opt-level=z -C link-arg=-nostartfiles 

zephyrlib_qemu_x86:
	WEE_ALLOC_STATIC_ARRAY_BACKEND_BYTES=2000000 RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-linux-gnu -- -C relocation-model=static -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elf/i586-zephyr-elf-gcc #--emit obj -C relocation-model=static -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elf/i586-zephyr-elf-gcc -C link-arg="-mstack-alignment=4" -C llvm-args="-stack-alignment=4" -C llvm-args="-stackrealign" -C link-arg=-no-pie 

zephyrlib_qemu_x86_iamcu:
	WEE_ALLOC_STATIC_ARRAY_BACKEND_BYTES=4048576 RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-elfiamcu-eabi -- -C link-arg=-no-pie --emit obj -C target-cpu=i586 -C inline-threshold=1 -C relocation-model=static -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elfiamcu/i586-zephyr-elfiamcu-gcc -C target-cpu=i586 -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" 

zephyrlib_arduino_101:
	WEE_ALLOC_STATIC_ARRAY_BACKEND_BYTES=40804 RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-elfiamcu-eabi -- --emit obj -C target-cpu=i586 -C inline-threshold=1 -C relocation-model=static -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles

clean:
	cargo clean
	rm -rf wasm_binaries
	rm -f a.out

zephyrlib_qemu_x86_old:
	RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-linux-gnu -- --emit obj  -C inline-threshold=1 -C relocation-model=static -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elfiamcu/i586-zephyr-elfiamcu-gcc -C target-cpu=i586 -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles

zephyrlib_qemu_x86_iamcu_old:
	RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-elfiamcu-eabi -- --emit obj  -C inline-threshold=1 -C relocation-model=static -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elfiamcu/i586-zephyr-elfiamcu-gcc -C target-cpu=i586 -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles

zephyrlib_arduino_101_old:
	WEE_ALLOC_STATIC_ARRAY_BACKEND_BYTES=40804 RUST_TARGET_PATH=`pwd` cargo xrustc -vv --lib --release --target=i586-unknown-elfiamcu-eabi -- --emit obj  -C inline-threshold=1 -C relocation-model=static -C linker=/home/jlbirch/zephyr-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-zephyr-elfiamcu/i586-zephyr-elfiamcu-gcc -C target-cpu=i586 -C lto=yes  -C link-arg="-march=iamcu" -C link-arg="-mtune=iamcu" -Z print-link-args -C opt-level=z -C link-arg=-nostartfiles
