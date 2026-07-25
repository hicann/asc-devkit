# Ln<a name="ZH-CN_TOPIC_0000001429283377"></a>

## 产品支持情况<a name="section1550532418810"></a>

### 不传入config的原型

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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

### 传入config的原型

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：不支持
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
- Kirin X90：不支持
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030：不支持
<!-- end id18 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_unary_intf.h"`。

Ln属于单目矢量类计算接口，负责将输入的tensor按元素取自然对数。计算公式如下：

$dst_i = \ln(src_i)$

## 函数原型<a name="section620mcpsimp"></a>

- 传入config的原型

    - tensor前n个数据连续计算

        ```cpp
        template <typename T, const LnConfig& config = DEFAULT_LN_CONFIG>
        __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - tensor高维切分计算
        - mask逐bit模式

            ```cpp
            template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - mask连续模式

            ```cpp
            template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

- 不传入config的原型

    - tensor前n个数据连续计算

        ```cpp
        template <typename T>
        __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
        ```

    - tensor高维切分计算
        - mask逐bit模式

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

        - mask连续模式

            ```cpp
            template <typename T, bool isSetMask = true>
            __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
            ```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
|---|---|
| T | 操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |
| <!-- npu="950" id19 -->config | 该参数仅支持Ascend 950PR/Ascend 950DT。<br>用于配置Subnormal计算模式，LnConfig类型，定义如下：<br><pre>enum&nbsp;class&nbsp;LnAlgo&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;INTRINSIC&nbsp;=&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_TRUE,<br>&nbsp;&nbsp;&nbsp;&nbsp;PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct&nbsp;LnConfig&nbsp;{<br>&nbsp;&nbsp;&nbsp;&nbsp;LnAlgo&nbsp;algo&nbsp;=&nbsp;LnAlgo::INTRINSIC;<br>};</pre>通过LnConfig结构体的参数algo来配置Subnormal计算模式。algo取值如下：<br>&bull; LnAlgo::INTRINSIC、LnAlgo::PRECISION_1ULP_FTZ_TRUE，使用单指令计算得出结果，所有Subnormal被近似为0。<br>&bull; LnAlgo::PRECISION_1ULP_FTZ_FALSE，支持Subnormal数据计算。<br>该参数的默认值DEFAULT_LN_CONFIG的取值如下：<br><pre>constexpr&nbsp;LnConfig&nbsp;DEFAULT_LN_CONFIG&nbsp;=&nbsp;{&nbsp;LnAlgo::INTRINSIC&nbsp;};</pre><br>调用本原型时若不显式传入config参数，则默认使用DEFAULT_LN_CONFIG，此时行为与不传入config参数的原型等价。<!-- end id19 --> |

**表2** 参数说明

<a name="table1055216132132"></a>

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| dst | 输出 | 目的操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。 |
| src | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。 |
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体说明请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[UnaryRepeatParams](../../辅助数据结构/UnaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

T支持的数据类型为：half、float。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md)。

<!-- npu="A3,910b,950" id20 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id26 -->
  - 针对如下型号，当参数count或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id27 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id27 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id29 -->
  <!-- end id26 -->
  <!-- npu="950" id30 -->
  - 针对Ascend 950PR/Ascend 950DT：该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，软仿行为不保证该接口被视为NOP（空操作）。
  <!-- end id30 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算占用8KB Unified Buffer。
  - tensor前n个数据连续计算不涉及8KB Unified Buffer的占用。
<!-- end id21 -->

<!-- npu="950" id24 -->
## 关键特性<a name="section18972943153217"></a>

针对Ascend 950PR/Ascend 950DT，有如下关键特性：

**最大精度误差**：

- LnAlgo::INTRINSIC、LnAlgo::PRECISION\_1ULP\_FTZ\_TRUE，最大精度误差为1ulp。
- LnAlgo::PRECISION\_1ULP\_FTZ\_FALSE，软仿实现，最大精度误差为1ulp。

**配置Subnormal模式**：

FTZ（Flush To Zero）：一种浮点运算模式，当结果为[Subnormal](../../数据结构/内置数据类型.md#p7381131713310)时，将其直接清零（近似为0），而非保留其精确的微小数值。

只有将algo设置为LnAlgo::PRECISION\_1ULP\_FTZ\_false时，Ln接口才会保留并正确输出Subnormal结果；其他模式下Subnormal均被FTZ。

由于Subnormal的计算行为是通过软件仿真算法实现，一般场景推荐使用性能更好的LnAlgo::INTRINSIC、LnAlgo::PRECISION\_1ULP\_FTZ\_TRUE；需要精确Subnormal输出的场景（如特定数据精度要求的算法、避免除零错误）使用LnAlgo::PRECISION\_1ULP\_FTZ\_FALSE。
<!-- end id24 -->

## 调用示例<a name="section176061616102911"></a>

- tensor高维切分计算样例-mask连续模式

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Ln(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor高维切分计算样例-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Ln(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor前n个数据计算接口样例

    ```cpp
    AscendC::Ln(dstLocal, srcLocal, 512);
    ```

    <!-- npu="950" id25 -->
    以下示例仅支持Ascend 950PR/Ascend 950DT

    ```cpp
    static constexpr LnConfig config = { LnAlgo::PRECISION_1ULP_FTZ_FALSE };
    AscendC::Ln<T, config>(dstLocal, srcLocal, 512);
    ```
    <!-- end id25 -->

结果示例如下：

```bash
输入数据srcLocal：[1 2 3 4 ...]
输出数据dstLocal：[0 0.6931 1.0986 1.3863 ...]
```
