This is a red-black tree library implements by C.

### how to run test?
you can following these steps to run test:
1. setup [cmockery](https://github.com/google/cmockery) library.
2. create a new folder in 'test' folder, such as 'build'.
3. run 'cmake ..' in the folder you create.
4. run 'make' to build test executable file.
5. if everything's ok, run './test_rbtree'.

### how to use the code?
There's no need to compile and setup this code. File rbtree.h contains all functions and structures you can use, you can include this file where you want to using red-black tree. Besides don't forget add all .c file except files in test folder to your build system.
