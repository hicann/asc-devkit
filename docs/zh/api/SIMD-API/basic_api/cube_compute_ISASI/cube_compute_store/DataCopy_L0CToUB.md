# DataCopy（L0C到UB数据搬运）<a name="ZH-CN_TOPIC_0000002382867725"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB_res.md#id1 -->

## 功能说明<a name="section474617392321"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

矩阵计算的结果存放在L0C Buffer，DataCopy接口用于将结果搬运至Unified Buffer（UB）中，并且在搬运过程中支持随路量化等操作。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品为例，下图展示了随路量化、随路ReLU、随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为非量化模式，仅为Cast，其余为随路scalar/tensor量化模式。

**图1** DataCopy L0C Buffer到UB数据搬运随路功能支持情况

![](../../../../figures/L0C2UB_Function_Combination_950.png)
<!-- end id8 -->

## 函数原型<a name="section1954364615315"></a>

<!-- npu="950" id9 -->
针对Ascend 950PR&950DT系列产品，DataCopy矩阵搬出接口支持多种随路能力的组合，需要设置不同的寄存器，配合数据搬运指令开启不同的数据搬运能力，对应的接口如下：

- 数据搬运接口，通路L0C Buffer-\>UB，配合设置寄存器实现量化和ReLU激活，矩阵分型格式的转换，函数原型为：

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
    ```

- [SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md)：寄存器设置接口，通过调用该接口设置Vector随路量化，其中tensor的每个元素都代表一个量化参数，启用tensor量化时需要设置。

- [SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md)：寄存器设置接口，通过调用该接口设置Scalar随路量化参数，此元素代表整个输出矩阵使用的量化参数，启用Scalar量化时需要设置。

- [SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md)：寄存器设置接口，通过调用该接口设置随路[Nz2ND](../cube_store_key_features/NZ2ND.md)格式转换配置，使用随路Nz2ND需要设置。

- [SetFixpipeNz2dnFlag](../cube_store_aux_config/SetFixpipeNz2dnFlag.md)：寄存器设置接口，通过调用该接口设置随路[Nz2DN](../cube_store_key_features/NZ2DN.md)格式转换配置，使用随路Nz2DN需要设置。
<!-- end id9 -->

<!-- npu="310p" id10 -->
针对Atlas推理系列产品AI Core，支持的函数原型如下：

- Local Memory -\> Local Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

- Local Memory -> Local Memory，支持源操作数和目的操作数类型不一致。

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```
<!-- end id10 -->

## 参数说明<a name="section1592117358563"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#section8664371012)。 |
| U | 源操作数的数据类型。支持的数据类型请参考[数据类型](#section8664371012)。 |

<!-- npu="950" id11 -->
**表2** 参数说明（[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，物理位置为UB（TPosition为VECIN、VECOUT或VECCALC），数据格式为Nz、ND或DN，地址需要满足32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，物理位置为L0C Buffer（TPosition为CO1），通常为Mmad接口计算的结果。数据格式为Nz，地址需要满足64字节对齐。 |
| intriParams | 输入 | 搬运参数，类型为[DataCopyCO12DstParams](#zh-cn_topic_0000002542828493_table35908519282)。 <br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3** DataCopyCO12DstParams结构体参数定义（[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="zh-cn_topic_0000002542828493_table35908519282"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| sid | 此参数用户无需关注，设置为0即可。 |
| nSize | 源Nz矩阵在N方向上的大小。取值范围nSize∈[0, 4095]，nSize必须为16的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 源Nz矩阵在M方向上的大小，取值范围为mSize∈[0, 65535]。若开启Nz2DN，mSize*sizeof(T)必须为32的倍数。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| dstStride | &bull;不开启Nz2ND功能（[Nz2Nz搬运](../cube_store_key_features/NZ2NZ.md)）：目的Nz矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为datablock（32字节）。<br>&bull;开启随路[Nz2ND](../cube_store_key_features/NZ2ND.md)功能：目的ND矩阵每一行中的元素个数，取值不为0 ，单位为element。 |
| srcStride | 源Nz矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]，单位为C0_Size（16*sizeof(T)），T为src的数据类型，其值应填成mSize对16向上取整。 |
| unitFlag | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag。<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位。<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>参数设置方案和特性细节可参考：Mmad计算中关键特性说明的[UnitFlag](../mmad_compute_key_features/UnitFlag.md)章节。 |
| quantPre | 用于控制量化模式，QuantMode_t类型，具体定义如下：<br>&nbsp;&nbsp;&bull; float/int32_t输出此需配置为QuantMode_t::NoQuant。<br>&nbsp;&nbsp;&bull; half/bfloat16_t输出，此参数需配置为QuantMode_t::F322F16/QuantMode_t::F322BF16。<br>&nbsp;&nbsp;&bull; 配置为scalar量化时，需要调用[SetFixpipePreQuantFlag](../cube_store_aux_config/SetFixpipePreQuantFlag.md)接口来设置scalar量化参数。<br>&nbsp;&nbsp;&bull; 配置为tensor量化时，需要调用[SetFixPipeConfig](../cube_store_aux_config/SetFixPipeConfig.md)来设置tensor量化参数，其中tensor量化参数需要通过DataCopy从L1 Buffer搬运至Fixpipe Buffer。<br>注：此参数需要用户手动配置，不会自动推导配置对应量化模式。<br>enum QuantMode_t<br>{<br>    NoQuant,               // 不开启量化功能<br>    F322F16,               // Float32_2_Float16: float cast成half，cast mode为CAST_RINT模式<br>    F322BF16,              // Float32_2_BFloat16 :float cast成bfloat16_t，cast mode为CAST_RINT模式<br>    DEQF16,                // DeQuant_Float16: int32_t量化成half, scalar量化<br>    VDEQF16,               // Vector_DeQuant_Float16: int32_t量化成half，tensor量化<br>    QF322B8_PRE,           // Quant_Float32_2_B8: float量化成int8_t/uint8_t，scalar量化<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: float量化成int8_t/uint8_t，tensor量化<br>    REQ8,                  // ReQuant_int8: int32_t量化成int8_t/uint8_t，scalar量化<br>    VREQ8,                 // Vector_ReQuant_int8: int32_t量化成int8_t/uint8_t，tensor量化<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，scalar量化<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，tensor量化<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，scalar量化<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，tensor量化<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，scalar量化<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，tensor量化<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，scalar量化<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，tensor量化<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: float量化成half，scalar量化<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: float量化成half，tensor量化<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: float量化成bfloat16_t，scalar量化<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: float量化成bfloat16_t，tensor量化<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: float量化成float，scalar量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../adv_api/quantization/AscendDequant.md)高阶API。<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float量化成float，tensor量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../adv_api/quantization/AscendDequant.md)高阶API。<br>}; |
| reluPre | 用于配置ReLU操作的模式，类型为uint8_t，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启ReLU；<br>&nbsp;&nbsp;&bull; 1：Normal ReLU。 |
| channelSplit | 类型为bool，默认值为false。本通路不支持Channel Split，保持默认值即可。 |
| nz2ndEn | 类型为bool，配置是否开启Nz2ND的格式转换。<br>如果要开启Nz2ND的功能需要同步调用[SetFixpipeNz2ndFlag](../cube_store_aux_config/SetFixpipeNz2ndFlag.md)来设置格式转换的相关配置信息。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
| dualDstCtl | 双目标模式控制参数。对于Ascend 950PR&950DT系列产品，同一AI Core内有一个Cube Core和两个Vector Core，当启用双目标模式控制时，L0C Buffer中的M×N矩阵将被分成两半，并同时分别写入两个Vector Core各自的UB中，其中前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。<br>&nbsp;&nbsp;&bull; 2'b00：单目标模式，将整个矩阵写入通过subBlockId参数配置的目标UB。<br>&nbsp;&nbsp;&bull; 2'b01：双目标模式，按M维度拆分成形状为M / 2 \* N的两个矩阵，分别写入两个UB, M必须为2的倍数。<br>&nbsp;&nbsp;&bull; 2'b10：双目标模式，按N维度拆分成形状为M \* N / 2的两个矩阵，分别写入两个UB, N须为32的倍数。<br>&nbsp;&nbsp;&bull; 2'b11：保留值。<br>dualDstCtl仅支持在普通搬运模式（Nz2Nz）或Nz2ND搬运场景下使用，不支持随路功能场景。<br>参数设置方案和特性细节可参考：[L0C到UB双目标模式](../cube_store_key_features/L0C_to_UB_dual_target_mode.md) |
| subBlockId | 启用单目标模式时用于指示目标UB的SUB BLOCK ID。取值为0或1，取值为0时写入SUB BLOCK0，为1时写入SUB BLOCK1，默认为0。 |
| nz2dnEn | 类型为bool，配置是否开启Nz2DN的格式转换。<br>如果要开启Nz2DN的功能需要同步调用[SetFixpipeNz2dnFlag](../cube_store_aux_config/SetFixpipeNz2dnFlag.md)来设置格式转换的相关配置信息。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。 |
<!-- end id11 -->

<!-- npu="310p" id12 -->
**表4** 参数说明（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。 |
| src | 输入 | 源操作数，类型为LocalTensor。 |
| intriParams | 输入 | 搬运参数。DataCopyParams类型。 |
| enhancedParams | 输入 | 增强信息参数。DataCopyEnhancedParams类型。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表5** DataCopyEnhancedParams结构参数说明（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 参数名称 | 含义 |
| ---------- | ---------- |
| blockMode | 数据搬运基本分形，BlockMode枚举类型，支持以下配置：<br>&nbsp;&nbsp;&bull; BLOCK_MODE_NORMAL：表示传输单位为32字节。当前暂不支持。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_MATRIX：表示传输单位为一个16 \* 16的cube分形。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_VECTOR：表示传输单位为一个1 \* 16的cube分形。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_SMALL_CHANNEL：表示传输单位为一个16 \* 4的cube分形。当前暂不支持。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_DEPTHWISE：表示传输单位为一个16 \* 16的cube分形，提供随路channel-split功能。当前暂不支持。<br>每种模式下对应的blockLen等参数单位见[表6](#table13396838183618)。 |
| deqScale | 随路精度转换辅助参数，即量化模式，支持的量化模式取值和对应的数据类型等信息请参考[表7](#table168091348673)。其中DEQ、DEQ8、DEQ16模式，需要传入deqValue量化系数，设置deqValue的对应比特位；VDEQ、VDEQ8、VDEQ16模式，需要传入包含16个元素（deqValue）的量化参数向量，设置deqTensorAddr的对应比特位，同时保证DEQADDR中存储的反量化参数向量的每个元素（deqValue）都符合预期和使用限制。<br>VDEQ模式下，反量化参数向量长度为32字节（16个half元素）；其他模式下，反量化参数向量长度为128字节（16个64bit的反量化元素）。 |
| deqValue | 量化系数。 deqValue的配置方式请参考[deqValue配置方式](#table54451538192912)。 |
| deqTensorAddr | Unified Buffer（UB）中存储反量化参数向量的起始地址。deqScale为VDEQ/VDEQ8/VDEQ16模式时，需要传入反量化运算时的参数向量的地址。该地址要满足32字节对齐。<br>对于VDEQ模式，该地址指向32字节大小的反量化参数向量，其中每个元素大小为16bit（half）。<br>对于VDEQ8、VDEQ16模式，反量化参数向量中的每个元素大小都为64bit。搬运时会搬运blockCount个连续传输数据块，每个数据块的长度为blockLen。每个数据块对应一个128字节的反量化向量。对于同一个数据块，反量化参数向量中的16个元素会被连续复用。不同的数据块，对应不同的反量化参数向量，地址会相应的偏移12。例如：假设对应起始地址为A，第一个数据块的12反量化参数向量起始地址为A，第二个数据块的12反量化参数向量起始地址为A + 12。<br>同一个反量化参数向量的每一个元素的MCB标志位必须一致。 |
| sidStoreMode | 用于deqScale为DEQ8/VDEQ8时配置存储模式，控制反量化结果如何存储在dst地址中。配置效果参考[sidStoreMode配置示意图](#fig5416115192414)。<br>&nbsp;&nbsp;&bull; 0：dst的数据存储在每个DataBlock的前半段，即每32字节的高16字节；<br>&nbsp;&nbsp;&bull; 1：dst的数据存储在每个DataBlock的后半段，即每32字节的低16字节；<br>&nbsp;&nbsp;&bull; 2：dst的数据存储在完整的DataBlock中，即整个32字节。 |
| isRelu | 配置是否可以随路做线性整流操作。配置deqValue的情况下，如果该参数被置为true，那么会刷新deqValue的ReLU标志位为1；如果被置为false，则不会做修改。配置deqTensorAddr的情况下，反量化参数向量元素中的ReLU标志位不生效，以isRelu为准。<br>仅配置isRelu，不配置量化参数，即deqValue配置为DEQ_NONE场景，支持src和dst的数据类型组合如下：{half，half}，{float，float}，{int32_t，int32_t}，{float，half}；同时配置isRelu和量化参数的场景，支持的数据类型组合参考[表7](#table168091348673)。 |
| padMode | 预留参数，当前暂不支持。 |

**表6** 不同blockMode对应的参数单位（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="table13396838183618"></a>

| blockMode | src | dst | 数据类型 | blockLen单位 | srcStride单位 | dstStride单位 |
| ---------- | ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| BLOCK_MODE_MATRIX | L0C Buffer（CO1） | UB（CO2） | half、int16_t、uint16_t | 512B | 512B | 32B |
| BLOCK_MODE_MATRIX | L0C Buffer（CO1） | UB（CO2） | float、int32_t、uint32_t | 1024B | 1024B | 32B |
| BLOCK_MODE_VECTOR | L0C Buffer（CO1） | UB（CO2） | half、int16_t、uint16_t | 32B | 512B | 32B |
| BLOCK_MODE_VECTOR | L0C Buffer（CO1） | UB（CO2） | float、int32_t、uint32_t | 64B | 1024B | 32B |

**表7** deqScale参数列表（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="table168091348673"></a>

| 量化模式 | src.dtype | dst.dtype | 配合使用的参数 |
| ---------- | ---------- | ---------- | ---------- |
| DEQ | int32_t | half | deqValue中的变量M |
| DEQ | half | half | deqValue中的变量M |
| DEQ8 | int32_t | int8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ8 | int32_t | uint8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | half | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | int16_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ | int32_t | half | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | int8_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | uint8_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | half | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | int16_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |

**表8** deqValue配置方式（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="table54451538192912"></a>

| 模式 | 比特位数 | 变量名 | 作用介绍 |
| ---------- | ---------- | ---------- | ---------- |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 0~31 | M | 32位数视为float，作为反量化计算所需要乘的值。src为int32_t，dst为int16_t的场景下，变量M不生效。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 32~35 | N | 4位比特位，表示范围为[1, 16]（b'0000对应表示1，b'1111对应表示16）。<br>当模式为DEQ8、VDEQ8时，MCB标志位置为1时，将输入的值进行右移N比特位。当模式为DEQ16、VDEQ16并且dst数据类型为int16_t时，直接进行N位的右移，不受MCB标志位控制。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 36 | MCB标志位 | Mode Control Bit。如果置为0，输入的int32_t会被直接转换为float。如果置为1，输入的int32_t会先右移N比特位，转变成int16_t，然后转换为float。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 37~45 | Offset | 9bit的整型数据，在进行反量化src * M的计算结果后与Offset进行相加。仅在DEQ8、VDEQ8模式中会用到。如果不使用offset，请置为0。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 46 | Sign标志位 | 如果置为1，表明反量化结果是signed(int8)；如果置为0，表明反量化结果是unsigned(uint8)。仅在DEQ8、VDEQ8模式中会用到。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 47 | ReLU标志位 | 如果置为1，对最终结果进行ReLU计算；如果置为0，不进行额外计算。<br>&nbsp;&nbsp;&bull;对于int32_t->int8_t，配置ReLU时，offset必须配置成-128；<br>&nbsp;&nbsp;&bull;对于int32_t->uint8_t，配置ReLU时，offset必须配置成0。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 48~63 | - | 预留 |
| DEQ、VDEQ | 0 ~ 15 | M | 这16位数被视为half，作为反量化计算需要乘的值。 |

**图2** sidStoreMode配置示意图（[NPU架构版本2002](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="fig5416115192414"></a>  

![](../../../../figures/sidstoremode_config.png "sidStoreMode配置示意图")
<!-- end id12 -->

## 数据类型<a id="section8664371012"></a>

<!-- npu="950" id13 -->
针对Ascend 950PR&950DT系列产品，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（UB） |
| ---------- | ---------- |
| float | int8_t、uint8_t、hifloat8_t、fp8_e4m3fn_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、bfloat16_t、int32_t。 |
<!-- end id13 -->

<!-- npu="310p" id14 -->
针对Atlas推理系列产品AI Core：

- 源操作数和目的操作数的数据类型保持一致时，支持的数据类型为：half、float、int32_t、uint32_t。

- 源操作数和目的操作数的数据类型不一致时，支持情况见下表：

  | 源操作数的数据类型 | 目的操作数的数据类型 |
  | ---------- | ---------- |
  | float | half |
  | int32_t | int8_t、uint8_t、int16_t、half。 |
<!-- end id14 -->

## 返回值说明<a name="section16867144513189"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id15 -->
针对Ascend 950PR&950DT系列产品，约束说明如下：

- 源矩阵Nz格式地址要求64字节对齐，目的矩阵Nz格式地址需要满足32字节对齐。
- 当搬出的mSize、nSize或者ndNum中的任意一个值为0时，该指令不会被执行。ndNum=0时会报warning。
- 量化和ReLU参数不能为inf/nan和非规格化数。
- 目标数据不能有重叠。如果对目的地址有重叠写入，硬件不会报告任何警告和错误，也不保证重叠数据的写入顺序。
- unitFlag特性开启需要配合Mmad同时开启。
- dualDstCtl仅支持在普通搬运模式（Nz2Nz）或Nz2ND搬运场景下使用。
- 当启用Nz2DN且srcNzC0Stride不等于1时，不能同时开启unitFlag。
- 该接口仅适用于Mix算子场景，且仅支持Cube核数与Vector核数比例为1:2的配置。在该场景下，subBlockId仅支持取值0或1。
- 特殊值/边界值约束说明：

    对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式，该模式由CTRL\[48\]直接决定；

    对于fp8\_e4m3fn\_t类型，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行更精细的设置；

    - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。对于fp8\_e4m3fn\_t类型，若结果的绝对值为inf或大于fp8\_e4m3fn\_t的最大规格化值，则输出结果为nan。
    - 饱和模式：CTRL\[48\]设置成1'b0，inf输出会被饱和为±MAX， nan输出会被饱和为0。对于fp8\_e4m3fn\_t类型，当CTRL\[50\]设置成1'b0时，nan被转换为零，当CTRL\[50\]设置成1'b1时，nan保持不变，若结果的绝对值为inf或大于fp8\_e4m3fn\_t的最大规格化值，则结果为最大规格化值。

        ```cpp
        // 该模式由CTRL[48]直接决定，不受CTRL[60]限制。
        AscendC::SetCtrlSpr<48, 48>(0);
        ```

    对于整数类型只有饱和模式。
    L0C Buffer到UB搬运接口的边界值汇总请见[矩阵计算搬出相关接口边界值汇总](../../../../appendix/matrix_compute_move_out_interface_boundary_value_summary.md)。
<!-- end id15 -->

<!-- npu="310p" id16 -->
针对Atlas推理系列产品AI Core，约束说明如下：

- 量化参数不能为inf/nan和非规格化数。
- 开发者需要保证DataCopyEnhancedParams中的isRelu参数配置和量化系数deqValue/量化参数向量deqTensorAddr的ReLU标志位配置一致：都开启或都不开启。
- 如果有随路精度转换，操作数的blockLen单位需要减半。
<!-- end id16 -->

## 调用示例<a name="section1491211256313"></a>

<!-- npu="950" id17 -->
针对Ascend 950PR&950DT系列产品，调用示例如下：

- 输入A矩阵和B矩阵的数据类型为half，输出C矩阵为int8_t，不开启Nz2ND和Nz2DN的格式转换，不开启双目标模式，搬入默认的SUB BLOCK0，开启scalar量化：
    
    ```cpp
    constexpr uint16_t c0Size = 32;
    AscendC::DataCopyCO12DstParams dataCopyParams;

    dataCopyParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
    dataCopyParams.mSize = baseM;
    dataCopyParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
    dataCopyParams.nSize = baseN;
    dataCopyParams.quantPre = QuantMode_t::QF322B8_PRE;

    uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScalar));
    constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
    deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
    AscendC::SetFixpipePreQuantFlag(deqScalar);
    AscendC::DataCopy(ubLocal, c, dataCopyParams);
    ```
<!-- end id17 -->

<!-- npu="310p" id18 -->
针对Atlas推理系列产品AI Core，调用示例如下：

- Local Memory -\> Local Memory

    ```cpp
    // srcLocal、dstLocal为half类型的LocalTensor
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素
    intriParams.srcGap = 0; // 源操作数做连续搬运
    intriParams.dstGap = 0; // 目的操作数连续排布
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // 传输单位为16*16的cube分形
    enhancedParams.deqScale = DeqScale::DEQ_NONE; // 不做量化
    enhancedParams.deqValue = 0;
    enhancedParams.deqTensorAddr = 0;
    enhancedParams.sidStoreMode = 0; // 仅deqScale为DEQ8/VDEQ8时生效
    enhancedParams.isRelu = false; // 不支持ReLU
    enhancedParams.padMode = pad_t::PAD_NONE; // 预留参数
    // 功能等同基础数据搬运
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```

- Local Memory -\> Local Memory，源操作数与目的操作数类型不一致

    ```cpp
    // 以下示例为L0C Buffer -> UB通路的数据搬运，仅在Atlas推理系列产品AI Core场景下支持。srcLocal为int32_t类型的L0C Buffer上的LocalTensor，dstLocal为int8_t类型的UB上的LocalTensor
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素
    intriParams.srcGap = 0; // 源操作数做连续搬运
    intriParams.dstGap = 0; // 目的操作数连续排布
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // 传输单位为16*16的cube分形
    enhancedParams.deqScale = DeqScale::DEQ8; // int32_t -> int8_t量化模式，配合deqValues使用
    enhancedParams.deqValue = 70369809530880; // float 1 = uint64_t 1065353216, when s322s8 use 1065353216 | (1 << 46)
    enhancedParams.deqTensorAddr = 0; // 仅VDEQ模式下使用
    enhancedParams.sidStoreMode = 2; // dstLocal的数据存储在完整的DataBlock中
    enhancedParams.isRelu = false; // 不支持ReLU
    enhancedParams.padMode = pad_t::PAD_NONE; // 预留参数
    // 此时dstLocal = srcLocal
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```
<!-- end id18 -->
