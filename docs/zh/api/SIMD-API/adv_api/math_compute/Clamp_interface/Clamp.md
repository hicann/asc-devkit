# Clamp

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Clamp_interface/Clamp_res.md#id1 -->

## 功能说明

将输入中除nan值以外大于max的数替换为max，小于min的数替换为min，小于等于max和大于等于min的数保持不变，作为输出。当min大于max时，将除nan值外所有值替换为max。min和max可以为标量或LocalTensor。

![](../../../../figures/zh-cn_formulaimage_0000002386121366.png)

![](../../../../figures/zh-cn_formulaimage_0000002419939453.png)

## 函数原型

```
template <const ClampConfig& config = DEFAULT_CLAMP_CONFIG, typename T, typename U, typename S>
__aicore__ inline void Clamp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& min, const S& max, const uint32_t count)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| config | Clamp算法的相关配置。此参数可选配，ClampConfig类型，具体定义如下方代码所示，其中参数的含义为：<br><br>isReuseSource：该参数预留，传入默认值false即可。 |
| T | 操作数的数据类型。支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。 |
| U | LocalTensor类型或标量类型。根据输入参数min自动推导类型，开发者无需配置该参数，保证min满足数据类型的约束即可。<br><br>支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。 |
| S | LocalTensor类型或标量类型。根据输入参数max自动推导类型，开发者无需配置该参数，保证max满足数据类型的约束即可。<br><br>支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。 |

```
struct ClampConfig {
    bool isReuseSource;
};
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型与目的操作数保持一致。 |
| min | 输入 | 数据下限。类型为标量或LocalTensor，类型为LocalTensor时，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。 |
| max | 输入 | 数据上限。类型为标量或LocalTensor，类型为LocalTensor时，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>数据类型与目的操作数保持一致。 |
| count | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

## 调用示例

更多示例请参考[Clamp算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/clamp)。

```
AscendC::LocalTensor<half> dst, src;
uint32_t count = 512;
half min = 30;
half max = 60;
AscendC::Clamp(dst, src, min, max, count);
```

结果示例如下：

```
输入数据（src）:
[13, 78, 35, 95, 83,  2,  2, 95, 51, 73, 98,  3, 55, 32, 61,  2, 40, 26, 95, ... 63]
输入数据（min）:
[30]
输入数据（max）:
[60]
输出数据（dst）:
[30, 60, 35, 60, 60, 30, 30, 60, 51, 60, 60, 30, 55, 32, 60, 30, 40, 30, 60, ... 60]
```
