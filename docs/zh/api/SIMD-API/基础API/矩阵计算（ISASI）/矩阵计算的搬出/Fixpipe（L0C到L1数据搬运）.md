# Fixpipe（L0C到L1数据搬运）<a id="ZH-CN_TOPIC_0000002538231198"></a>

## 产品支持情况<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持包含FixpipeParamsV220/FixpipeParamsArch3510参数的接口。
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：仅支持包含FixpipeParamsV220参数的接口。
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：仅支持包含FixpipeParamsV220参数的接口。
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_fixpipe_intf.h、basic_api/kernel_operator_data_copy_intf.h"`。

矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至L1 Buffer中，并且在搬运过程中支持随路格式转换等操作。

<!-- npu="950,A3,910b" id10 -->

下图展示了随路量化、随路ReLU、随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为非量化模式，仅为Cast，其余为随路scalar/tensor量化模式。

<!-- npu="A3,910b" id11 -->
**图1** L0C2L1流程图（[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id11 -->

<!-- npu="950" id13 -->
**图2** L0C2L1流程图（[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh-cn_topic_0000002542828493_fig1828513492547"></a>  

![](../../../../figures/L0C2L1_Function_Combination_950.png)
<!-- end id13 -->
<!-- end id10 -->

## 函数原型<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

为了方便用户使用，减少学习理解成本，针对用户经常使用的场景组合，提供了矩阵搬出的组合接口Fixpipe，接口内包含了设置寄存器与数据搬运能力，对应的接口如下：

**传入FixpipeParamsArch3510参数的接口：**

- 通路L0C Buffer(CO1)->L1 Buffer(C1)，不开启随路[tensor量化](../矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- 通路L0C Buffer(CO1)->L1 Buffer(C1)，开启随路[tensor量化](../矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**传入FixpipeParamsV220参数的接口：**

- 通路L0C Buffer(CO1)->L1 Buffer(C1)，不开启随路[tensor量化](../矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- 通路L0C Buffer(CO1)->L1 Buffer(C1)，开启随路[tensor量化](../矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

## 参数说明<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**表1** Fixpipe模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T/U | 目的操作数/源操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002511188540_section4219135304818)。 |
| config | Fixpipe相关配置参数，类型为FixpipeConfig。取值如下：<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR（默认取值）**：开启NZ2ND，输出数据格式为ND格式。<!-- npu="A3,910b" id17 -->针对<!-- npu="A3" id18 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品，<!-- end id18 --><!-- npu="910b" id19 -->Atlas A2 训练系列产品/Atlas A2 推理系列产品，<!-- end id19 -->在L0C Buffer -> L1 Buffer通路下不生效。<!-- end id17 --><br>&nbsp;&nbsp;&bull; CFG_NZ：NZ2NZ，输出数据格式为NZ格式。<br><!-- npu="950" id14 -->&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR：针对Ascend 950PR/Ascend 950DT，开启NZ2DN，输出数据格式为DN格式。<br><!-- end id14 --><pre>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // 用于用户指定目的地址的位置是否是UB <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // 输出数据格式仍为NZ格式。<br>    ROW_MAJOR, // 开启NZ2ND，输出数据格式为ND格式。<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // 仅Ascend 950PR/Ascend 950DT支持，开启NZ2DN，输出数据格式为DN格式。<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // 仅Ascend 950PR/Ascend 950DT支持<!-- end id16 --></pre> |
| S | 参数cbufWorkspace的数据类型，即随路量化参数的数据类型。<br>&nbsp;&nbsp;&bull;当目的操作数、源操作数、cbufWorkspace使用基础数据类型时，模板参数S必须为uint64_t类型，否则编译失败。<br>&nbsp;&nbsp;&bull;当目的操作数、源操作数、cbufWorkspace使用TensorTrait类型时，模板参数S的LiteType必须为uint64_t类型，否则编译失败。<br>模板参数S后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表2** Fixpipe参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。数据格式为NZ格式。NZ地址需满足32字节对齐。<!-- npu="950" id20 --><br>针对Ascend 950PR/Ascend 950DT，还支持数据格式为ND、DN，地址需满足32字节对齐。<!-- end id20 --> |
| src | 输入 | 源操作数，类型为LocalTensor，支持的物理地址为L0C Buffer（TPosition为CO1），为Mmad接口计算的结果。数据格式为NZ格式，地址需要满足64字节对齐。 |
| intriParams | 输入 | Fixpipe搬运参数，具体定义请参考"basic_api/interface/kernel_struct_fixpipe.h"。<br>参数说明请参考Fixpipe搬运参数（FixpipeParamsArch3510、FixpipeParamsV220）结构体说明。 |
| cbufWorkspace | 输入 | 量化参数，类型为`LocalTensor<uint64_t>`，支持的物理地址为L1 Buffer（TPosition为C1），地址需满足32字节对齐。<br>&nbsp;&nbsp;&bull;当quantPre为VDEQF16、VQF322B8_PRE、VREQ8时支持。<br><!-- npu="950" id21 -->&nbsp;&nbsp;&bull;针对Ascend 950PR/Ascend 950DT，除上述外还有VQF322FP8_PRE、VQF322HIF8_PRE、VQF322HIF8_PRE_HYBRID、VQS322BF16_PRE、VQF322F16_PRE、VQF322BF16_PRE、VQF322F32_PRE支持。<br><!-- end id21 -->quantPre介绍请参考Fixpipe搬运参数结构体中quantPre部分。 |

**表3** Fixpipe搬运参数（FixpipeParamsArch3510）结构体说明

| 参数名称 | 数据类型 | 含义 |
| --- | --- | --- |
| nSize | 必选输入 | 源NZ矩阵在N方向上的大小，取值范围为nSize∈[0, 4095]。<br>&nbsp;&nbsp;&bull; 若开启[channelSplit功能](../矩阵搬出关键特性说明/F32-Channel-Split.md)功能，nSize必须为8的倍数。<br>&nbsp;&nbsp;&bull; 若不开启channelSplit功能，nSize必须为16的倍数。<br>&nbsp;&nbsp;&bull; 若开启NZ2ND，nSize*sizeof(T)必须为32的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 必选输入 | 源NZ矩阵在M方向上的大小，取值范围为mSize∈[0, 65535]。若开启NZ2DN，mSize*sizeof(T)必须为32的倍数。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 必选输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]，单位为C0_Size（16*sizeof(T)），T为src的数据类型，其值应填成mSize对16向上取整。 |
| dstStride | 必选输入 | &bull; 不开启NZ2ND/NZ2DN功能（[NZ2NZ搬运](../矩阵搬出关键特性说明/NZ2NZ.md)）：目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为element。（与Fixpipe搬运参数（FixpipeParamsV220）中的dstStride相比，二者的单位发生了变化，FixpipeParamsV220中dstStride的单位为datablock（32字节），而FixpipeParamsArch3510中dstStride的单位为element。）<br>&bull; 开启[NZ2ND](../矩阵搬出关键特性说明/NZ2ND.md)/[NZ2DN](../矩阵搬出关键特性说明/NZ2DN.md)功能：目的ND/DN矩阵每一行中的元素个数，取值不为0，单位为element。 |
| [quantPre](../../矩阵计算（TensorAPI）/矩阵搬出关键特性说明/quant_pre.md) | 可选输入 |用于控制量化模式，QuantMode_t类型，默认值为`QuantMode_t::NoQuant`，具体定义如下：<br>注：此参数需要用户手动配置，不会自动推导配置对应量化模式。<br><pre>enum QuantMode_t<br>{<br>    NoQuant,               // 不开启量化功能<br>    F322F16,               // Float32_2_Float16: float cast成half，cast mode为CAST_RINT模式<br>    F322BF16,              // Float32_2_BFloat16 :float cast成bfloat16_t，cast mode为CAST_RINT模式<br>    DEQF16,                // DeQuant_Float16: int32_t量化成half, scalar量化<br>    VDEQF16,               // Vector_DeQuant_Float16: int32_t量化成half，tensor量化<br>    QF322B8_PRE,           // Quant_Float32_2_B8: float量化成int8_t/uint8_t，scalar量化<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: float量化成int8_t/uint8_t，tensor量化<br>    REQ8,                  // ReQuant_int8: int32_t量化成int8_t/uint8_t，scalar量化<br>    VREQ8,                 // Vector_ReQuant_int8: int32_t量化成int8_t/uint8_t，tensor量化<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，scalar量化<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: float量化成fp8_e4m3fn_t，tensor量化<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，scalar量化<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: float量化成hifloat8_t(Half to Away Round)，tensor量化<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，scalar量化<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: float量化成hifloat8_t(Hybrid Round)，tensor量化<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，scalar量化<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: int32_t量化成bfloat16_t，tensor量化<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: float量化成half，scalar量化<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: float量化成half，tensor量化<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: float量化成bfloat16_t，scalar量化<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: float量化成bfloat16_t，tensor量化<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: float量化成float，scalar量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: float量化成float，tensor量化，精度可以达到双千分之一，无法达到双万分之一。如果有双万分之一的精度要求，建议使用[AscendDeQuant](../../../高阶API/量化操作/AscendDequant.md)高阶API。<br>};</pre> |
| deqScalar | 可选输入 | scalar量化参数，表示单个scale值，quantPre量化模式为[随路量化](../矩阵搬出关键特性说明/随路量化.md)时需要设置该参数。支持的数据类型为`uint64_t`。 |
| [reluEn](../矩阵搬出关键特性说明/随路ReLU.md) | 可选输入 | 是否开启NormReLU的开关：<br>&nbsp;&nbsp;&bull; `false`：不开启NormReLU功能， 默认为`false`；<br>&nbsp;&nbsp;&bull; `true`：开启NormReLU功能。 |
| unitFlag | 可选输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>参数设置方案和特性细节可参考：Mmad计算中关键特性说明的[UnitFlag](../Mmad计算关键特性说明/UnitFlag.md)章节。 |
| params | 可选输入 | 用于选择和配置不同的随路格式转换（NZ2NZ/NZ2ND/NZ2DN），该参数为TransformParams类型的结构体。TransformParams结构体是一个基于模板参数的类型选择器，用于在编译时根据定义FixpipeParamsArch3510搬运参数时使用的模板参数，自动选择对应的参数类型。<br><pre>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};</pre><br>CO2Layout布局类型为ROW_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2ND转换，结构体参数如下：<br><pre>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};</pre>&nbsp;&nbsp;&bull; ndNum：源NZ矩阵的数目，也就是传输ND矩阵的数目，取值范围为ndNum∈[0, 65535]。**注：ndNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&nbsp;&nbsp;&bull; srcNdStride：不同NZ矩阵起始地址之间的间隔，取值范围为srcNdStride∈[0, 65535]，单位为C0_SIZE。当ndNum配置为1时，srcNdStride配置为0即可，不生效。<br>&nbsp;&nbsp;&bull; dstNdStride：目的相邻ND矩阵起始地址之间的偏移，取值范围为dstNdStride∈[1, 2^32 -1]，单位为element。当ndNum配置为1时，dstNdStride配置为0即可，不生效。<br><br>CO2Layout布局类型为COLUMN_MAJOR时， 该指令被定义为从L0C Buffer到目标位置的数据移动，并附带NZ2DN转换，结构体参数如下：<br><pre>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};</pre>&nbsp;&nbsp;&bull; dnNum：传输DN矩阵的数目，取值范围为dnNum∈[0, 65535]。**注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。**<br>&nbsp;&nbsp;&bull; srcNzMatrixStride：不同源NZ矩阵的偏移（头与头），取值范围srcNzMatrixStride∈[0, 65535]，单位C0_SIZE。当dnNum配置为1时，srcNzMatrixStride配置为0即可，不生效。<br>&nbsp;&nbsp;&bull; dstDnMatrixStride：目的相邻ND矩阵起始地址之间的偏移，取值范围dstDnMatrixStride∈[0, 2^32 -1]，单位element。当dnNum配置为1时，dstDnMatrixStride配置为0即可，不生效。<br>&nbsp;&nbsp;&bull; srcNzC0Stride：源矩阵NZ分形中相邻行的地址偏移（头与头），取值范围srcNzC0Stride∈[0, 65535]，单位C0_SIZE。当启用NZ2DN时，srcNzC0Stride不能为0。<br><br>CO2Layout布局类型为NZ时，为普通搬运DMA模式，表示从L0C Buffer到目标位置的正常数据移动。 |
| dualDstCtrl | 可选输入 | 此参数仅在L0C Buffer到Unified Buffer通路下有效。 |
| subBlockId | 可选输入 | 此参数仅在L0C Buffer到Unified Buffer通路下有效。 |
| [isChannelSplit](../矩阵搬出关键特性说明/F32-Channel-Split.md) | 可选输入 | 此参数仅在L0C Buffer到Global Memory通路下有效。 |

**表4** Fixpipe搬运参数（FixpipeParamsV220）结构体说明

| 参数名称 | 数据类型 | 含义 |
| --- | --- | --- |
| nSize | 必选输入 | 源NZ矩阵在N方向上的大小。取值范围nSize∈[0, 4095]，nSize必须为16的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 必选输入 | 源NZ矩阵在M方向上的大小。取值范围为mSize∈[0, 65535]。<br>**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 必选输入 | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]，单位为C0_Size（16*sizeof(T)），T为src的数据类型，其值应填成mSize对16向上取整。 |
| dstStride | 必选输入 | 目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为datablock（32字节）。 |
| [quantPre](../../矩阵计算（TensorAPI）/矩阵搬出关键特性说明/quant_pre.md) | 可选输入 | 用于控制量化模式，QuantMode_t类型，默认值为`QuantMode_t::NoQuant`，具体定义如下：<br>&nbsp;&nbsp;&bull; half/bfloat16_t输出，此参数需配置为`QuantMode_t::F322F16`/`QuantMode_t::F322BF16`。<br>注：此参数需要用户手动配置，不会自动推导配置对应量化模式。此通路不支持NoQuant模式。<br><pre>enum QuantMode_t<br>{<br>    NoQuant,      // 不开启量化功能，此通路不支持<br>    F322F16,      // Float32_2_Float16：float cast成half，cast mode为CAST_RINT模式<br>    F322BF16,     // Float32_2_BFloat16：float cast成bfloat16_t，cast mode为CAST_RINT模式<br>    DEQF16,       // DeQuant_Float16：int32_t量化成half，scalar量化<br>    VDEQF16,      // Vector_DeQuant_Float16：int32_t量化成half，tensor量化<br>    QF322B8_PRE,  // Quant_Float32_2_B8：float量化成int8_t/uint8_t，scalar量化<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8：float量化成int8_t/uint8_t，tensor量化<br>    REQ8,         // ReQuant_int8：int32_t量化成int8_t/uint8_t，scalar量化<br>    VREQ8,        // Vector_ReQuant_int8：int32_t量化成int8_t/uint8_t，tensor量化<br>};</pre> |
| deqScalar | 可选输入 | scalar量化参数，表示单个scale值，quantPre量化模式为[随路量化](../矩阵搬出关键特性说明/随路量化.md)时需要设置该参数。支持的数据类型为`uint64_t`。 |
| [reluEn](../矩阵搬出关键特性说明/随路ReLU.md) | 可选输入 | 是否开启NormReLU的开关：<br>&nbsp;&nbsp;&bull; `false`：不开启NormReLU功能，默认为`false`；<br>&nbsp;&nbsp;&bull; `true`：开启NormReLU功能。 |
| unitFlag | 可选输入 | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>参数设置方案和特性细节可参考：Mmad计算中关键特性说明的[UnitFlag](../Mmad计算关键特性说明/UnitFlag.md)章节。 |
| [channelSplit](../矩阵搬出关键特性说明/F32-Channel-Split.md) | 可选输入 | 仅在L0C Buffer（CO1）->GM通路时生效，此通路设置为false即可。 |
| ndNum | 可选输入 | 仅在L0C Buffer（CO1）->GM通路时生效，此通路无需额外设置。 |
| srcNdStride | 可选输入 | 仅在L0C Buffer（CO1）->GM通路时生效，此通路无需额外设置。 |
| dstNdStride | 可选输入 | 仅在L0C Buffer（CO1）->GM通路时生效，此通路无需额外设置。 |

## 数据类型<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**源矩阵与目的矩阵支持的数据类型组合**

<!-- npu="950" id22 -->
针对Ascend 950PR/Ascend 950DT，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、hifloat8_t、fp8_e4m3fn_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、bfloat16_t、int32_t。 |
<!-- end id22 -->

<!-- npu="A3" id23 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t。 |
| int32_t | int8_t、uint8_t、half。 |
<!-- end id23 -->

<!-- npu="910b" id24 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t。 |
| int32_t | int8_t、uint8_t、half。 |
<!-- end id24 -->

<!-- npu="x90" id25 -->
针对Kirin X90，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、half。 |
| int32_t | int8_t、half。 |
<!-- end id25 -->

<!-- npu="9030" id26 -->
针对Kirin 9030，支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | half |
| int32_t | half |
<!-- end id26 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- 对于量化输入为float32数据类型的说明如下：
    - 标准的IEEE 754 float32格式为：1bit符号位，8bits指数位，23bits尾数位；当前AI处理器支持的float32格式为：1bit符号位，8bits指数位，10bits尾数位。
    - 如果用户提供的是标准的IEEE 754 float32输入，API内部会处理成处理器支持的float32格式进行计算，此时如果golden数据生成过程中使用的是标准的IEEE 754 float32数据，则可能引入精度不匹配问题，需要修正golden数据的生成，将量化参数的23bits尾数位的低13bits数据位清零再参与量化计算。
- 源矩阵NZ格式地址要求64字节对齐，目的矩阵NZ格式地址需要满足32字节对齐。
- 当搬出的mSize、nSize或者ndNum中的任意一个值为0时，该指令不会被执行。ndNum=0时会报warning。
- 量化和ReLU参数不能为inf/nan和非规格化数。
- 目标数据不能有重叠。如果对目的地址有重叠写入，硬件不会报告任何警告和错误，也不保证重叠数据的写入顺序。
- unitFlag特性开启需要配合Mmad同时开启。
<!-- npu="950" id27 -->
- 当启用NZ2DN且srcNzC0Stride不等于1时，不能同时开启unitFlag。
<!-- end id27 -->
<!-- npu="A3,910b" id28 -->
- 针对如下产品型号，特殊值/边界值约束说明如下：
    <!-- npu="A3" id29 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id30 -->
    对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式：

    - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。
    - 饱和模式：CTRL\[48\]设置成1'b0，inf输出会被饱和为±MAX，nan输出会被饱和为0。

        ```cpp
        // 设置CTRL[48]为0，开启浮点数饱和模式
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    对于整数类型只有饱和模式。
<!-- end id28 -->
<!-- npu="950" id31 -->
- Ascend 950PR/Ascend 950DT特殊值/边界值约束说明：

    对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式；

    对于fp8\_e4m3fn\_t类型，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行更精细的设置；

    - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。对于fp8\_e4m3fn\_t类型，若结果的绝对值为inf或大于fp8\_e4m3fn\_t的最大规格化值，则输出结果为nan。
    - 饱和模式：CTRL\[48\]设置成1'b0，inf输出会被饱和为±MAX， nan输出会被饱和为0。对于fp8\_e4m3fn\_t类型，当CTRL\[50\]设置成1'b0时，nan被转换为零，当CTRL\[50\]设置成1'b1时，nan保持不变，若结果的绝对值为inf或大于fp8\_e4m3fn\_t的最大规格化值，则结果为最大规格化值。

        ```cpp
        // 设置CTRL[48]为0，开启浮点数饱和模式
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    对于整数类型只有饱和模式。
<!-- end id31 -->

## 调用示例<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

<!-- npu="950,A3,910b" id32 -->
Fixpipe完整样例请参考[fixpipe\_l0c2l1样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2l1)。

- 示例：通路L0C Buffer->L1 Buffer。输入A矩阵和B矩阵的数据类型为half，输出C矩阵为int8_t，不开启NZ2ND的格式转换，开启scalar量化。

    ```cpp
        AscendC::LocalTensor<outputType> c1Local(AscendC::TPosition::C1, c1Addr, cSizeAlignL1);
    
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        uint16_t c0Size = 32;
        AscendC::FixpipeParamsV220 fixpipeParams;
        // NZ2NZ场景，FixpipeParamsV220的dstStride单位为datablock（32字节）
        fixpipeParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        // scalar quant
        fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE;
        uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t *>(&quantScalar));
        constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
        deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
        fixpipeParams.deqScalar = deqScalar;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(c1Local, c, fixpipeParams);
    
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        uint16_t c0Size = 32;
        AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::NZ> fixpipeParams;
        // NZ2NZ场景，FixpipeParamsArch3510的dstStride单位为元素个数
        fixpipeParams.dstStride = baseM * c0Size;
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        // scalar quant
        fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE;
        uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t *>(&quantScalar));
        constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
        deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
        fixpipeParams.deqScalar = deqScalar;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(c1Local, c, fixpipeParams);
    #endif
    ```
<!-- end id32 -->
