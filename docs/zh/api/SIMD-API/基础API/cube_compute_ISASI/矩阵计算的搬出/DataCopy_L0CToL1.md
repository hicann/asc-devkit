# DataCopy（L0C到L1数据搬运）<a id="ZH-CN_TOPIC_0000002538231198"></a>

## 产品支持情况<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

矩阵计算的结果存放在L0C Buffer，DataCopy接口用于将结果搬运至L1 Buffer中，并且在搬运过程中支持随路格式转换等操作。

<!-- npu="950,A3,910b" id8 -->
下图展示了随路量化、随路ReLU、随路通道合并的有效组合、中间数据类型和数据路径。下图中的F32-\>F16与F32-\>BF16为非量化模式，仅为Cast，其余为随路scalar/tensor量化模式。

<!-- npu="A3,910b" id9 -->
**图1** L0C2L1流程图（[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id9 -->

<!-- npu="950" id11 -->
**图2** L0C2L1流程图（[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh-cn_topic_0000002542828493_fig1828513492475"></a>  

![](../../../../figures/L0C2L1_Function_Combination_DataCopy_950.png)
<!-- end id11 -->
<!-- end id8 -->

## 函数原型<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

DataCopy矩阵搬出接口支持多种随路能力的组合，需要设置不同的寄存器，配合数据搬运指令开启不同的数据搬运能力，对应的接口如下：

- 数据搬运接口，通路L0C Buffer(CO1)->L1 Buffer(C1)，配合设置寄存器实现量化和ReLU激活。

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
    ```

- [SetFixPipeConfig](../矩阵搬出辅助配置接口/SetFixPipeConfig.md)：寄存器设置接口，通过调用该接口设置随路量化，其中tensor的每个元素都代表一个量化参数，使用tensor量化时需要设置。

- [SetFixpipePreQuantFlag](../矩阵搬出辅助配置接口/SetFixpipePreQuantFlag.md)：寄存器设置接口，通过调用该接口设置随路量化参数，此元素代表整个输出矩阵使用的量化参数，使用Scalar量化时需要设置。

- [SetFixpipeNz2ndFlag](../矩阵搬出辅助配置接口/SetFixpipeNz2ndFlag.md)：寄存器设置接口，通过调用该接口设置随路NZ2ND格式转换配置，使用随路NZ2ND需要设置。

<!-- npu="310b" id12 -->
针对Atlas 200I/500 A2 推理产品，还支持如下两个接口：

- [SetFixPipeClipRelu](../矩阵搬出辅助配置接口/SetFixPipeClipRelu.md)：寄存器设置接口，通过调用该接口设置ClipReLU操作的最大值。

- [SetFixPipeAddr](../矩阵搬出辅助配置接口/SetFixPipeAddr.md)：寄存器设置接口，通过调用该接口设置Elementwise操作时LocalTensor的地址。
<!-- end id12 -->

## 参数说明<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**表1** 数据搬运DataCopy模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002511188540_section4219135304818)。 |
| U | 源操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002511188540_section4219135304818)。 |

**表2** 数据搬运DataCopy接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，数据格式为NZ<!-- npu="950" id10 -->、ND（仅Ascend 950PR/Ascend 950DT支持）<!-- end id10 -->格式，地址需要满足32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，支持的物理地址为L0C Buffer（TPosition为CO1），为Mmad接口计算的结果。数据格式为NZ格式，地址需要满足64字节对齐。 |
| intriParams | 输入 | 搬运参数，类型为[DataCopyCO12DstParams](#zh-cn_topic_0000002511188540_table35908519282)。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3** DataCopyCO12DstParams结构体参数定义<a id="zh-cn_topic_0000002511188540_table35908519282"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| sid | 此参数用户无需关注，设置为0即可。 |
| nSize | 源NZ矩阵在N方向上的大小。取值范围nSize∈[0, 4095]，nSize必须为16的倍数。<br>**注：nSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mSize | 源NZ矩阵在M方向上的大小。<br>&nbsp;&nbsp;&bull;不开启随路NZ2ND功能（[NZ2NZ搬运](../矩阵搬出关键特性说明/NZ2NZ.md)）：取值范围为mSize∈[0, 65535]。<br><!-- npu="950" id25 -->&nbsp;&nbsp;&bull;开启随路[NZ2ND](../矩阵搬出关键特性说明/NZ2ND.md)功能（仅Ascend 950PR/Ascend 950DT支持）：取值范围为mSize∈[0, 8192]。<br><!-- end id25 -->**注：mSize=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| dstStride | &bull;不开启NZ2ND功能（[NZ2NZ搬运](../矩阵搬出关键特性说明/NZ2NZ.md)）：目的NZ矩阵中相邻Z排布的起始地址偏移，取值不为0，单位为datablock（32字节）。<br><!-- npu="950" id26 -->&bull;开启随路[NZ2ND](../矩阵搬出关键特性说明/NZ2ND.md)功能（仅Ascend 950PR/Ascend 950DT支持）：目的ND矩阵每一行中的元素个数，取值不为0 ，单位为element。<!-- end id26 --> |
| srcStride | 源NZ矩阵中相邻Z排布的起始地址偏移，取值范围为srcStride∈[0, 65535]，单位为C0_Size（16*sizeof(T)），T为src的数据类型，其值应填成mSize对16向上取整。 |
| unitFlag | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag。<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位。<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>参数设置方案和特性细节可参考：Mmad计算中关键特性说明的[UnitFlag](../Mmad计算关键特性说明/UnitFlag.md)章节。 |
| clipReluPre | <!-- npu="950,A3,910b" id13 -->该参数仅在Atlas 200I/500 A2 推理产品支持。<br><!-- end id13 --><!-- npu="310b" id14 -->用于配置是否开启ClipReLU操作，参数类型为uint8_t，取值如下：0，不开启ClipReLU；1，开启ClipReLU，此时需要调用[SetFixPipeClipRelu](../矩阵搬出辅助配置接口/SetFixPipeClipRelu.md)来设置ClipReLU的最大值。<br>&nbsp;&nbsp;&bull;该操作在随路量化后进行，quantPre配置后才能使用，当前支持的量化模式有F322F16/DEQF16/VDEQF16/QF322B8_PRE/VQF322B8_PRE/REQ8/VREQ8。<br><!-- end id14 --> |
| eltWiseOp | <!-- npu="950,A3,910b" id15 -->该参数仅在Atlas 200I/500 A2 推理产品支持。<br><!-- end id15 --><!-- npu="310b" id16 -->用于配置是否开启Elementwise操作及操作模式。Elementwise操作是指进行随路量化后，可以逐个元素加/减一个LocalTensor，大小为mSize * nSize，具体LocalTensor地址相关参数需要调用[SetFixPipeAddr](../矩阵搬出辅助配置接口/SetFixPipeAddr.md)来设置。<br>eltWiseOp参数类型为uint8_t，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启Elementwise；<br>&nbsp;&nbsp;&bull; 1：Elementwise Addition；<br>&nbsp;&nbsp;&bull; 2：Elementwise Subtraction。<br><!-- end id16 --> |
| quantPre | 用于控制量化模式，QuantMode_t类型，具体定义如下：<br>&nbsp;&nbsp;&bull; half/bfloat16_t输出，此参数需配置为QuantMode_t::F322F16/QuantMode_t::F322BF16。<br>&nbsp;&nbsp;&bull;配置为scalar量化时，需要调用[SetFixpipePreQuantFlag](../矩阵搬出辅助配置接口/SetFixpipePreQuantFlag.md)接口来设置scalar量化参数。<br>&nbsp;&nbsp;&bull;配置为tensor量化时，需要调用[SetFixPipeConfig](../矩阵搬出辅助配置接口/SetFixPipeConfig.md)来设置tensor量化参数，其中tensor量化参数需要通过DataCopy从L1 Buffer搬运至Fixpipe Buffer。<br>注：此参数需要用户手动配置，不会自动推导配置对应量化模式。此通路不支持NoQuant模式。<br><pre>enum QuantMode_t<br>{<br>    NoQuant,      // 不开启量化功能，此通路不支持<br>    F322F16,      // Float32_2_Float16：float cast成half，cast mode为CAST_RINT模式<br>    F322BF16,     // Float32_2_BFloat16：float cast成bfloat16_t，cast mode为CAST_RINT模式<br>    DEQF16,       // DeQuant_Float16：int32_t量化成half，scalar量化<br>    VDEQF16,      // Vector_DeQuant_Float16：int32_t量化成half，tensor量化<br>    QF322B8_PRE,  // Quant_Float32_2_B8：float量化成int8_t/uint8_t，scalar量化<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8：float量化成int8_t/uint8_t，tensor量化<br>    REQ8,         // ReQuant_int8：int32_t量化成int8_t/uint8_t，scalar量化<br>    VREQ8,        // Vector_ReQuant_int8：int32_t量化成int8_t/uint8_t，tensor量化<br>};</pre> |
| reluPre | 用于配置ReLU操作的模式，类型为uint8_t，取值如下：<br>&nbsp;&nbsp;&bull; 0：不开启ReLU<br>&nbsp;&nbsp;&bull; 1：Normal ReLU |
| channelSplit | <!-- npu="950" id27 -->**该参数仅在Ascend 950PR/Ascend 950DT上生效**，仅在NZ格式float类型输出时生效，类型为bool，配置是否开启通道切分功能。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。<br><!-- end id27 --><!-- npu="A3,910b,310b" id29 -->针对如下产品型号，此通路上该参数不生效，设置为false即可。<br><!-- npu="A3" id30 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品<br><!-- end id30 --><!-- npu="910b" id31 -->Atlas A2 训练系列产品/Atlas A2 推理系列产品<br><!-- end id31 --><!-- npu="310b" id32 -->Atlas 200I/500 A2 推理产品<!-- end id32 --><!-- end id29 --> |
| nz2ndEn | <!-- npu="950" id28 -->**该参数仅在Ascend 950PR/Ascend 950DT上生效**，类型为bool，配置是否开启NZ2ND的格式转换。<br>如果要开启NZ2ND的功能需要同步调用[SetFixpipeNz2ndFlag](../矩阵搬出辅助配置接口/SetFixpipeNz2ndFlag.md)来设置格式转换的相关配置信息。<br>&nbsp;&nbsp;&bull; false：不开启；<br>&nbsp;&nbsp;&bull; true：开启。<br><!-- end id28 --><!-- npu="A3,910b,310b" id33 -->针对如下产品型号，此通路上该参数不生效，设置为false即可。<br><!-- npu="A3" id34 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品<br><!-- end id34 --><!-- npu="910b" id35 -->Atlas A2 训练系列产品/Atlas A2 推理系列产品<br><!-- end id35 --><!-- npu="310b" id36 -->Atlas 200I/500 A2 推理产品<!-- end id36 --><!-- end id33 --> |

## 数据类型<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**源矩阵与目的矩阵支持的数据类型组合**

<!-- npu="950" id17 -->
针对Ascend 950PR/Ascend 950DT，DataCopy接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t、float。 |
| int32_t | int8_t、uint8_t、half、int32_t。 |
<!-- end id17 -->

<!-- npu="A3" id18 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，DataCopy接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t。 |
| int32_t | int8_t、uint8_t、half。 |
<!-- end id18 -->

<!-- npu="910b" id19 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，DataCopy接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t。 |
| int32_t | int8_t、uint8_t、half。 |
<!-- end id19 -->

<!-- npu="310b" id20 -->
针对Atlas 200I/500 A2 推理产品，DataCopy接口支持的数据类型组合如下：

| 源矩阵（L0C Buffer） | 目的矩阵（L1 Buffer） |
| ---------- | ---------- |
| float | int8_t、uint8_t、half、bfloat16_t。 |
| int32_t | int8_t、uint8_t、half。 |
<!-- end id20 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- 对于量化输入为float32数据类型的说明如下：
    - 标准的IEEE 754 float32格式为：1bit符号位，8bits指数位，23bits尾数位；当前AI处理器支持的float32格式为：1bit符号位，8bits指数位，10bits尾数位。
    - 如果用户提供的是标准的IEEE 754 float32输入，API内部会处理成处理器支持的float32格式进行计算，此时如果golden数据生成过程中使用的是标准的IEEE 754 float32数据，则可能引入精度不匹配问题，需要修正golden数据的生成，将量化参数的23bits尾数位的低13bits数据位清零再参与量化计算。
- 源矩阵NZ格式地址要求64字节对齐，目的矩阵NZ格式地址需要满足32字节对齐。
- 当搬出的mSize或nSize中的任意一个值为0时，该指令不会被执行。
- 量化和ReLU参数不能为inf/nan和非规格化数。
- 目标数据不能有重叠。如果对目的地址有重叠写入，硬件不会报告任何警告和错误，也不保证重叠数据的写入顺序。
- unitFlag特性开启需要配合Mmad同时开启。
<!-- npu="A3,910b" id21 -->
- 针对如下产品型号，特殊值/边界值约束说明如下：
    <!-- npu="A3" id22 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id23 -->
    对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式：

    - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。
    - 饱和模式：CTRL\[48\]设置成1'b0，inf输出会被饱和为±MAX，nan输出会被饱和为0。

        ```cpp
        // 设置CTRL[48]为0，开启浮点数饱和模式
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    对于整数类型只有饱和模式。
<!-- end id21 -->
<!-- npu="950" id24 -->
- Ascend 950PR/Ascend 950DT特殊值/边界值约束说明：

    对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数量化搬出时的饱和模式；

    对于fp8\_e4m3fn\_t类型，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行更精细的设置；

    - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。对于fp8\_e4m3fn\_t类型，若结果的绝对值为inf或大于fp8\_e4m3fn\_t的最大规格化值，则输出结果为nan。
    - 饱和模式：CTRL\[48\]设置成1'b0，Inf输出会被饱和为±MAX， NaN输出会被饱和为0。对于fp8\_e4m3fn\_t类型，当CTRL\[50\]设置成1'b0时，NaN被转换为零，当CTRL\[50\]设置成1'b1时，NaN保持不变，若结果的绝对值为Inf或大于fp8\_e4m3fn\_t的最大规格化值，则结果为最大规格化值。

        ```cpp
        // 设置CTRL[48]为0，开启浮点数饱和模式
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    对于整数类型只有饱和模式。
<!-- end id24 -->

<!-- npu="950,A3,910b" id25 -->
## 调用示例<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

示例：通路L0C Buffer->L1 Buffer。输入A矩阵和B矩阵的数据类型为half，输出C矩阵为int8_t，不开启NZ2ND的格式转换，开启scalar量化。
    
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
    AscendC::DataCopy(c1Local, c, dataCopyParams);
    ```
<!-- end id25 -->

