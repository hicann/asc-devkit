# AI Core Compilation Basics<a name="section229217121411"></a>
<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T15:05:22.738Z pushedAt=2026-11-03T07:05:13.460Z nl=2 -->

After a developer finishes writing a kernel function and launches it on the host side using the <<<\>\>\> syntax, the operator source code needs to be compiled and the operator needs to be run. This section describes the basic usage of compiling operators with the BiSheng Compiler.

## AI Core SIMD Compilation
The basic command for compiling Ascend C source files using BiSheng is as follows, where **--npu-arch=dav-\<npu architecture\>** specifies the AI processor architecture version:
```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture>
```

BiSheng can use **-I \<path\>** to specify the header file path, **-L \<path\>** to specify the library path, **-l \<library\>** to specify the dynamic or static library to be linked, and **-D\<macro\>=\<value\>** to specify a macro definition. An example is as follows:
```shell
  bisheng add_kernel.asc -I <path_to_include> -L <path_to_library> -l <library> -o <output_file> -D<macro>=<value> --npu-arch=dav-<npu architecture>
```

The basic compilation process of AI Core SIMD is as follows: the host code is compiled into a host binary by the host compiler. The AI Core SIMD code is divided into cube code and vector code, which must be compiled into a cube binary and a vector binary respectively. The cube binary and vector binary are first linked into a Fatbin file, which is then merged with the host binary to generate the executable binary.
![AI Core compilation process](../../../figures/aicore_compilation.png)

- Heterogeneous compilation. For a complete example, see [LINK](../../../../../../examples/01_simd_cpp_api/02_features/04_compile/00_basic_compile/README_en.md).
  ```shell
  // ----- add_kernel.asc -----
  // Specify that the kernel function runs on the AI Core
  __global__ __vector__ add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)

  bisheng -c add_kernel.asc -o add_kernel.o --npu-arch=dav-xxxx
  bisheng -c main.cpp -o main.o -I${INSTALL_DIR}/include
  bisheng add_kernel.o main.o -o main
  // or
  bisheng main.cpp add_kernel.asc -o main --npu-arch=dav-xxxx
  ```

  > [!NOTE] Note
  > Replace `${INSTALL_DIR}` with the actual storage path of the CANN software after installation. For example, when installed by the **root** user, the default storage path after installation is **/usr/local/Ascend/cann**.

- Standalone compilation. For a complete example, see [LINK](../../../../../../examples/01_simd_cpp_api/02_features/04_compile/01_separate_compile/README_en.md).

  BiSheng heterogeneous compilation uses the whole-program compilation mode by default. This mode requires that the device program compiled from a single source file X.asc has no unresolved external device function or variable references. A device function can call device functions defined in other compilation units or access device variables defined in them, but the `-dc` compilation option must be specified on the BiSheng command line to enable linking of device code across different compilation units. This capability of supporting cross-compilation-unit linking of device code and symbols is called standalone compilation.

  Standalone compilation enables more flexible code organization, shorter compilation time, and smaller executable files. Compared with whole-program compilation, it adds a certain degree of complexity to the compilation and build process. In addition, device code linking may affect program performance, which is why this mode is not set as the default mode. Link-time optimization (LTO) can effectively reduce the performance loss introduced by standalone compilation.

  Mandatory requirements for standalone compilation:
  - A non-constant device variable defined in one compilation unit must be declared with the extern keyword when referenced in another compilation unit.
  - All constant device variables must be declared with the extern keyword both when defined and when referenced across compilation units.
  - All Ascend C source files (.asc) must be compiled with the `-dc` option.

  In the following example, **add_compute.asc** defines variables and functions, and **add_kernel.asc** references them. The two files are compiled separately and finally linked into a complete executable file.
  ```c++
  // ------- add_compute.asc -------
  __gm__ int dev_var = 5;
  __aicore__ void add_compute(...);

  // ------- add_kernel.asc -------
  extern __gm__ int dev_var;
  __aicore__  void add_compute(...);
  __global__  __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z) {
    dev_var = 0;
    add_compute(...);
  }
  ```

  ```shell
  bisheng -dc add_compute.asc -o add_compute.o --npu-arch=dav-xxxx
  bisheng -dc add_kernel.asc -o add_kernel.o --npu-arch=dav-xxxx
  bisheng -c main.cpp -o main.o -I${INSTALL_DIR}/include
  bisheng add_compute.o add_kernel.o main.o -o program
  ```
  > [!NOTE] Note
  > Replace `${INSTALL_DIR}` with the actual storage path of the CANN software after installation. For example, when installed by the **root** user, the default storage path after installation is **/usr/local/Ascend/cann**.

- Dynamic library compilation. For a complete example, see [LINK](../../../../../../examples/01_simd_cpp_api/02_features/04_compile/02_dynamic_library_compile/README_en.md).
   ```shell
  bisheng -shared add_kernel.asc -o libadd_kernel.so -fPIC --npu-arch=dav-xxxx
  ```

- Static library compilation. For a complete example, see [LINK](../../../../../../examples/01_simd_cpp_api/02_features/04_compile/03_static_library_compile/README_en.md).
  ```shell
  bisheng -lib add_kernel.asc -o libadd_kernel.a --npu-arch=dav-xxxx
  ```

## AI Core SIMT Compilation

When compiling Ascend C SIMT source files with the BiSheng Compiler, add `--enable-simt` to the basic command for AI Core SIMD compilation. The following is an example:

```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> --enable-simt
```

The basic compilation process of AI Core SIMT is as follows: the host code is compiled into a host binary by the host compiler, and the AI Core SIMT code is compiled into a SIMT binary. The SIMT binary is first linked into a Fatbin file, which is then merged with the host binary to generate an executable binary.
![AI Core SIMT compilation process](../../../figures/aicore_simt_comilation.png)

## Summary of Basic Compilation Commands

|Compilation Method|AI Core SIMD Compilation Command| AI Core SIMT Compilation Command|
|-|-|-|
|Heterogeneous compilation| bisheng <source_file>.asc -o \<output_file\> --npu-arch=dav-\<npu architecture\> | bisheng <source_file>.asc -o <output_file> --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|Standalone compilation|bisheng **-dc** <source_file>.asc -o \<output_file\>**.o** --npu-arch=dav-\<npu architecture\>| bisheng **-dc** <source_file>.asc -o <output_file>**.o** --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|Dynamic library compilation|bisheng **-shared** <source_file>.asc -o \<output_file\>**.so** --npu-arch=dav-\<npu architecture\> **-fPIC**| bisheng **-shared** <source_file>.asc -o <output_file>**.so** --npu-arch=dav-\<npu architecture\> **--enable-simt** **-fPIC**|
|Static library compilation|bisheng **-lib** <source_file>.asc -o \<output_file\>**.a** --npu-arch=dav-\<npu architecture\>| bisheng **-lib** <source_file>.asc -o <output_file>**.a** --npu-arch=dav-\<npu architecture\> **--enable-simt**|


## Common Compilation Options<a name="ZH-CN_TOPIC_0000002462746461"></a>

| Option | Required | Description |
|------|----------|------|
| -help | No | Displays help. |
| --npu-arch | Yes | Specifies the AI processor architecture at compilation time. The value is dav-&lt;arch-version&gt;, where &lt;arch-version&gt; is the NPU architecture version. For the architecture version corresponding to each product model, see the [mapping table](../../language_extension/simd_builtin_keywords.md#npu-arch). |
| --npu-soc | No | Specifies the AI processor model at compilation time. When both npu-soc and npu-arch are configured, the npu-arch configuration takes precedence. For how to obtain the AI processor model, see [the note below](#note-reference). |
| -x | No | Specifies the compilation language. For example, -x asc specifies the Ascend C programming language. |
| -o &lt;file&gt; | No | Specifies the name and location of the output file. |
| -c | No | Compiles and generates an object file. |
| -dc | No | Compiles and generates a relocatable object file. |
| -shared, --shared | No | Compiles and generates a dynamic link library. |
| -lib, --cce-build-static-lib | No | Compiles and generates a static link library. The compiler compiles and links the device-side code to generate a device-side binary, then uses this file as the input for host-side compilation, and finally links to generate a static link library. |
| -g | No | Adds debugging information during compilation. |
| --sanitizer | No | Adds code correctness verification information during compilation. When using the sanitizer option, you must also add the -g option, and it cannot be used in the -O0 scenario.<br>Note that after this option is enabled, GlobalTensor uses the L2 Cache by default, and the mode of not using the L2 Cache cannot be set through the AscendC::SetL2CacheHint API. |
| -fPIC | No | Instructs the compiler to generate position-independent code. |
| -O | No | Specifies the compiler optimization level. Currently -O3, -O2, and -O0 are supported. |
| --run-mode=sim | No | sim mode: at link time, the user adds the implementation library corresponding to the simulation mode, so that the code runs in simulation mode. Simulation-related logs can be viewed, which facilitates performance debugging. |
| --enable-simt | No | In the SIMT programming scenario, specifies SIMT compilation. When the compilation option `--enable-simt` is set, including the header file of [SIMD API](../../../../api/SIMD-API/SIMD-API.md) causes a compilation failure. |
| --cce-disable-asc-reserved-ubuf | No | Disables the use of the reserved Unified Buffer (UB) space by Ascend C APIs. After it is enabled, Ascend C APIs that depend on the reserved UB space are unavailable on the corresponding chip architecture, and a compilation error is reported when they are used. For the list of APIs that use the reserved UB space, see [APIs that use the reserved UB space](../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#api-scope-for-reservation-usage-of-ub-space). Because the same API uses the reserved UB space differently on different NPU architectures, after enabling this compilation option, you need to manually adjust the API call method or replace it with an implementation that does not depend on the reserved UB space to complete compatibility migration. |
| --cce-disable-vf-stack-reserved-ubuf | No | Disables the UB space reserved for the SIMD VF stack. After it is enabled, the compiler no longer reserves this part of the UB space, and this space can be used as ordinary UB space. For [NPU architecture version 2201](../../language_extension/simd_builtin_keywords.md#npu-arch), this compilation option has no actual effect. For [NPU architecture version 3510](../../language_extension/simd_builtin_keywords.md#npu-arch), this compilation option takes effect. When the user uses this compilation option, the compiler cannot use the reserved UB space to cache register spills, and the user must ensure that registers do not spill. |
| --cce-auto-sync | No | Enables automatic synchronization of the BiSheng Compiler. The execution units inside the AI Core are asynchronous and parallel, and Tensor reads and writes may have data dependencies. After it is enabled, the BiSheng Compiler can automatically insert partial synchronization. For details, see [Key Feature Description](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/key_features.md). |
| --cce-auto-sync-log=&lt;file&gt; | No | Outputs the automatic synchronization insertion information of the BiSheng Compiler to the &lt;file&gt; file. |
| --cce-ftz=false/true | No | Controls the handling of denormalized numbers (extremely small numbers). true means that denormalized numbers are directly flushed to 0, which improves performance. false means that denormalized numbers are retained, which provides higher precision but may lower performance. The default value is true. Currently SIMD and SIMT are supported, where SIMD supports only NPU architecture version 3510. |
| --cce-prec-div=false/true | No | Specifies whether to use precise division calculation. The default value is false. Currently only SIMT is supported. |
| --cce-prec-sqrt=false/true | No | Specifies whether to use the precise square root function. The default value is false. Currently only SIMT is supported. |
| --cce-use-fast-math=false/true | No | Specifies whether to enable the fast calculation mode. The default value is true. When enabled, it is equivalent to --cce-ftz=true --cce-prec-div=false --cce-prec-sqrt=false (if there is a conflict, the settings of --cce-ftz, --cce-prec-div, and --cce-prec-sqrt prevail). Currently only SIMT is supported. |

>[!NOTE] Note
><!-- npu="A3,910b,910,310p,310b" id13 -->
>- For the following products, the following compilation options are not supported: --cce-ftz, --cce-prec-div, --cce-prec-sqrt, --cce-use-fast-math.<br><br>
>    <!-- npu="A3" id18 -->
>    Atlas A3 products
>    <!-- end id18 -->
>
>    <!-- npu="910b" id14 -->
>    Atlas A2 products  
>    <!-- end id14 -->
>
>    <!-- npu="310b" id15 -->
>    Atlas 200I/500 A2 inference products  
>    <!-- end id15 -->
>
>    <!-- npu="310p" id16 -->
>    Atlas inference products  
>    <!-- end id16 -->
>
>    <!-- npu="910" id17 -->
>    Atlas training products<br><br> 
>    <!-- end id17 -->
><!-- end id13 -->
><!-- npu="950" id19 -->
>- For Ascend 950PR&950DT products, the following compilation options are supported: --cce-ftz, --cce-prec-div, --cce-prec-sqrt, --cce-use-fast-math.
><!-- end id19 -->

For more compilation commands and usage, see [BiSheng Compiler](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/compiler/BishengCompiler/atlas_bisheng_10_0001.html).

<a id="note-reference"></a>

>[!NOTE] Note
><!-- npu="910b,910,310p,310b" id1 -->
>- For the following products, run the `npu-smi info` command on the server where the AI processor is installed to query the Name information. The actual configuration value is AscendName. For example, if the Name value is xxxyyy, the actual configuration value is Ascendxxxyy.<br><br>
>    <!-- npu="910b" id2 -->
>    Atlas A2 products  
>    <!-- end id2 -->
>
>    <!-- npu="310b" id3 -->
>    Atlas 200I/500 A2 inference products  
>    <!-- end id3 -->
>
>    <!-- npu="310p" id4 -->
>    Atlas inference products  
>    <!-- end id4 -->
>
>    <!-- npu="910" id5 -->
>    Atlas training products<br><br> 
>    <!-- end id5 -->
><!-- end id1 -->
><!-- npu="A3" id6 -->
>- For <term>Atlas A3 products</term>, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the AI processor is installed to query the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234. Where:<br>`id`: device ID. The NPU ID obtained by running the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.<br>`chip_id`: chip ID. The Chip ID obtained by running the `npu-smi info -m` command is the chip ID, represented as `<chip_id>` in the command.<br><br>
><!-- end id6 -->
><!-- npu="950" id7 -->
>- For Ascend 950PR&950DT products, run the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234.<br>Where: `id` is the device ID. The NPU ID obtained by running the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.
><!-- end id7 -->


## CMake Compilation<a name="ZH-CN_TOPIC_0000002428982142"></a>

In a project, you can use CMake to compile Ascend C operators with the BiSheng Compiler more conveniently, generating executable files, dynamic libraries, static libraries, or binary files.

The following shows an example of a CMake script and the description of its core steps:

```cmake
# 1. Set compilation configuration variables
# CMAKE_ASC_ARCHITECTURES: specifies the NPU architecture version. The current default value is dav-2201, which can be overridden by -DCMAKE_ASC_ARCHITECTURES=dav-xxxx
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture")

# 2. find_package(ASC) is a CMake command used to find and configure the Ascend C compilation toolchain
find_package(ASC)

# 3. Specify that the languages supported by the project include ASC, where ASC indicates that the BiSheng Compiler is used to compile the Ascend C programming language
project(kernel_samples LANGUAGES ASC)

# 4. Use CMake interfaces to compile executable files, dynamic libraries, static libraries, and binary files
add_executable(demo
    add_custom.asc
)
```

When compiling AI Core SIMT code, add `--enable-simt` to the compilation options, or set `CMAKE_ASC_ENABLE_SIMT` to ON to enable the SIMT compilation mode. An example is as follows. For the complete sample, click [here](../../../../../../examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt).

```cmake
# CMAKE_ASC_ARCHITECTURES: Specify the NPU architecture version. You can override the default value with -DCMAKE_ASC_ARCHITECTURES=dav-xxxx
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")
# CMAKE_ASC_ENABLE_SIMT: Enable the SIMT compilation mode. It must be set before find_package(ASC)
set(CMAKE_ASC_ENABLE_SIMT ON)

find_package(ASC)
project(kernel_samples LANGUAGES ASC)

add_executable(demo
    add_custom.asc
)
```

The following shows examples of dynamic library and static library compilation, and how to switch the source file to be compiled with the ASC language:

-   Compile the .asc file to generate a dynamic library.

    ```cmake
    # 3. Use the CMake API to compile a dynamic library
    add_library(kernel_shared SHARED
        cube.asc
        vector.asc
        mix.asc
    )
    ```

-   Compile the .asc file to generate a static library.

    ```cmake
    # 3. Use the CMake API to compile a static library
    add_library(kernel_lib STATIC
          cube.asc
          vector.asc
          mix.asc
    )
    ```
**Table 1** Common CMAKE configuration variables

| Variable Name | Description |
|--|--|
| CMAKE_BUILD_TYPE | Compilation mode option. It can be set to Release or Debug. The Release version does not contain debugging information and is used to compile the final release version. The Debug version contains debugging information to facilitate development and debugging. When other values are set, CMake issues a warning but does not abort the build. |
| CMAKE_ASC_COMPILER | Specifies the compiler path of Ascend C. By default, CMake detects it automatically. If the path does not exist, an error is reported and the configuration phase is terminated. |
| CMAKE_ASC_SOURCE_FILE_EXTENSIONS | List of ASC source file extensions. The default value is asc. |
| CMAKE_ASC_STANDARD | Specifies the C++ standard version used for compilation. The default value is 17. When a value lower than 17 is set, CMake issues a warning. |
| CMAKE_ASC_ARCHITECTURES | Specifies the target NPU architecture version. The value is `dav-<arch-version>` (for example, dav-2201 and dav-3510). In NPU run mode, CMake automatically injects this value into the compilation command as `--npu-arch=<value>`. |
| CMAKE_ASC_RUN_MODE | Specifies the operator run mode. Supported values are npu (default, running on an NPU device), cpu (CPU debugging mode), and sim (simulation mode). CMake automatically adjusts the linked libraries and compilation options in different modes. |
| CMAKE_ASC_FLAGS | Common compilation options added for all build types (Debug, Release, and so on). |
| CMAKE_ASC_FLAGS\_DEBUG | Compilation options dedicated to the Debug build. The default value is `-O0 -g`. |
| CMAKE_ASC_FLAGS\_RELEASE | Compilation options dedicated to the Release build. The default value is `-O3 -DNDEBUG`. |
| CMAKE_ASC_COMPILER_LAUNCHER | Pre-launcher of the compilation command (for example, ccache and distcc). When it is not set, the compiler is invoked directly. |
| CMAKE_ASC_LINKER_LAUNCHER | Pre-launcher of the link command. When it is not set, linking is executed directly. |
| CMAKE_ASC_COMPILER_AR | Static library archiving tool. The system ar tool is used by default. After it is set, this tool replaces the default ar in the static library archiving command. |
| CMAKE_ASC_COMPILER_LINKER | Link driver. The bisheng compiler is used as the link driver by default. Set it only when there is an explicit replacement requirement. |
| CMAKE_ASC_ENABLE_SIMT | Whether to enable the SIMT compilation mode. When it is set to ON, the --enable-simt option is automatically injected into the compilation command. The default value is OFF. |
| CMAKE_INSTALL_PREFIX | A built-in CMake variable that specifies the installation path prefix when CMake executes the install command.|

>[!CAUTION] Caution
> In a CMake project, variables that affect Ascend C language initialization, compiler detection, and compilation rule generation
> must be set before `find_package(ASC)`, or specified with `-D` when executing the `cmake` configuration command.
> These variables are read during Ascend C language initialization. If they are set after `find_package(ASC)`,
> they may not affect the compilation rules that have already been generated.

## Other Compilation Precautions

### Built-in Compilation Macro Switches<a name="section57020345148"></a>

The built-in compilation macro switches are listed as follows:
 -   **ASCENDC\_DUMP** controls the dump switch. The default switch is enabled. After developers call **printf/DumpTensor/assert**, information is printed. (Note that the kernel file of a direct-call project contains host functions. If **printf** is called in a host function, the printf-related initialization in the kernel is also triggered, which affects the kernel execution performance.) When it is set to 0, the switch is disabled. The following is an example:
     ```shell
     # Disable the printf printing of all operators
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DUMP=0
     ```
 <!-- npu="910b,310p" id8 -->
 -   **ASCENDC\_DEBUG** controls the debugging switch of the Ascend C API. The default switch is disabled. After this compilation macro is added, the switch is enabled. In this case, the assert check inside the API takes effect. If the check fails, assert logs are printed to the screen. Enabling this function affects the actual running performance of the operator to a certain extent, and it is usually used in the debugging phase. The following is an example:
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DEBUG
     ```
     The product models that currently support the ASCENDC\_DEBUG feature are:
     <!-- npu="310p" id9 -->
     Atlas inference products
     <!-- end id9 -->
     <!-- npu="910b" id10 -->
     Atlas A2 products
     <!-- end id10 -->
 <!-- end id8 -->

 <!-- npu="950" id11 -->
 -   **ENABLE\_CV\_COMM\_VIA\_SSBUF** controls whether to use SSBuffer and the hard channel from UB to the L1 Buffer. Pay attention to this option when CV communication (AIC and AIV) is involved or when data movement APIs are used. Enabling this option can improve the performance of related APIs or extend more functions. The default switch is disabled. When it is set to true, the switch is enabled. The following is an example:
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DENABLE_CV_COMM_VIA_SSBUF=true
     ```
     This option is supported only on Ascend 950PR&950DT products.
     -   For operators ported from other hardware platforms to this platform, the switch is disabled by default to maintain compatibility.
     -   For operators newly developed on this platform, the switch needs to be enabled in the following scenarios: using the advanced Matmul API for cube computation and using APIs such as SetTensorScaleA. These APIs are new functions of Ascend 950PR&950DT products, and their internal implementation uses SSBuffer; using the DataCopy API to copy data from UB to the L1 Buffer.
 <!-- end id11 -->

 <!-- npu="950" id12 -->
 -   **NO\_OVERLAP\_IN\_MULTI\_REPEAT**
  This compilation option is used to remove unnecessary memory synchronization instructions when there is no address overlap, so as to improve performance. For Ascend 950PR&950DT products, when using the high-dimensional split computation APIs of the basic API, memory synchronization instructions are inserted by default to ensure data correctness in complex scenarios such as address overlap, but these synchronization instructions incur performance overhead. In scenarios that pursue ultimate performance, if you can confirm that memory overlap will never occur in the code under any circumstances, you can use this option.
 <!-- end id12 -->

 <!-- npu="950" id20 -->
 -   **ASCENDC\_USE\_LEGACY\_PRECISION** Used to control the implementation version of SIMT APIs. The switch is disabled by default. For Ascend 950PR&950DT products, CANN 9.2.0 improves the precision of the implementation of some SIMT APIs. If your operator is not sensitive to precision and you find that performance has degraded, you can enable this macro to roll back the related SIMT APIs to the previous implementation version and restore the original performance level. The following is an example:
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_USE_LEGACY_PRECISION
     ```
     This option is supported only on Ascend 950PR&950DT products, and is used to ensure that operators developed before CANN 9.2.0 remain compatible after the SIMT API implementation changes. It is not recommended for operators developed after CANN 9.2.0.
 <!-- end id20 -->


### Built-in Libraries<a name="section57020345148"></a>

The library files linked by default by the BiSheng compiler are listed as follows:

| Name | Description |
|------|----------|
| libascendc_runtime.a | Library for assembling Ascend C operator parameters and other data. |
| libruntime.so | Runtime library. |
| libprofapi.so | Library for collecting Ascend C operator runtime performance data. |
| libunified_dlog.so | CANN log collection library. |
| libmmpa.so | CANN system API library. |
| libascend_dump.so | CANN maintenance and test information library. |
| libc_sec.so | CANN security function library. |
| liberror_manager.so | CANN error information management library. |
| libascendcl.so | Library of acl-related APIs. |

### Commonly Used Libraries for Advanced APIs
When using advanced APIs, you must link the following libraries because these libraries are depended by the advanced API functions. In other scenarios, you can decide whether to link these libraries based on your specific requirements.

|Library Name|Description|Usage Scenario|Dynamic Library Path|
|--|--|--|--|
|libtiling_api.a|Library related to tiling functions.|Link this library when using Tiling APIs related to advanced APIs.|${ASCEND_HOME_PATH}/lib64|
|libregister.so|Library related to tiling functions.|Link this library when using Tiling APIs related to advanced APIs.|${ASCEND_HOME_PATH}/lib64|
|libgraph_base.so|Library of basic data structures and APIs.|Link this library when calling basic structures such as ge::Shape and ge::DataType.|${ASCEND_HOME_PATH}/lib64|
|libplatform.so|Library of hardware platform information.|Link this library when using hardware platform information APIs related to PlatformAscendC.|${ASCEND_HOME_PATH}/lib64|
