# How to contribute

Here are a few guidelines that I hope will ease the process of contributing.

## Files to change

:exclamation: Before you make any changes, note that content of [`lib` directory](https://github.com/seleznevae/libfort/tree/develop/lib) is **generated** from the source files in the [`src` directory](https://github.com/seleznevae/libfort/tree/develop/src). Please **do not** edit files in `lib` directory directly, but change corresponding files in the `src` directory and regenerate files in the `lib` directory by executing `python amalgamate.py`.

If you add or change a feature, please also add a unit test. The unit tests and all code examples can be compiled and executed with

   ```sh
   $ mkdir build
   $ cd build
   $ cmake ..
   $ cmake --build .
   $ ctest
   ```

