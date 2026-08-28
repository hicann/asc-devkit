# AI CPU算子编译<a name="ZH-CN_TOPIC_0000002522571023"></a>

与AI Core算子只需一个`.asc`文件即可编译生成可执行文件不同，AI CPU算子需要同时包含`.aicpu`文件（核函数（Kernel）定义）和`.asc`文件（通过内核调用符调用核函数（Kernel））才能编译生成可执行文件。

## 通过bisheng命令行编译<a name="section153291123460"></a>

下文基于一个Hello World打印样例来讲解如何通过bisheng命令行编译AI CPU算子。该样例包含`hello_world.aicpu`文件（AI CPU核函数（Kernel）定义）和`main.asc`文件（通过内核调用符调用AI CPU核函数（Kernel））。

hello\_world.aicpu文件内容如下：

```
#include "aicpu_api.h"

__global__ __aicpu__ uint32_t hello_world(void *args)
{
    AscendC::printf("Hello World!!!\n");
    return 0;
}
```

Host侧使用内核调用符<<<...\>\>\>进行AI CPU算子的调用， main.asc示例代码如下：

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

开发者可以使用bisheng命令行将hello\_world.aicpu与main.asc分别编译成.o，再链接成为可执行文件，编译命令如下：

-   编译hello\_world.aicpu时，通过-I指定依赖头文件所在路径；通过--cce-aicpu-laicpu\_api为Device链接依赖的库libaicpu\_api.a，通过--cce-aicpu-L指定libaicpu\_api.a的库路径；通过--cce-aicpu-toolkit-path指定AI CPU编译工具链路径；通过--cce-aicpu-sysroot指定AI CPU编译所使用的系统根目录。
-   编译main.asc时，通过--npu-arch编译选项指定对应的架构版本号。

`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

各产品型号对应的架构版本号请通过[对应关系表](../../language_extension/simd_builtin_keywords.md#npu-arch)进行查询。

```
bisheng -O2 hello_world.aicpu --cce-aicpu-L${INSTALL_DIR}/lib64/device/lib64 --cce-aicpu-laicpu_api --cce-aicpu-toolkit-path=${INSTALL_DIR}/toolkit/toolchain/hcc/bin --cce-aicpu-sysroot=${INSTALL_DIR}/toolkit/toolchain/hcc/sysroot -I${INSTALL_DIR}/asc/include/aicpu_api -c -o hello_world.aicpu.o
bisheng --npu-arch=dav-2201 main.asc -c -o main.asc.o
bisheng hello_world.aicpu.o main.asc.o -o demo
```

上文我们通过一个入门示例介绍了使用bisheng命令行编译生成可执行文件的示例。除此之外，使用bisheng命令行也支持编译生成AI CPU算子的动态库与静态库，用户可在asc代码中通过内核调用符<<<...\>\>\>调用AI CPU算子的核函数（Kernel），并在编译asc代码源文件生成可执行文件的时候，链接AI CPU动态库或者静态库，注意：若单独编译AI CPU算子代码生成动态库、静态库时，需要手动链接[表格中的库文件](ai_core_operator_compilation.md#内置链接库)。

-   编译生成算子动态库

    ```
    # 编译test_aicpu.cpp生成算子动态库
    # -lxxx表示默认链接库
    # bisheng -shared -x aicpu test_aicpu.cpp -o libtest_aicpu.so -lxxx ...
    ```

-   编译生成算子静态库

    ```
    # 编译test_aicpu.cpp生成算子静态库
    # -lxxx表示默认链接库
    # bisheng -lib -x aicpu test_aicpu.cpp -o libtest_aicpu.a -lxxx ...
    ```

## AI CPU算子常用编译选项<a name="section345885113142"></a>

AI CPU算子常用的编译选项说明如下：

<a name="table9126181131320"></a>
<table><thead align="left"><tr id="row312711101316"><th class="cellrowborder" valign="top" width="33.63636363636363%" id="mcps1.1.4.1.1"><p id="p71271711201318"><a name="p71271711201318"></a><a name="p71271711201318"></a><strong id="b01279110139"><a name="b01279110139"></a><a name="b01279110139"></a>选项</strong></p>
</th>
<th class="cellrowborder" valign="top" width="9.676767676767676%" id="mcps1.1.4.1.2"><p id="p1212711115131"><a name="p1212711115131"></a><a name="p1212711115131"></a><strong id="b101271011101310"><a name="b101271011101310"></a><a name="b101271011101310"></a>是否必需</strong></p>
</th>
<th class="cellrowborder" valign="top" width="56.686868686868685%" id="mcps1.1.4.1.3"><p id="p8127121151311"><a name="p8127121151311"></a><a name="p8127121151311"></a><strong id="b15127191120134"><a name="b15127191120134"></a><a name="b15127191120134"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row8127161113135"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p131279114139"><a name="p131279114139"></a><a name="p131279114139"></a>-help</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p61271711121318"><a name="p61271711121318"></a><a name="p61271711121318"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p14127201181312"><a name="p14127201181312"></a><a name="p14127201181312"></a>查看帮助。</p>
</td>
</tr>
<tr id="row19128611141312"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p17128511131312"><a name="p17128511131312"></a><a name="p17128511131312"></a>-x</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p13128181141318"><a name="p13128181141318"></a><a name="p13128181141318"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p4128101191320"><a name="p4128101191320"></a><a name="p4128101191320"></a>指定编译语言。</p>
<p id="p21281116136"><a name="p21281116136"></a><a name="p21281116136"></a>指定为aicpu时表示AI CPU算子编程语言。</p>
</td>
</tr>
<tr id="row10128111115130"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p3128191110133"><a name="p3128191110133"></a><a name="p3128191110133"></a>-o &lt;file&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p181287119131"><a name="p181287119131"></a><a name="p181287119131"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p812861131314"><a name="p812861131314"></a><a name="p812861131314"></a>指定输出文件的名称和位置。</p>
</td>
</tr>
<tr id="row7128911121316"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p2012821151310"><a name="p2012821151310"></a><a name="p2012821151310"></a>-c</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p11128151116138"><a name="p11128151116138"></a><a name="p11128151116138"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p131281311121315"><a name="p131281311121315"></a><a name="p131281311121315"></a>编译生成目标文件。</p>
</td>
</tr>
<tr id="row15128151111314"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1312831111136"><a name="p1312831111136"></a><a name="p1312831111136"></a>-shared，--shared</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p91289111133"><a name="p91289111133"></a><a name="p91289111133"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p10128171112137"><a name="p10128171112137"></a><a name="p10128171112137"></a>编译生成动态链接库。</p>
</td>
</tr>
<tr id="row512881114134"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1912841161317"><a name="p1912841161317"></a><a name="p1912841161317"></a>-lib</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p212813114130"><a name="p212813114130"></a><a name="p212813114130"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p612811111131"><a name="p612811111131"></a><a name="p612811111131"></a>编译生成静态链接库。</p>
</td>
</tr>
<tr id="row1912891101318"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p81287111130"><a name="p81287111130"></a><a name="p81287111130"></a>-g</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p1212811115139"><a name="p1212811115139"></a><a name="p1212811115139"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p41281811131314"><a name="p41281811131314"></a><a name="p41281811131314"></a>编译时增加调试信息。</p>
</td>
</tr>
<tr id="row1128911201315"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p2128161131317"><a name="p2128161131317"></a><a name="p2128161131317"></a>-fPIC</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p112871121316"><a name="p112871121316"></a><a name="p112871121316"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1912817114131"><a name="p1912817114131"></a><a name="p1912817114131"></a>告知编译器产生位置无关代码。</p>
</td>
</tr>
<tr id="row3128151113131"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p312831171319"><a name="p312831171319"></a><a name="p312831171319"></a>-O</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14128131114137"><a name="p14128131114137"></a><a name="p14128131114137"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1412851101319"><a name="p1412851101319"></a><a name="p1412851101319"></a>用于指定编译器的优化级别，当前支持-O3，-O2，-O0。</p>
</td>
</tr>
<tr id="row118491817141416"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1565513583296"><a name="p1565513583296"></a><a name="p1565513583296"></a>--cce-aicpu-L</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p2655195822910"><a name="p2655195822910"></a><a name="p2655195822910"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p365515813296"><a name="p365515813296"></a><a name="p365515813296"></a>指定AI CPU Device依赖的库路径。</p>
</td>
</tr>
<tr id="row49581340171415"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p196884185304"><a name="p196884185304"></a><a name="p196884185304"></a>--cce-aicpu-l</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p17688161833011"><a name="p17688161833011"></a><a name="p17688161833011"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1688918113011"><a name="p1688918113011"></a><a name="p1688918113011"></a>指定AI CPU Device依赖的库。</p>
</td>
</tr>
</tbody>
</table>

## CMake方式编译<a name="section1121825118533"></a>

项目中可以使用CMake来更简便地使用毕昇编译器编译AI CPU算子，生成可执行文件、动态库、静态库或二进制文件。

仍以[通过bisheng命令行编译](#section153291123460)中介绍的Hello World打印样例为例，除了代码实现文件，还需要在工程目录下准备一个CMakeLists.txt。

```
├── hello_world.aicpu // AI CPU算子核函数（Kernel）定义
├── main.asc // AI CPU算子核函数（Kernel）调用
└── CMakeLists.txt
```

CMakeLists.txt内容如下：

```
cmake_minimum_required(VERSION 3.16)
# 1、find_package()是CMake中用于查找和配置Ascend C编译工具链的命令
find_package(ASC REQUIRED) 
find_package(AICPU REQUIRED) 

# 2、指定项目支持的语言包括ASC、AICPU和CXX，ASC表示支持使用毕昇编译器对Ascend C编程语言进行编译，AI CPU表示支持使用毕昇编译器对AI CPU算子进行编译
project(kernel_samples LANGUAGES ASC AICPU CXX)

# 3、使用CMake接口编译可执行文件
add_executable(demo
    hello_world.aicpu
    main.asc
)

#4、由于存在ASC与AI CPU语言，需要指定链接器
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)  # 指定链接使用语言

target_compile_options(demo PRIVATE
    # --npu-arch用于指定NPU的架构版本，dav-后为架构版本号
    # <COMPILE_LANGUAGE:ASC>:表明该编译选项仅对语言ASC生效
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
)
```

各产品型号对应的架构版本号请通过[对应关系表](../../language_extension/simd_builtin_keywords.md#npu-arch)进行查询。

如果需要CMake编译编译生成动态库、静态库，下面提供了更详细具体的编译示例：

-   编译.cpp文件生成动态库

    ```
    # 将.cpp文件置为ASC属性，启用Ascend C语言进行编译
    set_source_files_properties(
        add_custom_base.cpp 
        sub_custom_base.cpp
        PROPERTIES LANGUAGE ASC
    )
    
    # 将.cpp文件置为AICPU属性，支持AI CPU算子编译
    set_source_files_properties(
        aicpu_kernel.cpp
        PROPERTIES LANGUAGE AICPU
    )
    
    add_library(kernel_lib SHARED
        add_custom_base.cpp 
        sub_custom_base.cpp
        aicpu_kernel.cpp # 支持AI CPU算子与AI Core算子一起打包为动态库
    )
    
    target_compile_options(kernel_lib PRIVATE
        $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=dav-2201>
    )
    
    # AI CPU算子编译时，需要手动链接以下依赖库（若指定链接语言为ASC时，不需要手动链接以下库）
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

-   编译.asc文件与.aicpu文件生成静态库

    ```
    # .asc文件会默认启用Ascend C语言进行编译，.aicpu文件会默认启用AICPU语言进行编译，不需要通过set_source_files_properties进行设置
    add_library(kernel_lib STATIC
        add_custom_base.asc 
        sub_custom_base.asc
        aicpu_kernel.aicpu  # 可支持AI CPU算子与AI Core算子一起打包为静态库
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
