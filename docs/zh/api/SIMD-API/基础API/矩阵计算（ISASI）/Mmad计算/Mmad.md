# Mmad<a name="ZH-CN_TOPIC_00000025382311899"></a>

## 产品支持情况

### 不传入bias的原型

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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

### 传入bias的原型

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90：支持
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030：不支持
<!-- end id18 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

Mmad接口是Ascend C面向昇腾AI芯片的矩阵乘加核心计算接口，专为高性能算子开发设计，封装了昇腾NPU硬件的矩阵乘加计算能力，广泛用于神经网络层（如全连接层、卷积层）、数值计算类算子的开发。

Mmad接口实现昇腾NPU矩阵乘计算能力，其数学表达式为：

$$C = A \times B + C$$

<!-- npu="950" id19 -->
针对Ascend 950PR/Ascend 950DT：

**表1** 矩阵计算矩阵A、B、C解释说明

| 矩阵计算逻辑 | 矩阵计算物理位置 | 维度 | 输入/输出数据格式 | 数据类型 |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Nz | <a href="#zh_cn_topic_mmad_section5">数据类型</a> |
| B | L0B Buffer | K x N | Zn | <a href="#zh_cn_topic_mmad_section5">数据类型</a> |
| C | L0C Buffer | M x N，可支持使用偏置矩阵Bias进行初始化，维度为1 x N | Nz | <a href="#zh_cn_topic_mmad_section5">数据类型</a> |

**图1** Mmad接口矩阵乘分形示意图（Ascend 950PR/Ascend 950DT）<a id="zh_cn_topic_mmad_section2_figure1"></a>

![Mmad接口计算分形示意图](../../../../figures/mmad_demo_a5.png)
<!-- end id19 -->

<!-- npu="A3,910b" id20 -->
针对如下产品型号，矩阵说明见下表：
<!-- npu="A3" id21 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id21 -->
<!-- npu="910b" id22 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id22 -->
**表2** 矩阵计算矩阵A、B、C解释说明

  <table>
    <thead>
      <tr>
        <th>矩阵计算逻辑</th>
        <th>矩阵计算物理位置</th>
        <th>维度</th>
        <th>输入/输出数据格式</th>
        <th>数据类型</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>Zz</td>
        <td rowspan="3"><a href="#zh_cn_topic_mmad_section5">数据类型</a></td>
      </tr>
      <tr>
        <td>B</td>
        <td>L0B Buffer</td>
        <td>K x N</td>
        <td>Zn</td>
      </tr>
      <tr>
        <td>C</td>
        <td>L0C Buffer</td>
        <td>M x N，可支持使用偏置矩阵Bias进行初始化，维度为1 x N</td>
        <td>Nz</td>
      </tr>
    </tbody>
  </table>
<!-- end id20 -->

## 函数原型

- 不传入bias

    ```cpp
    template <typename T, typename U, typename S>
    __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
    ```

- 传入bias

    ```cpp
    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadParams& mmadParams)
    ```

## 参数说明

**表3** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，结果矩阵C，类型为LocalTensor，支持的物理存储位置为L0C Buffer（TPosition:CO1）。<br>LocalTensor的起始地址需要按照1024字节对齐。 |
| fm | 输入 | 源操作数，左矩阵A，类型为LocalTensor，支持的物理存储位置为L0A Buffer（TPosition: A2）。<br>LocalTensor的起始地址需要按照512字节对齐。 |
| filter | 输入 | 源操作数，右矩阵B，类型为LocalTensor，支持的物理存储位置为L0B Buffer（TPosition: B2）。<br>LocalTensor的起始地址需要按照512字节对齐。 |
| bias | 输入 | 源操作数，Bias矩阵，类型为LocalTensor，支持的物理存储位置为BT Buffer（TPosition:C2）。<br>LocalTensor的起始地址需要按照64字节对齐。 |
| mmadParams | 输入 | 矩阵乘相关参数。<br>该参数类型的具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>MmadParams参数说明请参考[表4](#zh_cn_topic_mmad_section4_table4)。 |

**表4** MmadParams结构体内参数说明<a id="zh_cn_topic_mmad_section4_table4"></a>

| 参数名称 | 含义 |
| --- | --- |
| m | 左矩阵Height，取值范围：m∈[0, 4095]。默认值为0。 |
| n | 右矩阵Width，取值范围：n∈[0, 4095]。默认值为0。 |
| k | 左矩阵Width、右矩阵Height，取值范围：k∈[0, 4095]。默认值为0。 |
| cmatrixInitVal | 是否开启C矩阵默认初始化清零操作。默认值为 true。<br>&nbsp;&nbsp;&bull; true：C矩阵默认初始化为0；<br>&nbsp;&nbsp;&bull; false：C矩阵不进行默认操作，通过设置cmatrixSource参数进行初始化。 |
| cmatrixSource | 配置C矩阵初始值是否来源于BT Buffer。默认值为false。<br>&nbsp;&nbsp;&bull; false：不对L0C Buffer进行初始化操作；<br>&nbsp;&nbsp;&bull; true：使用BT Buffer(TPosition:C2)的数据对L0C Buffer进行初始化操作。<br><br>Atlas 训练系列产品，仅支持配置为false。<br><br>Atlas 推理系列产品AI Core，仅支持配置为false。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持配置为true/false。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持配置为true/false。<br><br>Atlas 200I/500 A2 推理产品，支持配置为true/false。<br><br>Ascend 950PR/Ascend 950DT，支持配置为true/false。<br><br>Kirin X90仅支持配置为false。<br><br>Kirin 9030仅支持配置为false。<br><br>注意：带Bias输入的接口配置该参数无效，会根据bias输入的位置来判断C矩阵初始值是否来源于BT Buffer。 |
| isBias | 该参数废弃，新开发内容不要使用该参数。如果需要累加初始矩阵，请使用带Bias的接口来实现；也可以通过cmatrixInitVal和cmatrixSource参数配置C矩阵的初始值来源来实现。推荐使用带Bias的接口，相比于配置cmatrixInitVal和cmatrixSource参数更加简单方便。<br><br>配置是否需要累加初始矩阵，默认值为false，取值说明如下：<br>&nbsp;&nbsp;&bull; false：矩阵乘，无需累加初始矩阵，C = A \* B。<br>&nbsp;&nbsp;&bull; true：矩阵乘加，需要累加初始矩阵，C += A \* B。 |
| disableGemv | M=1时，该参数用来配置Mmad计算是否开启[GEMV](../Mmad计算关键特性说明/GEMV.md#ZH-CN_TOPIC_0000002538231187)模式。<br>&nbsp;&nbsp;&bull; false：开启GEMV模式。<br>&nbsp;&nbsp;&bull; true：关闭GEMV模式。<br><br>该参数仅支持如下型号：<br><br>Ascend 950PR/Ascend 950DT |
| unitFlag | unitFlag可以控制Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br><br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br><br>该参数仅支持如下型号：<br><br>Ascend 950PR/Ascend 950DT<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品。参数设置方案和特性细节可参考：[UnitFlag特性说明](../Mmad计算关键特性说明/UnitFlag.md#ZH-CN_TOPIC_00000025690709788) |
| kDirectionAlign | K方向对齐的核心功能是通过`kDirectionAlign`参数控制在使用float数据类型时，L0A Buffer和L0B Buffer矩阵在K方向上的对齐方式。<br><br>取值说明如下：<br>&nbsp;&nbsp;&bull; false：默认值，K方向对齐到`ceil(K / 8) * 8`。<br>&nbsp;&nbsp;&bull; true：K方向对齐到`ceil(K/16)*16`。<br><br>Atlas 训练系列产品，仅支持配置为false。<br><br>Atlas 推理系列产品AI Core，仅支持配置为false。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，仅支持配置为true/false。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，仅支持配置为true/false。<br><br>Atlas 200I/500 A2 推理产品，仅支持配置为false。<br><br>Ascend 950PR/Ascend 950DT，仅支持配置为false。<br><br>Kirin X90支持配置为true/false。<br><br>Kirin 9030支持配置为true/false。<br><br>特性细节可参考：[kDirectionAlign特性说明](../Mmad计算关键特性说明/K-方向对齐约束.md#ZH-CN_TOPIC_0000002569070973)。 |
| fmOffset | 左矩阵offset（整个左矩阵对应一个值），支持Scalar（应与src_fm.dtype一致）/立即数，默认0。<br><br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enSsparse | 开启结构化稀疏特性，默认false；<br><br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enWinogradA | 指示矩阵a是否通过winograd_feature_map_transform()生成，用于支持winograd特性，bool类型，默认false；<br><br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enWinogradB | 指示矩阵b是否通过winograd_weight_transform()生成，用于支持winograd特性，bool类型，默认false；<br><br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |

## 数据类型<a id="zh_cn_topic_mmad_section5"></a>

<!-- npu="950" id23 -->
**表5** dst、fm、filter支持的精度类型组合（Ascend 950PR/Ascend 950DT）

| 左矩阵fm type | 右矩阵filter type | 结果矩阵dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | float |
| fp8_e4m3fn_t | fp8_e5m2_t | float |
| fp8_e5m2_t | fp8_e4m3fn_t | float |
| fp8_e5m2_t | fp8_e5m2_t | float |
| hifloat8_t | hifloat8_t | float |

**表6** dst、fm、filter、bias支持的精度类型组合（Ascend 950PR/Ascend 950DT）

| 左矩阵fm type | 右矩阵filter type | bias type | 结果矩阵dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | float | float |
| fp8_e4m3fn_t | fp8_e5m2_t | float | float |
| fp8_e5m2_t | fp8_e4m3fn_t | float | float |
| fp8_e5m2_t | fp8_e5m2_t | float | float |
| hifloat8_t | hifloat8_t | float | float |
<!-- end id23 -->

<!-- npu="A3,910b,310b" id24 -->
**表7** dst、fm、filter支持的精度类型组合（Atlas A2 训练系列产品/Atlas A2 推理系列产品）（Atlas A3 训练系列产品/Atlas A3 推理系列产品）（Atlas 200I/500 A2 推理产品）

| 左矩阵fm type | 右矩阵filter type | 结果矩阵dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| int4b_t | int4b_t | int32_t |

**表8** dst、fm、filter、bias支持的精度类型组合（Atlas A2 训练系列产品/Atlas A2 推理系列产品）（Atlas A3 训练系列产品/Atlas A3 推理系列产品）（Atlas 200I/500 A2 推理产品）

| 左矩阵fm type | 右矩阵filter type | bias type | 结果矩阵dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |
<!-- end id24 -->

<!-- npu="310p" id25 -->
**表9** dst、fm、filter支持的精度类型组合（Atlas 推理系列产品AI Core）

| 左矩阵fm type | 右矩阵filter type | 结果矩阵dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| uint8_t | uint8_t | int32_t |
| half | half | half<br> 说明： <br>该精度类型组合，精度无法达到双千分之一，且后续处理器版本不支持该类型转换，建议直接使用half输入float输出。<br><br>双千分之一是指每个实际数据和真值数据之间的误差不超过千分之一，误差超过千分之一的数据总和不超过总数据数的千分之一。 |
| half | half | float |
| int4b_t | int4b_t | int32_t |
<!-- end id25 -->

<!-- npu="910" id26 -->
**表10** dst、fm、filter支持的精度类型组合（Atlas 训练系列产品）

| 左矩阵fm type | 右矩阵filter type | 结果矩阵dst type |
| --- | --- | --- |
| uint8_t | uint8_t | uint32_t |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| half | half | half<br> 说明： <br>该精度类型组合，精度无法达到双千分之一，且后续处理器版本不支持该类型转换，建议直接使用half输入float输出。<br><br>双千分之一是指每个实际数据和真值数据之间的误差不超过千分之一，误差超过千分之一的数据总和不超过总数据数的千分之一。 |
| half | half | float |
<!-- end id26 -->

<!-- npu="x90" id27 -->
**表11** dst、fm、filter、bias支持的精度类型组合（Kirin X90）

| 左矩阵fm type | 右矩阵filter type | bias type | 结果矩阵dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | half | half |
<!-- end id27 -->

<!-- npu="9030" id28 -->
**表12** dst、fm、filter、bias支持的精度类型组合（Kirin 9030）

| 左矩阵fm type | 右矩阵filter type | bias type | 结果矩阵dst type |
| --- | --- | --- | --- |
| half | half | half | half |
<!-- end id28 -->

## 返回值说明

无

## 约束说明

- 结果矩阵C只支持位于L0C Buffer（CO1），左矩阵A只支持位于L0A Buffer（A2），右矩阵B只支持位于L0B Buffer（B2）。
- 当M、K、N中的任意一个值为0时，表示指令不会执行，该接口将被视为NOP（空操作）。
- 当M = 1时，会默认开启GEMV（General Matrix-Vector Multiplication）功能。在这种情况下，Mmad API从L0A Buffer读取数据时，会以ND格式进行读取，而不会将其视为ZZ或NZ格式。所以此时左矩阵需要直接按照ND格式进行排布（[GEMV特性说明](../Mmad计算关键特性说明/GEMV.md#ZH-CN_TOPIC_0000002538231187)）。要注意的是，开启GEMV的情况下，A矩阵的起始地址仍要求512字节对齐。<!-- npu="950" id29 -->针对Ascend 950PR/Ascend 950DT产品，可以通过设置MmadParams的disableGemv参数为true，将该功能关闭。<!-- end id29 -->
- 一般来说，一次Mmad计算至少完成一次A(16\*16\*half) \* B(16\*16\*half)数据块计算。但实际计算时M、K、N有效值可能不是16的倍数，从而有部分数据为无效数据。以M=30，K=70，N=40为例来介绍无效数据与有效数据的排布方式。

    数据为half类型，当M=30，K=70，N=40的时候，L0A Buffer（A2）中有2x5个16x16矩阵，L0B Buffer（B2）中有5x3个16x16矩阵，L0C Buffer（CO1）中有2x3个16x16矩阵。在这种场景下M、K和N都不是16的倍数，A2中右下角的矩阵实际有效的数据只有14x6个，但是也需要占一个16x16矩阵的空间，其他无效数据在计算中会被忽略。一个16x16分形的数据块中，无效数据与有效数据排布的方式示意如下：

    ![有效数据相乘示意图](../../../../figures/mmad_validshape_demo.png)

- **同步优化说明**

    当矩阵计算沿K轴进行累加时，中间结果通常需要写入同一块L0C Buffer。理论上，在连续两次Mmad接口调用之间，应插入PipeBarrier\(PIPE\_M\)，以确保前一次Mmad的写操作已完成，再进行下一轮Mmad的读操作，从而保证数据一致性。

    然而，当矩阵维度m和n超过一定阈值时，由于计算量足够大，数据访问模式具备良好的局部性和流水线并行潜力，硬件调度器能够自动处理写-读依赖问题。此时无需插入PipeBarrier\(PIPE\_M\)，Mmad指令可并行执行，从而显著提升计算性能。

    阈值公式：$$(m / 16) * (n / 16) < 10$$

    当计算量小于阈值时，需要插入PipeBarrier\(PIPE\_M\)。

    ```cpp
    if ((m / 16) * (n / 16) < 10) {
        AscendC::PipeBarrier<PIPE_M>();
    }
    ```

    当计算量大于阈值时，不需要插入PipeBarrier\(PIPE\_M\)。

- **UnitFlag特性约束说明**

    Mmad和Fixpipe接口均提供了 \`unitFlag\`参数来控制该功能的启用，需确保两者同步开启，才能正常生效。

    当希望控制同一块L0C Buffer内存空间能持续只被多条Mmad或多条Fixpipe指令操作时，需将对应的前n-1条指令的unitFlag值设置为2，维持被操作内存空间的持续占用状态，最后一条指令设置为3，解除被占用状态。

    当启用 \`unitFlag\`功能后，建议Mmad的计算数据量与Fixpipe搬出的数据量保持一致。若Mmad计算了大块数据（M × N = 128 × 128），但Fixpipe只搬出了其中一部分数据（M × N = 64 × 64），则可能会导致执行异常，可以通过SetFixPipeConfig\(\)接口重置L0C Buffer的状态，详细操作方式见[UnitFlag特性说明](../Mmad计算关键特性说明/UnitFlag.md#ZH-CN_TOPIC_00000025690709788)中的示例。

- **特殊值/边界值约束说明**

  对于浮点类型inf/nan输入输出，可以通过CTRL寄存器（控制寄存器）的CTRL\[48\]比特位进行设置，控制浮点数计算时的饱和模式：

  - 非饱和模式：CTRL\[48\]设置成1'b1，inf/nan保持原输出。
  - 饱和模式：CTRL\[48\]设置成1'b0，inf输出会被饱和为±MAX，nan输出会被饱和为0。

    ```cpp
    // 设置CTRL[48]为0，开启浮点数计算饱和模式
    AscendC::AscendCUtils::SetOverflow(0);
    ```

    注：Mmad指令应避免NaN输入，否则可能会产生执行报错。

  对于整数类型只有饱和模式。

## 调用示例

矩阵乘的样例请参考[Mmad样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad)。
