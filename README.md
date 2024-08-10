**Physics+**

***Dependencies***
* Vulkan 1.3.292.0
* GLFW 3.4.0

***Building***
Building Physics+ through CMake is incredibly simple and highly recommended, however one may write their own build files to manually compile the project.

****With CMake:****
1. Create a build directory: `mkdir <build_directory>` (common practice is to call this directory `build`)
2. Navigate to the build directory: `cd <build_directory>`
3. Create a CMake build configuration (see list of options below): `cmake .. <build_options>`
4. Build the library with the generated build files
5. The build library will be located in `<build_directory>/lib`
6. If the headers were built, they will be located in `<build_directory>/include`

****CMake Build Options****
* `PHYSP_GENERATE_HEADERS`: Requires zsh. Generates the physp headers with the header generation script
* `PHYSP_BUILD_TESTS`: Requires Catch2. Compiles tests for the library

These options are either `ON` or `OFF` and can be configured when generating the CMake build configuration as such: `cmake <source_directory> -DPHYSP_GENERATE_HEADERS=<ON/OFF> -DPHYSP_BUILD_TESTS=<ON/OFF>`

***Linking***
For manually linking, link to `-lphysp` or the path of the where the library is on your machine. A CMake package has not yet been created, so linking with CMake is similar to linking manually.