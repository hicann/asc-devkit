# QuietNaN

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/QuietNaN_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_limits_intf.h"`。

返回指定数据类型的[安静NaN值](../../data_structures/builtin_data_types.md)（浮点尾数最高位为1）。

## 函数原型

- 标量接口，返回值为标量：

    ```cpp
    constexpr __aicore__ static inline T QuietNaN()
    ```

- 矢量接口，为dstLocal前count个元素赋安静NaN值：

    ```cpp
    __aicore__ static inline void QuietNaN(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstLocal | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 输入数据元素个数。 |

## 数据类型

<!-- npu="950" id8 -->
针对Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16_t、float。
<!-- end id8 -->

## 返回值说明

标量接口返回值为对应数据类型的安静NaN值。

矢量接口无返回值。

**表 2**  QuietNaN各数据类型输出

| 数据类型 | 十进制数 | 二进制各bit位 |
| --- | --- | --- |
| half | nan | 0 11111 1000000000 |
| bfloat16_t | nan | 0 11111111 1000000 |
| float | nan | 0 11111111 10000000000000000000000 |

## 约束说明

标量接口无约束，矢量接口使用[duplicate](../../memory_vector_compute/data_padding/Duplicate.md)填充前count个元素赋安静NaN值，参照Memory矢量计算下的数据填充接口duplicate约束。

## 调用示例

- 标量接口：

    ```cpp
    float value = AscendC::NumericLimits<float>::QuietNaN();
    ```

- 矢量接口：

    ```cpp
    AscendC::NumericLimits<float>::QuietNaN(dstLocal, 256); // 返回256个float类型的NaN值
    ```
