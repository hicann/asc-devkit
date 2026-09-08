# BesselI0

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明

本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

按元素计算第一类零阶修正贝塞尔函数 I₀(x)（modified Bessel function of the first kind, order 0），计算公式如下：

```text
I0(x) = Σ_{k=0}^{+∞} (1 / (k!)^2) * (x/2)^{2k}
```

## 函数原型

- 通过sharedTmpBuffer入参传入临时空间
    - 源操作数Tensor全部/部分参与计算

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    - 源操作数Tensor全部参与计算

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

- 接口框架申请临时空间
    - 源操作数Tensor全部/部分参与计算

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    - 源操作数Tensor全部参与计算

        ```cpp
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

该接口的计算不需要临时空间，通过sharedTmpBuffer入参传入临时空间的重载中，sharedTmpBuffer可传入任意合法 `LocalTensor<uint8_t>`。临时空间大小BufferSize的获取方式请参考[GetBesselI0MaxMinTmpSize](GetBesselI0MaxMinTmpSize.md)。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>该接口的计算不需要临时空间，该参数可传入任意合法LocalTensor。临时空间大小BufferSize的获取方式请参考[GetBesselI0MaxMinTmpSize](GetBesselI0MaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

- 使用本接口前，需要包含头文件，头文件路径为：`adv_api/experimental/bessel_i0.h`。
- 使用前需要增加编译选项`-DCANN_ASC_USE_EXPERIMENTAL`开启试验特性。
- **不支持源操作数与目的操作数地址重叠。**
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

## 调用示例

```cpp
#include "adv_api/experimental/bessel_i0.h"
// dstLocal: 存放计算结果的Tensor
// srcLocal: 参与计算的输入Tensor
// 输入tensor长度为1024,算子输入的数据类型为float,实际计算个数为512
AscendC::experimental::BesselI0(dstLocal, srcLocal, 512);
```

结果示例如下：

```text
输入数据(srcLocal): [0.00            0.01            0.02             ...  5.10            5.11]
输出数据(dstLocal): [1.00000000e+00  1.00002500e+00  1.00010000e+00   ...  2.97888554e+01  3.00568915e+01]
```
