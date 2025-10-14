# llvm-pass-div-by-1

LLVM pass that replaces divide by zeros with divide by 1, and prints every time
divide by 0 is encountered.
It's for LLVM 17.
This code is adapted from https://github.com/sampsyo/llvm-pass-skeleton. 

Build:

    $ mkdir build
    $ cd build
    $ CC=/opt/homebrew/opt/llvm/bin/clang CXX=/opt/homebrew/opt/llvm/bin/clang++ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -fpass-plugin=build/divBy1/DivBy1.dylib -O0 testDiv.c -o test
