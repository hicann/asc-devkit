# GetBesselI0MaxMinTmpSize

## 功能说明

本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

kernel侧BesselI0接口的计算不需要预留/申请临时空间，本接口用于在host侧获取需要预留/申请的最大和最小临时空间大小。对于BesselI0接口，获取到的最大和最小临时空间大小均为0，0表示计算不需要临时空间。

## 函数原型

```cpp
void GetBesselI0MaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## 参数说明

**表1**  接口参数列表

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| srcShape | 输入 | 输入的shape信息，参数类型为[AscendC::TensorShape](../../data_structures/TensorShape.md)。 |
| typeSize | 输入 | 输入的数据类型大小，单位为字节。比如输入的数据类型为float，此处应传入4。 |
| isReuseSource | 输入 | 是否允许修改源操作数，与BesselI0接口的isReuseSource参数保持一致。 |
| maxValue | 输出 | BesselI0接口能完成计算所需的最大临时空间大小，超出该值的空间不会被该接口使用。最大空间大小为0表示计算不需要临时空间。 |
| minValue | 输出 | BesselI0接口能完成计算所需的最小临时空间大小。最小空间大小为0表示计算不需要临时空间。 |

## 返回值说明

无

## 约束说明

- 使用本接口前，需要包含头文件，头文件路径为：`adv_api/experimental/bessel_i0_tiling.h`。
- 使用前需要增加编译选项`-DCANN_ASC_USE_EXPERIMENTAL`开启试验特性。

## 调用示例

```cpp
// 输入shape信息为1024;算子输入的数据类型为float;不允许修改源操作数
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::experimental::GetBesselI0MaxMinTmpSize(shape, 4, false, maxValue, minValue);
// maxValue = 0, minValue = 0，无需预留临时空间
```
