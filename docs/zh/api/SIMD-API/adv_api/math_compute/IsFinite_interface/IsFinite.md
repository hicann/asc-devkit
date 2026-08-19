# IsFinite

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/IsFinite_interface/IsFinite_res.md#id1 -->

## 功能说明

按元素判断输入的浮点数是否非NAN、非±INF，输出结果为浮点数或者布尔值。对于非NAN或者非±INF的输入数据，当输出为浮点类型时，对应位置的结果为该浮点类型的1，反之为0；当输出为bool类型时，对应位置的结果为true，反之为false。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002218496437.png)

-   当输出为浮点类型时：

    ![](../../../../figures/zh-cn_formulaimage_0000002224017961.png)

-   当输出为bool类型时：

    ![](../../../../figures/zh-cn_formulaimage_0000002224061861.png)

## 函数原型

```
template<typename T, typename U>
__aicore__ inline void IsFinite(const LocalTensor<U>& dst, const LocalTensor<T>& src, uint32_t calCount)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 源操作数的数据类型。支持的数据类型为：half、bfloat16_t、float。 |
| U | 目的操作数的数据类型。支持的数据类型为：bool、half、bfloat16_t、float。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>目的操作数的数据类型和源操作数保持一致，或者目的操作数的数据类型为bool类型。当前支持的数据类型组合请见[表3](#tab3)。 |
| src | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| calCount | 输入 | 参与计算的元素个数。 |

**表3**  输入输出支持的数据类型组合<a name="tab3"></a>

| srcDtype | dstDtype |
| --- | --- |
| half | half |
| half | bool |
| float | float |
| float | bool |
| bfloat16_t | bfloat16_t |
| bfloat16_t | bool |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址偏移对齐要求请参见[通用说明和约束](../../../general_description_and_constraints.md)。

## 调用示例

完整算子样例请参考[isfinite算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/isfinite)。

```
// dstLocal: 存放IsFinite计算结果的Tensor
// srcLocal：存放IsFinite计算的输入Tensor
// 需要参与计算的元素个数为8
AscendC::IsFinite(dstLocal, srcLocal, 8);
```

结果示例如下：

```
输入的数据类型为float，输出的数据类型为bool
输入数据(src):
[1.0,+inf,3.0,4.0,nan,6.0,-inf,8.0]
输出数据(dst):
[true,false,true,true,false,true,false,true]
```
