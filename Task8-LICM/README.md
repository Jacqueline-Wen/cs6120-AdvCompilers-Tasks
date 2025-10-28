In Task8-LICM directory, run:

```
make -C build && `brew --prefix llvm`/bin/clang -fpass-plugin=build/licm/licm.dylib simpleTest2.c
`brew --prefix llvm`/bin/clang -fpass-plugin=build/licm/licm.dylib -emit-llvm -S -o - simpleTest2.c
`brew --prefix llvm`/bin/clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone simpleTest2.c -o simpleTest2.ll
`brew --prefix llvm`/bin/opt -load-pass-plugin=build/licm/licm.dylib -passes='licm' simpleTest2.ll -S > simpleTest2_opt.ll
```