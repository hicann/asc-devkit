# GetBesselI0TmpBufferFactorSize

## 功能说明

本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

该接口用于获取maxLiveNodeCount和extraBuf，在固定空间大小的情况下，通过maxLiveNodeCount和extraBuf可以推算算子单次最大计算元素数量。maxLiveNodeCount表示临时空间是单次计算数据量所占空间的多少倍；extraBuf表示使用的额外临时空间大小。

对于BesselI0接口，获取到的maxLiveNodeCount和extraBuf均为0，表示计算不需要临时空间。接口获取的maxLiveNodeCount值可能为0，推算单次最大计算元素数量时需要判断该值非0，避免除零错误。

## 函数原型

```cpp
void GetBesselI0TmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
```

## 参数说明

**表1**  接口参数列表

| 参数名 | 输入/输出 | 功能 |
| --- | --- | --- |
| typeSize | 输入 | 输入的数据类型大小，单位为字节。比如输入的数据类型为float，此处应传入4。 |
| maxLiveNodeCount | 输出 | 最大存活节点数，表示临时空间是单次计算数据量所占空间的多少倍。 |
| extraBuffer | 输出 | 使用的额外临时空间大小，单位为字节。 |

## 返回值说明

无

## 约束说明

- 使用本接口前，需要包含头文件，头文件路径为：`adv_api/experimental/bessel_i0_tiling.h`。
- 使用前需要增加编译选项`-DCANN_ASC_USE_EXPERIMENTAL`开启试验特性。

## 调用示例

```cpp
uint32_t maxLiveNodeCount = 0;
uint32_t extraBuf = 0;
AscendC::experimental::GetBesselI0TmpBufferFactorSize(typeSize, maxLiveNodeCount, extraBuf);
// maxLiveNodeCount = 0, extraBuf = 0，无需预留临时空间
```
