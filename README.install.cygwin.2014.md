1. Execute to `Cygwin Terminal` program.

2. rebuild and install to binutils
    1. You might check to output message like next.
	```
        Anonymous@Anonymous-PC ~
        $
	```

    2. Move to directory of binutils zip (For example, My version of binutils is 2.24.51.)
	```
        Anonymous@Anonymous-PC ~
        $ cd /usr/src/binutils-**[tap]**

        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51
        $ 
	```

    3. unzip to binutils-*.tar.bz2

    4. move to unzip directory
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ 
	```

    5. export environment variables to TARGET and PREFIX
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ export TARGET=x86_64-pc-linux

        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ export PREFIX=/usr/cross
	```

    6. binutils source - configure
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ ./configure --target=$TARGET --prefix=$PREFIX --enable-64bit-bfd --disable-shared --disable-nls
	```

    7. modify to environment information of build
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ make configure-host
	```

    8. Build to `binutils`
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ make LDFLAGS="-all-static"
	```

    9. Install to `binutils`
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $ make install
	```

    10. Supported to 64bit mode check
	```
        Anonymous@Anonymous-PC /usr/src/binutils-2.24.51-5.src/binutils-2.24.51/binutils-2.24.51
        $  x86_64-pc-linux-ld --help | grep "supported"
        x86_64-pc-linux-ld: supported targets: elf64-x86-64 elf32-i386 elf32-x86-64 a.out-i386-linux pei-i386 pei-x86-64 elf64-l1om elf64-k1om elf64-little elf64-big elf32-little elf32-big srec symbolsrec verilog tekhex binary ihex
        x86_64-pc-linux-ld: supported emulations: elf_x86_64 elf32_x86_64 elf_i386 i386linux elf_l1om elf_k1om
	```

3. Build and Install to Cross-GCC compiler
    1. You might check to output message like next.
	```
        Anonymous@Anonymous-PC ~
        $
	```

    2. If you want to install cross compiler about gcc, it should checked about install of library like next.
        - libgmp-devel
        - libmpfr-devel
        - libmpc-devel

    3. Move to directory of gcc zip (For example, My version of gcc is 4.8.3.)
	```
        Anonymous@Anonymous-PC ~
        $ cd /usr/src/gcc-**[tap]**

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src
        $ 
	```

    4. unzip to gcc-*.tar.bz2
        - If you want to install cross compiler safe, it should not use `tar`. instead you can use `bandizip` or other unzip programs.

    5. move to unzip directory
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ 
	```

    6. export environment variables to TARGET and PREFIX, PATH
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ export TARGET=x86_64-pc-linux

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ export PREFIX=/usr/cross

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ export PATH=$PREFIX/bin:$PATH
	```

    7. gcc source - configure
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ ./configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers --disable-shared --enable-multilib
	```

        - If you want to build about gcc only, `--enable-languages` option value is `c`.

    8. modify to environment information of build
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ make configure-host
	```

    9. Copy to library about build of gcc and g++
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ cp /lib/gcc/i686-pc-cygwin/4.8.3/libgcc_s.dll.a /lib/gcc/i686-pc-cygwin/4.8.3/libgcc_s.a

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ cp /lib/libmpfr.dll.a /lib/libmpfr.a

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ cp /lib/libgmp.dll.a /lib/libgmp.a

        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ cp /lib/libmpc.dll.a /lib/libmpc.a
	```

    10. Build to `gcc` & `g++`
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ make all-gcc
	```

    11. Install to `gcc` & `g++`
	```
        Anonymous@Anonymous-PC /usr/src/gcc-4.8.3-3.src/gcc-4.8.3
        $ make install-gcc
	```
