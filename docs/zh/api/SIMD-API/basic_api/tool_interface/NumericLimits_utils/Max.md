# Max

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/Max_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_limits_intf.h"`。

返回指定数据类型的最大有限值。

## 函数原型

- 标量接口，返回值为标量：

    ```cpp
    constexpr __aicore__ static inline T Max()
    ```

- 矢量接口，为dst前count个元素赋最大有限值：

    ```cpp
    __aicore__ static inline void Max(const LocalTensor<T>& dst, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 输入数据元素个数。 |

## 数据类型

<!-- npu="950" id8 -->
针对Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id8 -->

## 返回值说明

标量接口返回值为对应数据类型的最大有限值。

矢量接口无返回值。

**表 2**  Max各数据类型输出

| 数据类型 | 十进制数 | 二进制各bit位 |
| --- | --- | --- |
| uint8_t | 255 | 1111 1111 |
| int8_t | 127 | 0111 1111 |
| uint16_t | 65535 | 1111 1111 1111 1111 |
| int16_t | 32767 | 0111 1111 1111 1111 |
| half | 65504 | 0 11110 1111111111 |
| bfloat16_t | 3.4e+38 | 0 11111110 1111111 |
| uint32_t | 4294967295 | 1111 1111 1111 1111 1111 1111 1111 1111 |
| int32_t | 2147483647 | 0111 1111 1111 1111 1111 1111 1111 1111 |
| float | 3.4e+38 | 0 11111110 11111111111111111111111 |

## 约束说明

标量接口无约束，矢量接口使用[duplicate](../../memory_vector_compute/data_padding/Duplicate.md)填充前count个元素赋最大有限值，参照Memory矢量计算下的数据填充接口duplicate约束。

## 调用示例

- 标量接口：

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Max();
    ```

- 矢量接口：

    ```cpp
    AscendC::NumericLimits<int32_t>::Max(dstLocal, 256); // 返回256个int32_t类型的最大值
    ```
