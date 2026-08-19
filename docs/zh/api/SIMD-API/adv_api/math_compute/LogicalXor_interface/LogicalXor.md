# LogicalXor

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/LogicalXor_interface/LogicalXor_res.md#id1 -->

## 功能说明

按元素进行逻辑异或操作。当输入的数据类型不是bool时，零被视为False，非零数据被视为True。

![](../../../../figures/zh-cn_formulaimage_0000002475452485.png)

## 函数原型

```
template <const LogicalXorConfig& config = DEFAULT_LOGICAL_XOR_CONFIG, typename T, typename U>
__aicore__ inline void LogicalXor(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| config | LogicalXor算法的相关配置。此参数可选配，LogicalXorConfig类型，具体定义如下方代码所示，其中参数的含义是：<br><br>isReuseSource：该参数预留，传入默认值false即可。 |
| T | 目的操作数的数据类型。支持的数据类型为：bool。 |
| U | 源操作数的数据类型。支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。 |

```
struct LogicalXorConfig {
    bool isReuseSource;
};
```

**表2**  参数说明

| 参数名称 | 类型 | 说明 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src0、src1 | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| count | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址偏移对齐要求请参见[通用说明和约束](../../../general_description_and_constraints.md)。

## 调用示例

```
AscendC::LocalTensor<uint32_t> src0, src1;
AscendC::LocalTensor<bool> dst;
uint32_t count = 512; // 参与计算的元素个数
AscendC::LogicalXor(dst, src0, src1, count);
```

结果示例如下：

```
输入数据（src0）:
[-4, 4, 1, -8, 0, ... 3]
输入数据（src1）:
[-1, 3, 1, 5, 1, ... -3]
输出数据（dst）:
[False, False, False, False, True, ... False]
```
