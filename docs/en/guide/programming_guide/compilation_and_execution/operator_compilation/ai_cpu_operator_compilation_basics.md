# AI CPU Operator Compilation<a name="ZH-CN_TOPIC_0000002522571023"></a>

<!-- md-trans-meta sourceCommit=40865883d2b8f68c5388e72b1ff247aaf16ecd3d translatedAt=2026-08-26T10:53:16.351Z pushedAt=2026-08-31T03:46:36.725Z -->

Unlike AI Core operators, which require only a single `.asc` file to compile and generate an executable file, AI CPU operators require both an `.aicpu` file (kernel function definition) and an `.asc` file (which calls the kernel function through the kernel call operator) to compile and generate an executable file.

## Compilation Using the BiSheng Command Line<a name="section153291123460"></a>

This section uses a Hello World printing sample to explain how to compile AI CPU operators through the BiSheng command line. The sample contains the `hello_world.aicpu` file (AI CPU kernel function definition) and the `main.asc` file (which calls the AI CPU kernel function through the kernel call operator).

The content of the **hello\_world.aicpu** file is as follows:

```
#include "aicpu_api.h"

__global__ __aicpu__ uint32_t hello_world(void *args)
{
    AscendC::printf("Hello World!!!\n");
    return 0;
}
```

On the host side, the kernel call operator <<<...\>\>\> is used to call the AI CPU operator. The sample code of **main.asc** is as follows:

```
#include "acl/acl.h"

struct KernelArgs {
    int mode;
};

extern __global__ __aicpu__ uint32_t hello_world(void *args);

int32_t main(int argc, char const *argv[])
{
    aclInit(nullptr);
    int32_t deviceId = 0;
    aclrtSetDevice(deviceId);
    aclrtStream stream = nullptr;
    aclrtCreateStream(&stream);

    struct KernelArgs args = {0};
    constexpr uint32_t numBlocks = 1;
    hello_world<<<numBlocks, nullptr, stream>>>(&args, sizeof(KernelArgs));
    aclrtSynchronizeStream(stream);

    aclrtDestroyStream(stream);
    aclrtResetDevice(deviceId);
    aclFinalize();
    return 0;
}
```

Developers can use the BiSheng command line to compile **hello\_world.aicpu** and **main.asc** into .o files separately, and then link them into an executable file. The compilation commands are as follows:

-   When compiling **hello\_world.aicpu**, use **-I** to specify the path of the dependency header files; use **--cce-aicpu-laicpu\_api** to link the dependency library **libaicpu\_api.a** for the device, and use **--cce-aicpu-L** to specify the library path of **libaicpu\_api.a**.
-   When compiling **main.asc**, use the **--npu-arch** compilation option to specify the corresponding architecture version number.

Replace *$\{INSTALL\_DIR\}* with the file storage path after the CANN software is installed. For example, when installed as the **root** user, the default file storage path is **/usr/local/Ascend/cann**.

For the architecture version numbers corresponding to each product model, see the [mapping table](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).

```
bisheng -O2 hello_world.aicpu --cce-aicpu-L${INSTALL_DIR}/lib64/device/lib64 --cce-aicpu-laicpu_api -I${INSTALL_DIR}/include/ascendc/aicpu_api -c -o hello_world.aicpu.o
bisheng --npu-arch=dav-2201 main.asc -c -o main.asc.o
bisheng hello_world.aicpu.o main.asc.o -o demo
```

The preceding section uses a getting-started sample to introduce how to compile and generate an executable file through the BiSheng command line. In addition, the BiSheng command line also supports compiling and generating dynamic libraries and static libraries of AI CPU operators. Users can call the kernel function of an AI CPU operator through the kernel call operator <<<...\>\>\> in the asc code, and link the AI CPU dynamic library or static library when compiling the asc code source file to generate an executable file. Note: When compiling AI CPU operator code separately to generate dynamic libraries and static libraries, you need to manually link [Table 2](ai_core_operator_compilation_basic_usage.md#table201231542115513).

-   Compile and generate the operator dynamic library.

    ```
    # Compile test_aicpu.cpp to generate the operator dynamic library.
    # -lxxx indicates the default link library
    # bisheng -shared -x aicpu test_aicpu.cpp -o libtest_aicpu.so -lxxx ...
    ```

-   Compile and generate the operator static library.

    ```
    # Compile test_aicpu.cpp to generate the operator static library.
    # -lxxx indicates the default link library
    # bisheng -lib -x aicpu test_aicpu.cpp -o libtest_aicpu.a -lxxx ...
    ```

## Common Compilation Options for AI CPU Operators<a name="section345885113142"></a>

The common compilation options for AI CPU operators are described as follows:

<a name="table9126181131320"></a>
<table><thead align="left"><tr id="row312711101316"><th class="cellrowborder" valign="top" width="33.63636363636363%" id="mcps1.1.4.1.1"><p id="p71271711201318"><a name="p71271711201318"></a><a name="p71271711201318"></a><strong id="b01279110139"><a name="b01279110139"></a><a name="b01279110139"></a>Option</strong></p></th>
<th class="cellrowborder" valign="top" width="9.676767676767676%" id="mcps1.1.4.1.2"><p id="p1212711115131"><a name="p1212711115131"></a><a name="p1212711115131"></a><strong id="b101271011101310"><a name="b101271011101310"></a><a name="b101271011101310"></a>Required</strong></p></th>
<th class="cellrowborder" valign="top" width="56.686868686868685%" id="mcps1.1.4.1.3"><p id="p8127121151311"><a name="p8127121151311"></a><a name="p8127121151311"></a><strong id="b15127191120134"><a name="b15127191120134"></a><a name="b15127191120134"></a>Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="row8127161113135"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p131279114139"><a name="p131279114139"></a><a name="p131279114139"></a>-help</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p61271711121318"><a name="p61271711121318"></a><a name="p61271711121318"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p14127201181312"><a name="p14127201181312"></a><a name="p14127201181312"></a>Views the help information.</p></td>
</tr>
<tr id="row19128611141312"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p17128511131312"><a name="p17128511131312"></a><a name="p17128511131312"></a>-x</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p13128181141318"><a name="p13128181141318"></a><a name="p13128181141318"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p4128101191320"><a name="p4128101191320"></a><a name="p4128101191320"></a>Specifies the compilation language.</p>
<p id="p21281116136"><a name="p21281116136"></a><a name="p21281116136"></a>When set to aicpu, it indicates the AI CPU operator programming language.</p></td>
</tr>
<tr id="row10128111115130"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p3128191110133"><a name="p3128191110133"></a><a name="p3128191110133"></a>-o &lt;file&gt;</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p181287119131"><a name="p181287119131"></a><a name="p181287119131"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p812861131314"><a name="p812861131314"></a><a name="p812861131314"></a>Specifies the name and location of the output file.</p></td>
</tr>
<tr id="row7128911121316"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p2012821151310"><a name="p2012821151310"></a><a name="p2012821151310"></a>-c</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p11128151116138"><a name="p11128151116138"></a><a name="p11128151116138"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p131281311121315"><a name="p131281311121315"></a><a name="p131281311121315"></a>Compiles and generates the object file.</p></td>
</tr>
<tr id="row15128151111314"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1312831111136"><a name="p1312831111136"></a><a name="p1312831111136"></a>-shared, --shared</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p91289111133"><a name="p91289111133"></a><a name="p91289111133"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p10128171112137"><a name="p10128171112137"></a><a name="p10128171112137"></a>Compiles and generates a dynamic link library.</p></td>
</tr>
<tr id="row512881114134"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1912841161317"><a name="p1912841161317"></a><a name="p1912841161317"></a>-lib</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p212813114130"><a name="p212813114130"></a><a name="p212813114130"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p612811111131"><a name="p612811111131"></a><a name="p612811111131"></a>Compiles and generates a static link library.</p></td>
</tr>
<tr id="row1912891101318"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p81287111130"><a name="p81287111130"></a><a name="p81287111130"></a>-g</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p1212811115139"><a name="p1212811115139"></a><a name="p1212811115139"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p41281811131314"><a name="p41281811131314"></a><a name="p41281811131314"></a>Adds debugging information during compilation.</p></td>
</tr>
<tr id="row1128911201315"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p2128161131317"><a name="p2128161131317"></a><a name="p2128161131317"></a>-fPIC</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p112871121316"><a name="p112871121316"></a><a name="p112871121316"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1912817114131"><a name="p1912817114131"></a><a name="p1912817114131"></a>Instructs the compiler to generate position-independent code.</p></td>
</tr>
<tr id="row3128151113131"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p312831171319"><a name="p312831171319"></a><a name="p312831171319"></a>-O</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14128131114137"><a name="p14128131114137"></a><a name="p14128131114137"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1412851101319"><a name="p1412851101319"></a><a name="p1412851101319"></a>Used to specify the compiler optimization level. Currently, -O3, -O2, and -O0 are supported.</p></td>
</tr>
<tr id="row118491817141416"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1565513583296"><a name="p1565513583296"></a><a name="p1565513583296"></a>--cce-aicpu-L</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p2655195822910"><a name="p2655195822910"></a><a name="p2655195822910"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p365515813296"><a name="p365515813296"></a><a name="p365515813296"></a>Specifies the library path that the AI CPU device depends on.</p></td>
</tr>
<tr id="row49581340171415"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p196884185304"><a name="p196884185304"></a><a name="p196884185304"></a>--cce-aicpu-l</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p17688161833011"><a name="p17688161833011"></a><a name="p17688161833011"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1688918113011"><a name="p1688918113011"></a><a name="p1688918113011"></a>Specifies the library that the AI CPU device depends on.</p></td>
</tr>
</tbody>
</table>

## Compilation Using CMake<a name="section1121825118533"></a>

You can use CMake in a project to compile AI CPU operators with the BiSheng Compiler more conveniently, generating executable files, dynamic libraries, static libraries, or binary files.

Still using the Hello World printing sample described in [Compilation Using the BiSheng Command Line](#section153291123460) as an example, in addition to the code implementation files, you also need to prepare a **CMakeLists.txt** in the project directory.

```
├── hello_world.aicpu // AI CPU operator kernel function definition
├── main.asc // AI CPU operator kernel function call
└── CMakeLists.txt
```

The content of **CMakeLists.txt** is as follows:

```
cmake_minimum_required(VERSION 3.16)
# 1. find_package() is a CMake command used to find and configure the Ascend C compilation toolchain.
find_package(ASC REQUIRED) 
find_package(AICPU REQUIRED) 

# 2. Specify the languages supported by the project, including ASC, AICPU, and CXX. ASC indicates that the BiSheng Compiler is used to compile the Ascend C programming language, and AI CPU indicates that the BiSheng Compiler is used to compile AI CPU operators.
project(kernel_samples LANGUAGES ASC AICPU CXX)

# 3. Use the CMake API to compile the executable file.
add_executable(demo
    hello_world.aicpu
    main.asc
)

#4. Because both ASC and AI CPU languages are present, specify the linker.
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)  # Specify the language used for linking.

target_compile_options(demo PRIVATE
    # --npu-arch is used to specify the NPU architecture version. The value after dav- is the architecture version number.
    # <COMPILE_LANGUAGE:ASC>: indicates that this compilation option takes effect only for the ASC language.
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
)
```

For the architecture version number corresponding to each product model, see the [mapping table](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).

If you need to compile and generate dynamic libraries and static libraries through CMake, more detailed compilation examples are provided below:

-   Compile a .cpp file to generate a dynamic library.

    ```
    # Set the .cpp file to the ASC attribute to enable compilation using the Ascend C language.
    set_source_files_properties(
        add_custom_base.cpp 
        sub_custom_base.cpp
        PROPERTIES LANGUAGE ASC
    )
    
    # Set the .cpp file to the AICPU attribute to support AI CPU operator compilation.
    set_source_files_properties(
        aicpu_kernel.cpp
        PROPERTIES LANGUAGE AICPU
    )
    
    add_library(kernel_lib SHARED
        add_custom_base.cpp 
        sub_custom_base.cpp
        aicpu_kernel.cpp # Support packaging AI CPU operators and AI Core operators together into a dynamic library
    )
    
    target_compile_options(kernel_lib PRIVATE
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
    )
    
    # When compiling AI CPU operators, you need to manually link the following dependency libraries. (If you specify ASC as the link language, you do not need to manually link the following libraries.)
    target_link_libraries(kernel_lib PRIVATE
        ascendc_runtime
        profapi
        unified_dlog
        ascendcl
        runtime
        c_sec
        mmpa
        error_manager
        ascend_dump
    )
    
    add_executable(demo
        main.asc
    )
    target_link_libraries(demo PRIVATE
        kernel_lib
    )
    ```

-   Compile .asc and .aicpu files to generate a static library.

    ```
    # .asc files are compiled using Ascend C by default, and .aicpu files are compiled using AICPU by default. You do not need to configure them through set_source_files_properties.
    add_library(kernel_lib STATIC
        add_custom_base.asc 
        sub_custom_base.asc
        aicpu_kernel.aicpu  # AI CPU operators and AI Core operators can be packaged together into a static library.
    )
    
    target_compile_options(kernel_lib PRIVATE
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
    )
    
    add_executable(demo
        main.asc
    )
    target_link_libraries(demo PRIVATE
        kernel_lib
    )
    ```
