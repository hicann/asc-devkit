# MmadFix<a name="ZH-CN_TOPIC_000000253823118103"></a>

## 产品支持情况

### 不传入bias的原型

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
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
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- KirinX90处理器系列产品：不支持
<!-- end id8 -->
<!-- npu="9020" id9 -->
- Kirin9020处理器系列产品：不支持
<!-- end id9 -->
<!-- npu="9030" id10 -->
- Kirin9030处理器系列产品：不支持
<!-- end id10 -->
<!-- npu="9050" id11 -->
- Kirin9050处理器系列产品：支持
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id1 -->

### 传入bias的原型

<!-- npu="950" id12 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id12 -->
<!-- npu="A3" id13 -->
- Atlas A3系列产品：不支持
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2系列产品：不支持
<!-- end id14 -->
<!-- npu="310b" id15 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas训练系列产品：不支持
<!-- end id18 -->
<!-- npu="x90" id19 -->
- KirinX90处理器系列产品：不支持
<!-- end id19 -->
<!-- npu="9020" id20 -->
- Kirin9020处理器系列产品：不支持
<!-- end id20 -->
<!-- npu="9030" id21 -->
- Kirin9030处理器系列产品：不支持
<!-- end id21 -->
<!-- npu="9050" id22 -->
- Kirin9050处理器系列产品：支持
<!-- end id22 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id2 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_macro_intf.h"`。

MmadFix接口是Ascend C面向昇腾AI芯片、Kirin处理器产品的矩阵乘核心计算接口，专为高性能算子开发设计，封装了矩阵乘计算能力，并支持量化（Quant）、激活（ReLU）、eltwise等后处理功能的融合，广泛用于神经网络层（如全连接层）、量化推理场景的算子开发。

MmadFix接口实现矩阵乘计算能力，其数学表达式为：

$$
C = A \times B + Bias
$$

<!-- npu="9050" id23 -->
针对Kirin9050处理器系列产品：

**表1** 矩阵计算矩阵A、B、C解释说明

| 矩阵计算逻辑 | 矩阵计算物理位置 | 维度 | 输入/输出数据格式 | 数据类型 |
| --- | --- | --- | --- | --- |
| A | L1 Buffer | M x K | Nz | <a href="#zh_cn_topic_mmadfix_section5">数据类型</a> |
| B | L0B Buffer | K x N | Zn | <a href="#zh_cn_topic_mmadfix_section5">数据类型</a> |
| C | L1 Buffer | M x N，可支持使用偏置矩阵Bias进行初始化 | Nz | <a href="#zh_cn_topic_mmadfix_section5">数据类型</a> |

矩阵乘的M、K维度通过SetMatrixPara接口（MatrixParaParams参数）配置，N维度通过mmadFixParams参数的n字段传入。MmadFix接口的计算结果直接写入L1 Buffer，省去了传统矩阵乘从L0C Buffer搬出结果的过程。
<!-- end id23 -->


## 函数原型

- 不传入bias

    ```cpp
    template <typename T, typename U, typename S>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const MmadFixBitModeParams& mmadFixParams)

    template <typename T, typename U, typename S>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const MmadFixParams& mmadFixParams)
    ```

- 传入bias

    ```cpp
    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias, const MmadFixBitModeParams& mmadFixParams)

    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias, const MmadFixParams& mmadFixParams)
    ```

## 参数说明<a id="zh_cn_topic_mmadfix_section4"></a>

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，结果矩阵C，类型为LocalTensor，支持的物理存储位置为L1 Buffer（TPosition:C1）。 |
| left | 输入 | 源操作数，左矩阵A，类型为LocalTensor，支持的物理存储位置为L1 Buffer（TPosition:A1）。 |
| right | 输入 | 源操作数，右矩阵B，类型为LocalTensor，支持的物理存储位置为L0B Buffer（TPosition:B2）。 |
| bias | 输入 | 源操作数，Bias矩阵，类型为LocalTensor。Bias的存储位置和维度由mmadFixParams中的biasMatrixBroadcast参数决定：biasMatrixBroadcast=true时，Bias矩阵位于BT Buffer（TPosition:C2），维度为1 x N，沿M方向广播；biasMatrixBroadcast=false时，Bias矩阵位于L1 Buffer（TPosition:C1），维度为M x N，作为C矩阵初始值参与计算。 |
| mmadFixParams | 输入 | 矩阵乘相关参数，类型为MmadFixParams或MmadFixBitModeParams。<br>MmadFixParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`，MmadFixBitModeParams的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>MmadFixParams参数说明请参考[表3](#zh_cn_topic_mmadfix_section4_table3)，MmadFixBitModeParams参数说明请参考[表4](#zh_cn_topic_mmadfix_section4_table4)。 |

**表3** MmadFixParams结构体内参数说明<a id="zh_cn_topic_mmadfix_section4_table3"></a>

| 参数名称 | 含义 |
| --- | --- |
| biasMatrixInitVal | 是否将结果矩阵C初始化为0，默认值为false。<br>&nbsp;&nbsp;&bull; true：C矩阵初始值为0，计算结果为C = A \* B（+ Bias）；<br>&nbsp;&nbsp;&bull; false：C矩阵不初始化，保留已有数据，计算结果为C += A \* B（+ Bias）。 |
| biasMatrixBroadcast | 是否开启Bias矩阵沿M方向广播，默认值为false。<br>&nbsp;&nbsp;&bull; true：Bias矩阵维度为1 x N，位于BT Buffer（TPosition:C2），沿M方向广播，即结果矩阵每一行都使用同一份Bias；<br>&nbsp;&nbsp;&bull; false：Bias矩阵维度为M x N，位于L1 Buffer（TPosition:C1），作为C矩阵初始值参与计算。 |
| preQuantMode | 矩阵乘前量化模式，pre_quant_t枚举类型，默认值为No_Quant。取值包括：No_Quant（不量化）、REQS8_Vector、REQS8_Scalar、DEQF16_Vector、DEQF16_Scalar、DEQS16_Vector、DEQS16_Scalar、DEQS32_Vector、DEQS32_Scalar、REQS4_Vector、REQS4_Scalar、QS322FP8_E4M3_INT_Vector、QS322FP8_E4M3_INT_Scalar。 |
| preReluMode | 矩阵乘前ReLU激活模式，pre_relu_t枚举类型，默认值为NoRelu（不开启激活）。 |
| postQuantMode | 矩阵乘后量化模式，post_quant_t枚举类型，默认值为No_Quant（不量化）。 |
| actPostMode | 矩阵乘后激活模式，uint8_t类型，默认值为0。 |
| preClipReluMode | 矩阵乘前Clip ReLU模式，uint8_t类型，默认值为0。 |
| eltwiseOp | eltwise操作类型，eltwise_op_t枚举类型，默认值为No_Eltwise（不开启eltwise操作）。 |
| eltwiseAntiqEnable | 是否开启eltwise反量化，默认值为false。 |
| eltwiseBroadcastEnable | 是否开启eltwise广播，默认值为false。 |
| itaEnable | 是否开启ITA功能，默认值为false。开启后需通过SetItaMaxAddr接口配置最大值统计相关地址。 |
| biasS32Enable | 是否开启S32（int32_t）格式的Bias，默认值为false。 |
| hmaxEnable | 是否开启HMAX（最大值统计）功能，默认值为false。开启后需通过SetItaMaxAddr接口配置相关地址。 |
| lsbMask | 掩码控制参数，lsb_mask_t枚举类型，默认值为Disable。 |
| gemvCtrl | GEMV模式控制参数，默认值为false。M = 1的矩阵向量乘场景下，用于控制GEMV模式。 |
| groupMode | 分组模式，uint8_t类型，默认值为0。 |
| dependEnable | 是否开启指令间依赖控制，默认值为false。 |
| instrId | 指令编号，instr_id_t枚举类型，默认值为ID_0。 |
| postProcHalfEnable | 是否开启half精度后处理，默认值为false。 |
| layoutCtrl | 布局控制参数，默认值为false。 |
| n | 右矩阵Width（N维度），uint16_t类型，默认值为0。 |

**表4** MmadFixBitModeParams类参数说明<a id="zh_cn_topic_mmadfix_section4_table4"></a>

| 参数名称 | 含义 |
| --- | --- |
| config0 | uint64_t类型，与MmadFixBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | MmadFixBitModeConfig0位域（bit-field）结构体类型，参数参考[表5](#zh_cn_topic_mmadfix_section4_table5)，与config0组成联合体（union）。 |

MmadFixBitModeParams对于MmadFixParams结构体构造进行了优化，本接口适用于scalar流水成为性能优化瓶颈的场景。MmadFixBitModeParams类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数，减少了scalar计算，实现性能提升。

MmadFixBitModeParams类可以直接使用MmadFixParams结构体类型对象初始化：

```cpp
__aicore__ inline MmadFixBitModeParams(const MmadFixParams& mmadFixParams);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0直接进行逐bit位修改来修改参数。

**表5** MmadFixBitModeConfig0结构体参数说明<a id="zh_cn_topic_mmadfix_section4_table5"></a>

| 参数名称 | 含义 |
| --- | --- |
| biasMatrixInitVal | 是否将结果矩阵C初始化为0，取值true/false。<br>该参数是位域结构体的最低位参数，占用1bit，可以使用MmadFixBitModeParams类对象的SetBiasMatrixInitVal()函数设置其值，使用GetBiasMatrixInitVal()函数获取其值。 |
| biasMatrixBroadcast | 是否开启Bias矩阵沿M方向广播，取值true/false。<br>该参数是位域结构体的第二低位参数，占用1bit，可以使用MmadFixBitModeParams类对象的SetBiasMatrixBroadcast()函数设置其值，使用GetBiasMatrixBroadcast()函数获取其值。 |
| preQuantMode | 矩阵乘前量化模式，pre_quant_t枚举类型。<br>该参数是位域结构体的第三低位参数，占用5bit，可以使用MmadFixBitModeParams类对象的SetPreQuantMode()函数设置其值，使用GetPreQuantMode()函数获取其值。 |
| preReluMode | 矩阵乘前ReLU激活模式，pre_relu_t枚举类型。<br>该参数占用3bit，可以使用MmadFixBitModeParams类对象的SetPreReluMode()函数设置其值，使用GetPreReluMode()函数获取其值。 |
| postQuantMode | 矩阵乘后量化模式，post_quant_t枚举类型。<br>该参数占用5bit，可以使用MmadFixBitModeParams类对象的SetPostQuantMode()函数设置其值，使用GetPostQuantMode()函数获取其值。 |
| actPostMode | 矩阵乘后激活模式，uint8_t类型。<br>该参数占用3bit，可以使用MmadFixBitModeParams类对象的SetactPostMode()函数设置其值，使用GetactPostMode()函数获取其值。 |
| preClipReluMode | 矩阵乘前Clip ReLU模式，uint8_t类型。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetPreClipReluMode()函数设置其值，使用GetPreClipReluMode()函数获取其值。 |
| eltwiseOp | eltwise操作类型，eltwise_op_t枚举类型。<br>该参数占用3bit，可以使用MmadFixBitModeParams类对象的SetEltwiseOp()函数设置其值，使用GetEltwiseOp()函数获取其值。 |
| eltwiseAntiqEnable | 是否开启eltwise反量化，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetEltwiseAntiqEnable()函数设置其值，使用GetEltwiseAntiqEnable()函数获取其值。 |
| eltwiseBroadcastEnable | 是否开启eltwise广播，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetEltwiseBroadcastEnable()函数设置其值，使用GetEltwiseBroadcastEnable()函数获取其值。 |
| itaEnable | 是否开启ITA功能，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetItaEnable()函数设置其值，使用GetItaEnable()函数获取其值。 |
| biasS32Enable | 是否开启S32（int32_t）格式的Bias，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetBiasS32Enable()函数设置其值，使用GetBiasS32Enable()函数获取其值。 |
| hmaxEnable | 是否开启HMAX（最大值统计）功能，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetHmaxEnable()函数设置其值，使用GetHmaxEnable()函数获取其值。 |
| lsbMask | 掩码控制参数，lsb_mask_t枚举类型。<br>该参数占用3bit，可以使用MmadFixBitModeParams类对象的SetLsbMask()函数设置其值，使用GetLsbMask()函数获取其值。 |
| gemvCtrl | GEMV模式控制参数，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetGemvCtrl()函数设置其值，使用GetGemvCtrl()函数获取其值。 |
| groupMode | 分组模式，uint8_t类型。<br>该参数占用3bit，可以使用MmadFixBitModeParams类对象的SetGroupMode()函数设置其值，使用GetGroupMode()函数获取其值。 |
| dependEnable | 是否开启指令间依赖控制，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetDependEnable()函数设置其值，使用GetDependEnable()函数获取其值。 |
| instrId | 指令编号，instr_id_t枚举类型。<br>该参数占用2bit，可以使用MmadFixBitModeParams类对象的SetInstrId()函数设置其值，使用GetInstrId()函数获取其值。 |
| postProcHalfEnable | 是否开启half精度后处理，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetPostProcHalfEnable()函数设置其值，使用GetPostProcHalfEnable()函数获取其值。 |
| layoutCtrl | 布局控制参数，取值true/false。<br>该参数占用1bit，可以使用MmadFixBitModeParams类对象的SetLayoutCtrl()函数设置其值，使用GetLayoutCtrl()函数获取其值。 |
| n | 右矩阵Width（N维度），uint16_t类型。<br>该参数是位域结构体的最高位参数，占用16bit，可以使用MmadFixBitModeParams类对象的SetN()函数设置其值，使用GetN()函数获取其值。 |
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id3 -->

## 数据类型<a id="zh_cn_topic_mmadfix_section5"></a>

**表6** dst、left、right、bias支持的精度类型组合（Kirin9050处理器系列产品）

| 左矩阵left type | 右矩阵right type | bias type | 结果矩阵dst type |
| --- | --- | --- | --- |
| half | half | half | half |
| int16_t | int8_t | int32_t | int32_t |
| int8_t | int8_t | int32_t | int32_t |

## 返回值说明

无

## 约束说明

- 结果矩阵C只支持位于物理存储位置为L1 Buffer（TPosition:C1），左矩阵A只支持位于物理存储位置为L1 Buffer（TPosition:A1），右矩阵B只支持位于物理存储位置为L0B Buffer（TPosition:B2）。
- 矩阵乘的M、K维度通过SetMatrixPara接口配置，N维度通过mmadFixParams的n参数传入。
- 带bias输入的原型中，Bias矩阵的存储位置与维度由mmadFixParams中的biasMatrixBroadcast参数决定，需与bias LocalTensor的实际存储位置保持一致。
- 开启量化、激活、eltwise等后处理功能时，需配合SetFixPipeConfig等MacroCube系列接口完成对应参数（如量化参数地址、ReLU参数地址等）的配置。
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id4 -->

## 调用示例

```cpp
// Kirin9050处理器系列产品矩阵乘示例：C = A x B + Bias
// 输入：int8_t类型的左矩阵A（M x K）和右矩阵B（K x N），int32_t类型的Bias（1 x N）
// 输出：int32_t类型的结果矩阵C（M x N）
constexpr uint16_t m = 16;    // 左矩阵Height（M维度）
constexpr uint16_t k = 128;   // 左矩阵Width、右矩阵Height（K维度）
constexpr uint16_t n = 64;    // 右矩阵Width（N维度）

// 左矩阵A位于L1 Buffer（A1），右矩阵B位于L0B Buffer（B2），
// 结果矩阵C位于L1 Buffer（C1），Bias矩阵位于BT Buffer（C2）
AscendC::TBuf<AscendC::TPosition::A1> leftBuf;
AscendC::TBuf<AscendC::TPosition::B2> rightBuf;
AscendC::TBuf<AscendC::TPosition::C1> dstBuf;
AscendC::TBuf<AscendC::TPosition::C2> biasBuf;

// 1. 配置矩阵乘的M/K维度信息（N维度通过MmadFixParams的n字段传入）
AscendC::MatrixParaParams matrixParaParams(m, k, 0 /* groupK */);
AscendC::SetMatrixPara(matrixParaParams);

// 2. 配置MmadFix计算参数
AscendC::MmadFixParams mmadFixParams;
mmadFixParams.n = n;
mmadFixParams.biasMatrixInitVal = true;    // 结果矩阵初始化为0
mmadFixParams.biasMatrixBroadcast = true;  // Bias位于BT Buffer，沿M方向广播

// 3. 执行矩阵乘：C = A x B + Bias
AscendC::MmadFix(dstBuf.Get<int32_t>(), leftBuf.Get<int8_t>(), rightBuf.Get<int8_t>(),
                 biasBuf.Get<int32_t>(), mmadFixParams);
```

使用MmadFixBitModeParams的调用方式（适用于scalar流水成为性能优化瓶颈的场景）：

```cpp
// 方式一：直接使用MmadFixParams对象构造MmadFixBitModeParams
AscendC::MmadFixBitModeParams mmadFixBitModeParams(mmadFixParams);

// 方式二：使用Set函数逐项配置参数（与方式一二选一）
// AscendC::MmadFixBitModeParams mmadFixBitModeParams;
// mmadFixBitModeParams.SetN(n);
// mmadFixBitModeParams.SetBiasMatrixInitVal(true);
// mmadFixBitModeParams.SetBiasMatrixBroadcast(true);

AscendC::MmadFix(dstBuf.Get<int32_t>(), leftBuf.Get<int8_t>(), rightBuf.Get<int8_t>(),
                 biasBuf.Get<int32_t>(), mmadFixBitModeParams);
```
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id5 -->
