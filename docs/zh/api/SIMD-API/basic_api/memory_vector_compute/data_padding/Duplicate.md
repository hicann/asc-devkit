# Duplicate<a name="ZH-CN_TOPIC_0000002543264872"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_duplicate_intf.h"`。

Duplicate接口将一个变量或立即数复制多次并填充到向量中。

接口支持使用前n个数据计算和高维切分计算的方式，前n个数据计算原理和参考伪代码如下：

```python
import numpy as np

def Duplicate(scalarValue, dst, count):
    for i in range(count):     
        dst[i] = scalarValue
```
<!-- npu="950" id26 -->
针对Ascend 950PR/Ascend 950DT，为方便开发者使用，tensor前n个数据计算接口同时也支持直接传入LocalTensor，此时会将LocalTensor的第一个元素复制多次并填充到向量中。
<!-- end id26 -->

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算，源操作数为标量

    ```cpp
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count)
    ```

<!-- npu="950" id10 -->

- tensor前n个数据计算，源操作数为LocalTensor

    ```cpp
    // 该函数原型，仅支持Ascend 950PR/Ascend 950DT。
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```
<!-- end id10 -->

- tensor高维切分计算，mask逐比特模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

- tensor高维切分计算，mask连续模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数说明 | 描述 |
| ------ | ------ |
| T | 操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../mask_operations/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  接口参数说明
| 参数说明 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要按照32字节对齐。|
| scalarValue | 输入 | 被复制的源操作数，数据类型需与dst中元素的数据类型保持一致。|
|<!-- npu="950" id11 --> src | 输入| 源操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>数据类型需与dst中元素的数据类型保持一致。<br>当传入该参数时，会将src[0]复制多次并填充到向量中。<!-- end id11 -->|
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD_compute/continuous_compute.md)。|
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。<br>设置详见[掩码操作](../mask_operations/mask_operations.md)。 |
| repeatTime | 输入 | 指令迭代次数，每次迭代完成8个DataBlock的数据收集，取值范围：repeatTime∈[0,255]。|
| dstBlockStride | 输入 | 单次迭代内，矢量目的操作数不同datablock间地址步长，单位为DataBlock。 |
| dstRepeatStride | 输入 | 相邻迭代间，矢量目的操作数相同datablock地址步长，单位为DataBlock。 |

## 数据类型

<!-- npu="950" id12 -->
- 针对Ascend 950PR/Ascend 950DT，T支持的数据类型为：bool、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、complex64。其中，bool、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、fp8_e8m0_t、complex32、int64_t、uint64_t、complex64数据类型仅支持tensor前n个数据计算接口。
<!-- end id12 -->
<!-- npu="A3" id13 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id13 -->
<!-- npu="910b" id14 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id14 -->
<!-- npu="310b" id15 -->
- 针对Atlas 200I/500 A2 推理产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id15 -->
<!-- npu="310p" id16 -->
- 针对Atlas 推理系列产品AI Core，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id16 -->
<!-- npu="910" id17 -->
- 针对Atlas 训练系列产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id17 -->
<!-- npu="x90" id18 -->
- 针对Kirin X90，T支持的数据类型为：half。
<!-- end id18 -->
<!-- npu="9030" id19 -->
- 针对Kirin 9030，T支持的数据类型为：half。
<!-- end id19 -->

## 返回值说明<a name="section640mcpsimp"></a>	 

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
<!-- npu="A3,910b,950" id20 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id21 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id22 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id23 -->
  <!-- end id21 -->
  <!-- npu="950" id24 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../reg_vector_compute/reg_vector_compute.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id24 -->
<!-- end id20 -->

## 调用示例<a name="section642mcpsimp"></a>

本调用示例中只展示完整样例中的接口部分代码。如果您需要运行示例代码，请直接参考完整样例[Duplicate样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/duplicate)进行编译执行。

- tensor高维切分计算样例，mask连续模式

    ```cpp
    uint64_t mask = 128;
    half scalar = 18.0;
    // repeatTime = 2, 128 elements one repeat, 256 elements total
    // dstBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride = 8, no gap between repeats
    AscendC::Duplicate(dstLocal, scalar, mask, 2, 1, 8 );
    ```

- tensor高维切分计算样例，mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    half scalar = 18.0;
    // repeatTime = 2, 128 elements one repeat, 256 elements total
    // dstBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride = 8, no gap between repeats
    AscendC::Duplicate(dstLocal, scalar, mask, 2, 1, 8 );
    ```

- tensor前n个数据计算样例，源操作数为标量

    ```cpp
    half inputVal(18.0);
    int32_t srcDataSize = 256; // 参与计算的元素个数
    AscendC::Duplicate<half>(dstLocal, inputVal, srcDataSize);
    ```

<!-- npu="950" id25 -->

- tensor前n个数据计算样例，源操作数为Tensor
    ```cpp
    AscendC::Duplicate<half>(dstLocal, srcLocal, srcDataSize);
    ```
<!-- end id25 -->
结果示例如下：

```plain
scalar: 18.0
srcLocal: [18.0 1.0 2.0 ... 254.0 255.0]
dstLocal: [18.0 18.0 18.0 ... 18.0 18.0]
```
