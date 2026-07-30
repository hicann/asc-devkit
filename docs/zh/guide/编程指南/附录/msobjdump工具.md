# msobjdump工具<a name="ZH-CN_TOPIC_0000002028951292"></a>

本工具主要针对工程编译生成的算子ELF文件（Executable and Linkable Format）提供解析和解压功能，并将结果信息以可读形式呈现，方便开发者直观获得kernel文件信息。当前支持解析融合编译工程、标准/简易自定义算子工程，以及Kernel直调工程生成的相关产物。

>[!NOTE]说明 
>- ELF文件是一种用于二进制文件、可执行文件、目标代码、共享库和核心转储的文件格式，包括常见的\*.a、\*.so文件等。ELF文件常见构成如下：
>    - ELF头部：描述了整个文件的组织结构，包括文件类型、机器类型、版本号等信息。
>    - 程序头部表：描述了文件中各种段（segments）信息，包括程序如何加载到内存中执行的信息。
>    - 节区头部表：描述了文件中各个节（sections）信息，包括程序的代码、数据、符号表等。
>- 工具使用过程中，若出现如下场景，请根据日志提示信息，分析排查问题。
>    - ELF文件未找到
>    - ELF文件权限错误
>    - ELF文件存在但不支持解析或解压

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品：支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：支持
<!-- end id6 -->

## 工具安装<a name="section2423188195113"></a>

1.  安装msobjdump工具。

    工具跟随CANN软件包发布（参考[环境准备](../../入门教程/环境准备.md)完成CANN安装），其路径默认为`${INSTALL_DIR}/tools/msobjdump`，其中`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

2.  设置环境变量。
    -   root用户安装Ascend-cann-toolkit包时

        ```
        source /usr/local/Ascend/cann/set_env.sh
        ```

    -   非root用户安装Ascend-cann-toolkit包时

        ```
        source ${HOME}/Ascend/cann/set_env.sh
        ```

3.  检查工具是否安装成功。

    执行如下命令，若能正常显示--help或-h信息，则表示工具环境正常，功能可正常使用。

    ```
    msobjdump -h
    ```

## 命令格式<a name="section101202511916"></a>

-   **解析ELF文件的命令**

    ```
    msobjdump --dump-elf <elf_file> [--verbose]
    ```

    **表1**  参数说明<a name="table167911947163519"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `--dump-elf <elf_file>`，`-d` | 必选 | 解析ELF文件中包含的device信息，如文件名、文件类型、文件长度、符号表等，并终端打屏显示。<br>`<elf_file>`表示待解析ELF文件路径，如`/home/op_api/lib_api.so`。支持两种打印模式：<br>简单打印：默认仅打印部分device信息。<br>全量打印：与`--verbose`配套使用，开启全量device信息打屏显示。<br>不同工程打印字段信息不同，具体参见<a href="#table94384560259">表4</a>和<a href="#table217334916136">表6</a>。 |
    | `--verbose`，`-V` | 可选 | 必须与`--dump-elf`配套使用，用于开启ELF文件中全量打印device信息功能。 |

-   **解压ELF文件的命令**

    ```
    msobjdump --extract-elf <elf_file> [--out-dir <out_path>]
    ```

    **表2**  参数说明<a name="table131531242133819"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `--extract-elf <elf_file>`，`-e` | 必选 | 解压ELF文件中包含的device信息，并按解析结果落盘到输出路径下。<br>`<elf_file>`表示待解压ELF文件路径，如`/home/op_api/lib_api.so`。<br>默认路径：解压结果文件默认落盘到当前执行路径下。<br>自定义路径：可与`--out-dir`配套使用，设置落盘路径。 |
    | `--out-dir <out_path>`，`-o` | 可选 | 必须与`--extract-elf`配套使用，用于设置解压文件的落盘路径。<br>`<out_path>`为落盘文件目录，如`/home/extract/`。<br>**请注意**：`msobjdump`支持多用户并发调用，但需要指定不同的`--out-dir`，否则可能出现落盘内容被覆盖的问题。 |

-   **获取ELF文件列表的命令**

    ```
    msobjdump --list-elf <elf_file>
    ```

    **表3**  参数说明

    <a name="table121952819427"></a>
    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `--list-elf <elf_file>`，`-l` | 必选 | 获取ELF文件中包含的device信息文件列表，并打印显示。<br>`<elf_file>`表示待打印的ELF文件路径，如`/home/op_api/lib_api.so`。 |

**表4**  ELF解析字段说明（标准/简易自定义算子工程及融合编译工程）<a name="table94384560259"></a>

| 字段名 | 含义 | 是否必选 | 打印说明 |
| ---- | ---- | ---- | ---- |
| `.ascend.meta. ${id}` | 表示算子kernel函数名称，其中`${id}`表示meta信息的索引值。 | 是 | 不设置`--verbose`，默认打印。 |
| `VERSION` | 表示版本号。 | 是 | 不设置`--verbose`，默认打印。 |
| `DEBUG` | 调试相关信息，包含如下两部分内容：<br>`debugBufSize`：调试信息需要的内存空间。<br>`debugOptions`：调试开关状态。取值如下：<br>`0`：调试开关关闭。<br>`1`：通过DumpTensor、printf打印进行调试。<br>`2`：通过assert断言进行调试。<br>`4`：通过时间戳打点功能进行调试。<br>`8`：通过内存越界检测进行调试。 | 否 | 不设置`--verbose`，默认打印。 |
| `DYNAMIC_PARAM` | 算子kernel函数是否启用动态参数。取值分别为：<br>`0`：关闭动态参数模式。<br>`1`：开启动态参数模式。 | 否 | 不设置`--verbose`，默认打印。 |
| `OPTIONAL_PARAM` | 可选参数信息，包含如下两部分内容：<br>`optionalInputMode`：可选输入在算子kernel函数中是否需要占位。<br>`0`：可选输入不占位。<br>`1`：可选输入占位。<br>`optionalOutputMode`：可选输出在算子kernel函数中是否需要占位。<br>`0`：可选输出不占位。<br>`1`：可选输出占位。 | 否 | 不设置`--verbose`，默认打印。 |
| `RUNTIME_IMPLICIT_INFO` | 表示运行时隐式信息标志。取值如下：<br>`1`：`SIMD Printf Flag`，表示SIMD侧Printf标志。<br>`2`：`Hardware Sync Flag`，表示硬同步标志。<br>`3`：`L2Cache Hint Flag`，表示L2 Cache命中标志。<br>`4`：`SIMT Printf Flag`，表示SIMT侧Printf标志。<br>`5`：`SIMD Assert Flag`，表示SIMD侧Assert标志。<br>其他取值打印原始数值。 | 否 | 不设置`--verbose`，默认打印。 |
| `KERNEL_TYPE` | 表示kernel函数运行时core类型，取值参见<a href="#table187419221164">表5</a>。 | 否 | 不设置`--verbose`，默认打印。 |
| `CROSS_CORE_SYNC` | 表示硬同步syncall类型。<br>`USE_SYNC`：使用硬同步。<br>`NO_USE_SYNC`：不使用硬同步。 | 否 | 不设置`--verbose`，默认打印。 |
| `MIX_TASK_RATION` | 表示kernel函数运行时的Cube核/Vector核占比分配类型。 | 否 | 不设置`--verbose`，默认打印。 |
| `DETERMINISTIC_INFO` | 表示算子是否为确定性计算。<br>`0`：不确定计算。<br>`1`：确定性计算。 | 否 | 不设置`--verbose`，默认打印。 |
| `BLOCK_NUM` | 表示算子执行核数，该字段当前暂不支持，只打印默认值`0xFFFFFFFF`。 | 否 | 不设置`--verbose`，默认打印。 |
| `FUNCTION_ENTRY` | 算子TilingKey的值。 | 否 | 不设置`--verbose`，默认打印。 |
| `elf header infos` | 包括ELF Header、Section Headers、Key to Flags、Program Headers、Symbol表等信息。 | 否 | 设置`--verbose`，开启全量打印。 |

**表5**  kernel type信息

<a name="table187419221164"></a>
| KERNEL_TYPE | 说明 |
| ---- | ---- |
| `AICORE` | **该参数为预留参数，当前版本暂不支持。**<br>算子执行时仅会启动AI Core，比如用户在host侧设置blocknum为5，则会启动5个AI Core。 |
| `AIC` | 算子执行时仅启动AI Core上的Cube核，比如用户在host侧设置blocknum为10，则会启动10个Cube核。 |
| `AIV` | 算子执行时仅启动AI Core上的Vector核，比如用户在host侧设置blocknum为10，则会启动10个Vector核。 |
| `MIX_AIC_MAIN` | AIC、AIV混合场景下，设置核函数的类型为MIX，算子执行时会同时启动AI Core上的Cube核和Vector核，比如用户在host侧设置blocknum为10，且设置task_ration为1：2，则会启动10个Cube核和20个Vector核。 |
| `MIX_AIV_MAIN` | AIC、AIV混合场景下，使用了多核控制相关指令时，设置核函数的类型为MIX，算子执行时会同时启动AI Core上的Cube核和Vector核，比如用户在host侧设置blocknum为10，且设置task_ration为1：2，则会启动10个Vector核和20个Cube核。 |
| `AIC_ROLLBACK` | 算子执行时会同时启动AI Core和Vector Core，此时AI Core会当成Cube Core使用。 |
| `AIV_ROLLBACK` | 算子执行时会同时启动AI Core和Vector Core，此时AI Core会当成Vector Core使用。 |


**表6**  ELF解析字段说明（Kernel直调工程）

<a name="table217334916136"></a>
| 字段名 | 含义 | 是否必选 | 打印说明 |
| ---- | ---- | ---- | ---- |
| `VERSION` | 表示版本号。 | 是 | 不设置`--verbose`，默认打印。 |
| `TYPE COUNT` | 表示ELF文件中包含的kernel文件个数。 | 是 | 不设置`--verbose`，默认打印。 |
| `ELF FILE ${id}` | 表示ELF文件中包含的kernel文件名，`${id}`表示kernel文件序号。<br>kernel文件名的命名规则如下：<br>按`${sec_prefix}_${file_index}_${kernel_type}.o`拼接，其中`${sec_prefix}`为section段名（工具根据`.ascend.kernel`关键字搜索获取），`${file_index}`表示文件编号，`${kernel_type}`表示kernel类型。 | 是 | 不设置`--verbose`，默认打印。 |
| `KERNEL LEN` | 表示kernel文件的长度。 | 是 | 不设置`--verbose`，默认打印。 |
| `KERNEL TYPE` | 表示kernel类型，映射关系为`{0: mix, 1: aiv, 2: aic}`。 | 否 | 不设置`--verbose`，默认打印。 |
| `ASCEND META` | 表示算子执行时核间同步、Cube/Vector核占比（task_ration）等信息。<br>若没有获取到该信息，默认显示`None`。 | 否 | 不设置`--verbose`，默认打印。 |
| `elf header infos` | 包括ELF Header、Section Headers、Key to Flags、Program Headers、Symbol表等信息。 | 否 | 设置`--verbose`，开启全量打印。 |

## 使用样例（融合编译工程）<a name="section_fusion_compile_msobjdump"></a>

以融合编译工程生成的可执行文件为例，假设`${build_dir}`为工程构建目录，编译生成的可执行文件名为`demo`。对于该类产物，工具会基于可执行文件中的device相关信息进行解析，其中`--dump-elf`用于展示binary meta与function meta信息，`--list-elf`用于查看可提取的device文件名，`--extract-elf`用于将解析出的device文件落盘。调用样例可参考[msobjdump样例](https://gitcode.com/cann/asc-tools/tree/master/examples/04_msobjdump)。

-   **解析融合编译工程产物**

    支持两种打印方式，请按需选取，解析字段含义参见[表4](#table94384560259)。

    -   简单打印

        ```
        msobjdump --dump-elf ${build_dir}/demo
        ```

        执行上述命令，终端打印基础device信息，示例如下：

        ```
        .ascend.meta META INFO
        RUNTIME_IMPLICIT_INFO: L2Cache Hint Flag
        RUNTIME_IMPLICIT_INFO: Hardware Sync Flag
        VERSION: 1
        RUNTIME_IMPLICIT_INFO: SIMD Printf Flag
        .ascend.meta. [0]: _Z23matmul_leakyrelu_customPhS_S_S_S_N7AscendC6tiling11TCubeTilingE_mix_aic
        KERNEL_TYPE: MIX_AIC_MAIN
        CROSS_CORE_SYNC: USE_SYNC
        MIX_TASK_RATION: [1:2]
        .ascend.meta. [1]: _Z23matmul_leakyrelu_customPhS_S_S_S_N7AscendC6tiling11TCubeTilingE_mix_aiv
        KERNEL_TYPE: MIX_AIC_MAIN
        CROSS_CORE_SYNC: USE_SYNC
        MIX_TASK_RATION: [1:2]
        ```

    -   全量打印

        ```
        msobjdump --dump-elf ${build_dir}/demo --verbose
        ```

        执行上述命令，除基础device信息外，还会打印提取出的device文件对应的ELF详细信息，示例如下：
        ```
        ====== [elf header infos] ======
        ELF Header:
          Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
          Class:                             ELF64
          Data:                              2's complement, little endian
          Version:                           1 (current)
          OS/ABI:                            UNIX - System V
          ABI Version:                       0
          Type:                              EXEC (Executable file)
          Machine:                           <unknown>: 0x1029
          Version:                           0x1
          Entry point address:               0x0
          Start of program headers:          64 (bytes into file)
          Start of section headers:          33504 (bytes into file)
          Flags:                             0x940000
          Size of this header:               64 (bytes)
          Size of program headers:           56 (bytes)
          Number of program headers:         3
          Size of section headers:           64 (bytes)
          Number of section headers:         16
          Section header string table index: 14
    
        Section Headers:
          [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
          [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
          [ 1] .text             PROGBITS        0000000000000000 0000e8 006c94 00  AX  0   0  4
          ......................................................................................
          [15] .strtab           STRTAB          0000000000000000 007ce0 0005fc 00      0   0  1
        Key to Flags:
          W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
          L (link order), O (extra OS processing required), G (group), T (TLS),
          C (compressed), x (unknown), o (OS specific), E (exclude),
          D (mbind), p (processor specific)
    
        There are no section groups in this file.
    
        Program Headers:
          Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
          LOAD           0x0000e8 0x0000000000000000 0x0000000000000000 0x006ca7 0x006ca7 R E 0x1000
          LOAD           0x0070e8 0x0000000000007000 0x0000000000007000 0x000210 0x000210 RW  0x1000
          GNU_STACK      0x000000 0x0000000000000000 0x0000000000000000 0x000000 0x000000 RW  0
    
        ......
        ```

-   **获取融合编译工程产物中的device文件列表**

    ```
    msobjdump --list-elf ${build_dir}/demo
    ```

    执行上述命令，终端会打印可提取的device文件名，屏显信息形如：

    ```
    ELF file    0: demo.aicore.o
    ```

-   **解压融合编译工程产物中的device文件并落盘**

    ```
    msobjdump --extract-elf ${build_dir}/demo
    ```

    执行上述命令，默认在当前执行路径下落盘`demo.aicore.o`文件。若需要指定输出路径，可配合`--out-dir`使用。

## 使用样例（标准/简易自定义算子工程）<a name="section12835815105114"></a>

以下面的算子工程为例（仅为示例，具体以实际算子工程为准），假设$\{cmake\_install\_dir\}为算子Cmake编译产物根目录，目录结构如下，类似[算子编译](../高级编程/Aclnn算子工程化开发/编译与部署/基本流程.md#部署后的目录结构)。

```
├── op_api
│   ├── include
│       ├── aclnn_acos_custom.h
│       ├── aclnn_matmul_leakyrelu_custom.h
│       ├── .........
│   ├── lib
│       ├── libcust_opapi.so
```

工具对编译生成的库文件（如\*.so、\*.a等）进行解析和解压，功能实现命令样例如下：

-   **解析包含device信息的库文件**

    支持两种打印方式，请按需选取，解析字段含义参见[表4](#table94384560259)。

    -   简单打印

        ```
        msobjdump --dump-elf ${cmake_install_dir}/op_api/lib/libcust_opapi.so 
        ```

        执行上述命令，终端打印基础device信息，示例如下：

        ```
        .ascend.meta META INFO
        VERSION: 1
        DEBUG: debugBufSize=0, debugOptions=0
        DYNAMIC_PARAM: dynamicParamMode=0
        OPTIONAL_PARAM: optionalInputMode=1, optionalOutputMode=1
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_1
        KERNEL_TYPE: AIV
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 1
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_2_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 2
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_3_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 3
        ....................................
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_1
        KERNEL_TYPE: AIV
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 1
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_2_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 2
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_3_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 3
        ```

    -   全量打印

        ```
        msobjdump --dump-elf ${cmake_install_dir}/op_api/lib/libcust_opapi.so --verbose
        ```

        执行上述命令，终端打印基础device信息，示例如下：

        ```
        .ascend.meta META INFO
        VERSION: 1
        DEBUG: debugBufSize=0, debugOptions=0
        DYNAMIC_PARAM: dynamicParamMode=0
        OPTIONAL_PARAM: optionalInputMode=1, optionalOutputMode=1
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_1
        KERNEL_TYPE: AIV
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 1
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_2_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 2
        .ascend.meta. [0]: AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26_3_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 3
        ....................................
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_1
        KERNEL_TYPE: AIV
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 1
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_2_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 2
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_3_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        DETERMINISTIC_INFO: 1
        BLOCK_NUM: 0xFFFFFFFF
        FUNCTION_ENTRY: 3
        ```

        ```
        ....................................
        ===== [elf header infos] in ascendxxx_acos_custom_AcosCustom_da824ede53d7e754f85c14b9446ec2fc.o =====:
        ELF Header:
          Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
          Class:                             ELF64
          Data:                              2's complement, little endian
          Version:                           1 (current)
          OS/ABI:                            UNIX - System V
          ................................................
          Size of program headers:           56 (bytes)
          Number of program headers:         3
          Size of section headers:           64 (bytes)
          Number of section headers:         9
          Section header string table index: 7
        Section Headers:
          [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
          [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0 
           .....................................................................................
          [ 8] .strtab           STRTAB          0000000000000000 00529b 000119 00      0   0  1
        Key to Flags:
          W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
          L (link order), O (extra OS processing required), G (group), T (TLS),
          C (compressed), x (unknown), o (OS specific), E (exclude),
          D (mbind), p (processor specific)
        ................................................
        
        ===== [elf header infos] in ascendxxx_matmul_leakyrelu_custom_MatmulLeakyreluCustom_e052bee3255764ac919095f3bdf83389.o =====:
        ELF Header:
          Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
          Class:                             ELF64
          Data:                              2's complement, little endian
          Version:                           1 (current)
          ................................................
          Section header string table index: 6
        Section Headers:
          [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
          [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
          [ 1] .text             PROGBITS        0000000000000000 0000e8 007ed8 00  AX  0   0  4
          [ 2] .data             PROGBITS        0000000000008000 0080e8 000008 00  WA  0   0 256
          [ 3] .comment          PROGBITS        0000000000000000 0080f0 000043 01  MS  0   0  1
          [ 4] .bl_uninit        NOBITS          0000000000000000 008133 000020 00      0   0  1
          [ 5] .symtab           SYMTAB          0000000000000000 008138 0000c0 18      7   1  8
          [ 6] .shstrtab         STRTAB          0000000000000000 0081f8 00003b 00      0   0  1
          [ 7] .strtab           STRTAB          0000000000000000 008233 0000ec 00      0   0  1
          ................................................
        ```

-   **解压包含device信息的库文件并落盘**

    ```
    msobjdump --extract-elf ${cmake_install_dir}/op_api/lib/libcust_opapi.so 
    ```

    执行上述命令，默认在当前执行路径下保存解压文件，产物目录如下：

    ```
    |-- config                                                               // 算子原型配置文件目录
    |    ├── ${soc_version}   
    |        ├── acos_custom.json                                  
    |        ├── matmul_leakyrelu_custom.json                               
    |        ├── .......                                             
    |-- ${soc_version}                                                    // AI处理器名
    |     ├── acos_custom                                               // 基础单算子编译文件*.o和对应的*.json文件
    |         ├── AcosCustom_da824ede53d7e754f85c14b9446ec2fc.json      // 命名规则：${op_type}_${parm_info}.json或${op_type}_${parm_info}.o，${parm_info}是基于算子输入/输出dtype、shape等信息生成的标识码
    |         ├── AcosCustom_da824ede53d7e754f85c14b9446ec2fc.o
    |         ├── AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26.json
    |         ├── AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26.o
    |     ├── matmul_leakyrelu_custom  
    |         ├── MatmulLeakyreluCustom_e052bee3255764ac919095f3bdf83389.json
    |         ├── MatmulLeakyreluCustom_e052bee3255764ac919095f3bdf83389.o
    |     ├── axpy_custom    
    |         ├── .....
    ```

    以acos\_custom算子编译产物解压为例：

    -   查看算子原型（acos\_custom.json）

        ```
        {
            "binList": [
                {
                    "implMode": "high_performance",
                    "int64Mode": false,
                    "simplifiedKeyMode": 0,
                    "simplifiedKey": [......],
                    "staticKey": "96b2b4bb2e3xxx,ee37ce8796ef139dexxxx",
                    "inputs": [
                        {
                            "name": "x",
                            "index": 0,
                            "dtype": "float32",
                            "format": "ND",
                            "paramType": "required",
                            "shape": [
                                -2
                            ],
                            "format_match_mode": "FormatAgnostic"
                        }
                    ],
                    "outputs": [
                        {
                            "name": "y",
                            "index": 0,
                            "dtype": "float32",
                            "format": "ND",
                            "paramType": "required",
                            "shape": [
                                -2
                            ],
                            "format_match_mode": "FormatAgnostic"
                        }
                    ],
                    "attrs": [
                        {
                            "name": "tmp",
                            "dtype": "int",
                            "value": 0
                        },
                        .........
                    ],
                    "opMode": "dynamic",
                    "optionalInputMode": "gen_placeholder",
                    "deterministic": "ignore",
                    "binInfo": {
                        "jsonFilePath": "ascendxxx/acos_custom/AcosCustom_da824ede53d7e754f85c14b9446ec2fc.json"
                    }
                },
                {
                    "implMode": "high_performance",
                    "int64Mode": false,
                    "simplifiedKeyMode": 0,
                    "simplifiedKey": [
          
                    ],
                    "staticKey": "27d6f997f2f3551axxxx,1385590c47affa578eb429xxx",
                    "inputs": [
                        {
                            "name": "x",
                            "index": 0,
                            "dtype": "float16",
                            "format": "ND",
                            "paramType": "required",
                            "shape": [
                                -2
                            ],
                            "format_match_mode": "FormatAgnostic"
                        }
                    ],
                    "outputs": [
                        {
                            "name": "y",
                            "index": 0,
                            "dtype": "float16",
                            "format": "ND",
                            "paramType": "required",
                            "shape": [
                                -2
                            ],
                            "format_match_mode": "FormatAgnostic"
                        }
                    ],
                    "attrs": [
                        {
                            "name": "tmp",
                            "dtype": "int",
                            "value": 0
                        },
                        .........
                    ],
                    "opMode": "dynamic",
                    "optionalInputMode": "gen_placeholder",
                    "deterministic": "ignore",
                    "binInfo": {
                        "jsonFilePath": "ascendxxx/acos_custom/AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26.json"
                    }
                }
            ]
        }
        ```

    -   解析$\{op\_type\}\_$\{parm\_info\}.o文件获取.ascend.meta段信息。

        ```
        msobjdump --dump-elf ./AcosCustom_da824ede53d7e754f85c14b9446ec2fc.o
        ```

        执行上述命令，终端屏显如下，字段与库文件解析类似，参见[表4](#table94384560259)。

        ```
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_1
        KERNEL_TYPE: AIV
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_2_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        .ascend.meta. [0]: AcosCustom_da824ede53d7e754f85c14b9446ec2fc_3_mix_aiv
        KERNEL_TYPE: MIX_AIV_MAIN
        MIX_TASK_RATION: [0:1]
        ```

    -   查看$\{op\_type\}\_$\{parm\_info\}.json，直观获取device文件中算子信息。

        ```
        {
            "binFileName": "AcosCustom_da824ede53d7e754f85c14b9446ec2fc",
            "binFileSuffix": ".o",
            "blockDim": -1,
            "coreType": "MIX",
            "intercoreSync": 1,
            "kernelName": "AcosCustom_da824ede53d7e754f85c14b9446ec2fc",
            "magic": "RT_DEV_BINARY_MAGIC_ELF",
            "memoryStamping": [],
            "opParaSize": 24,
            "parameters": [],
            "sha256": "94e32d04fcaf435411xxxxxxxx",
            "workspace": {
                "num": 1,
                "size": [
                    -1
                ],
                "type": [
                    0
                ]
            },
            "kernelList": [
                {
                    "tilingKey": 1,
                    "kernelType": "MIX_AIC",
                    "taskRation": "0:1",
                    "crossCoreSync": 0,
                    "kernelName": "AcosCustom_da824ede53d7e754f85c14b9446ec2fc_1"
                },
                .........
            ],
            "taskRation": "tilingKey",
            "optionalInputMode": "gen_placeholder",
            "debugOptions": "printf",
            "debugBufSize": 78643200,
            "compileInfo": {},
            "supportInfo": {                                                        // 算子原型信息
                "implMode": "high_performance",
                "int64Mode": false,
                "simplifiedKeyMode": 0,
                "simplifiedKey": [......],
                "staticKey": "96b2b4bb2e35fa3dxxx,ee37ce8796ef139dedxxxxxxxx",
                "inputs": [
                    {
                        "name": "x",
                        "index": 0,
                        "dtype": "float32",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [
                            -2
                        ],
                        "format_match_mode": "FormatAgnostic"
                    }
                ],
                "outputs": [
                    {
                        "name": "y",
                        "index": 0,
                        "dtype": "float32",
                        "format": "ND",
                        "paramType": "required",
                        "shape": [
                            -2
                        ],
                        "format_match_mode": "FormatAgnostic"
                    }
                ],
                "attrs": [
                    {
                        "name": "tmp",
                        "dtype": "int",
                        "value": 0
                    },
                    .........
                ],
                "opMode": "dynamic",
                "optionalInputMode": "gen_placeholder",
                "deterministic": "ignore"
            },
            "filePath": "ascendxxx/acos_custom/AcosCustom_da824ede53d7e754f85c14b9446ec2fc.json"
        }
        ```

-   **获取包含device信息的库文件列表**

    ```
    msobjdump --list-elf ${cmake_install_dir}/op_api/lib/libcust_opapi.so 
    ```

    执行上述命令，终端会打印所有文件，屏显信息形如：

    ```
    ELF file    0: ascendxxx_acos_custom_AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26.json
    ELF file    1: ascendxxx_acos_custom_AcosCustom_dad9c8ca8fcbfd789010c8b1c0da8e26.o
    ....................
    ELF file    2: ascendxxx_acos_custom_AcosCustom_da824ede53d7e754f85c14b9446ec2fc.json
    ELF file    3: ascendxxx_acos_custom_AcosCustom_da824ede53d7e754f85c14b9446ec2fc.o
    ```


## 使用样例（Kernel直调算子工程）<a name="section12189203721319"></a>

以MatMulInvocationNeo算子为例（NPU模式），假设$\{cmake\_install\_dir\}为算子Cmake编译产物根目录，目录结构如下（仅为示例，具体以实际算子工程为准），类似[CMake编译配置文件编写](基于样例工程完成Kernel直调.md#section185111259496)。

```
out
├── lib
│   ├── libascendc_kernels_npu.so
├── include
│   ├── ascendc_kernels_npu
│           ├── aclrtlaunch_matmul_custom.h
│           ├── aclrtlaunch_triple_chevrons_func.h
......
```

工具对编译生成的库文件（如\*.so、\*.a等）进行解析和解压，功能实现命令样例如下：

-   **解析包含device信息的库文件**

    支持两种打印方式，请按需选取，解析字段含义参见[表6](#table217334916136)。

    -   简单打印

        ```
        msobjdump --dump-elf ${cmake_install_dir}/out/libascendc_kernels_npu.so
        ```

        执行上述命令，终端打印基础device信息，示例如下：

        ```
        ===========================
        [VERSION]: 1
        [TYPE COUNT]: 1
        ===========================
        [ELF FILE 0]: ascendxxxb1_ascendc_kernels_npu_0_mix.o
        [KERNEL TYPE]: mix
        [KERNEL LEN]: 511560
        [ASCEND META]: None
        ```

    -   全量打印

        ```
        msobjdump --dump-elf ${cmake_install_dir}/out/libascendc_kernels_npu.so --verbose
        ```

        执行上述命令，终端打印所有device信息，示例如下：

        ```
        ===========================
        [VERSION]: 1
        [TYPE COUNT]: 1
        ===========================
        [ELF FILE 0]: ascendxxxb1_ascendc_kernels_npu_0_mix.o
        [KERNEL TYPE]: mix
        [KERNEL LEN]: 511560
        [ASCEND META]: None
        ====== [elf header infos] ======
        ELF Header:
          Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
          Class:                             ELF64
          Data:                              2's complement, little endian
          Version:                           1 (current)
          OS/ABI:                            UNIX - System V
          ABI Version:                       0
          Type:                              EXEC (Executable file)
          Machine:                           <unknown>: 0x1029
          Version:                           0x1
          Entry point address:               0x0
          Start of program headers:          64 (bytes into file)
          Start of section headers:          510280 (bytes into file)
          Flags:                             0x940000
          Size of this header:               64 (bytes)
          Size of program headers:           56 (bytes)
          Number of program headers:         2
          Size of section headers:           64 (bytes)
          Number of section headers:         20
          Section header string table index: 18
        
        Section Headers:
          [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
          [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
          [ 1] .text             PROGBITS        0000000000000000 0000b0 010a08 00  AX  0   0  4
           .....................................................................................
          [19] .strtab           STRTAB          0000000000000000 071278 00b6cb 00      0   0  1
        Key to Flags:
          W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
          L (link order), O (extra OS processing required), G (group), T (TLS),
          C (compressed), x (unknown), o (OS specific), E (exclude),
          D (mbind), p (processor specific)
        
        There are no section groups in this file.
        
        Program Headers:
          Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
          LOAD           0x0000b0 0x0000000000000000 0x0000000000000000 0x010aa8 0x010aa8 R E 0x1000
          GNU_STACK      0x000000 0x0000000000000000 0x0000000000000000 0x000000 0x000000 RW  0
        ......
        ```

-   **解压包含device信息的库文件并落盘**

    ```
    msobjdump --extract-elf ${cmake_install_dir}/out/libascendc_kernels_npu.so
    ```

    执行上述命令，默认在当前执行路径下落盘ascendxxxb1\_ascendc\_kernels\_npu\_0\_mix.o文件。

-   **获取包含device信息的库文件列表**

    ```
    msobjdump --list-elf ${cmake_install_dir}/out/libascendc_kernels_npu.so
    ```

    执行上述命令，终端会打印所有文件，屏显信息形如：

    ```
    ELF file    0: ascendxxxb1_ascendc_kernels_npu_0_mix.o
    ```
