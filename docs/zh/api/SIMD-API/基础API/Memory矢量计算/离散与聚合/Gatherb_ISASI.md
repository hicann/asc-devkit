# Gatherb\(ISASI\)<a name="ZH-CN_TOPIC_0000001493030392"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
<!-- end id8 -->

## 功能说明<a name="section17600329101418"></a>

头文件路径为：`"basic_api/kernel_operator_vec_gather_intf.h"`。

Gatherb接口接受输入张量（src）、DataBlock偏移张量（offset），根据索引位置将输入张量按DataBlock（32字节）收集到结果张量（dst）中。

接口计算原理和参考伪代码如下：

```python
import numpy as np

def Gatherb(dst, src, offset, repeatTime, repeatParams):
    inputType = np.dtype("uint16")
    outputType = np.dtype("uint16")
    oneDataBlockSize = 32;
    if repeatParams.dstBlkStride == 0:
        repeatParams.dstBlkStride = 1
    for i in range(repeatTime): 
        for j in range(8):
            srcBlockStartIndex = offset[i * 8 + j] // inputType.itemsize
            srcBlockEndIndex = srcBlockStartIndex + oneDataBlockSize // inputType.itemsize
            dstBlockStartIndex = (i * oneDataBlockSize * repeatParams.dstRepStride + j * oneDataBlockSize * repeatParams.dstBlkStride) // outputType.itemsize
            dstBlockEndIndex = (i * oneDataBlockSize * repeatParams.dstRepStride + j * oneDataBlockSize * repeatParams.dstBlkStride + oneDataBlockSize) // outputType.itemsize
            dst[dstBlockStartIndex : dstBlockEndIndex] = src[srcBlockStartIndex : srcBlockEndIndex]
```

完整示例请参考：[Gatherb真值计算](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)。

## 函数原型<a name="section15660625202219"></a>

```cpp
template <typename T>
__aicore__ inline void Gatherb(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& offset, const uint8_t repeatTime, const GatherRepeatParams& repeatParams)
```

## 参数说明<a name="section1619484392111"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| :------- | :-------- | :--- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，存储位置为UB（TPosition为VECIN/VECCALC/VECOUT）。<br><br>LocalTensor的起始地址需要按照32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为UB（TPosition为VECIN/VECCALC/VECOUT）。<br><br>LocalTensor的起始地址需要按照32字节对齐。 |
| offset | 输入 | 每个DataBlock在源操作数中对应的地址偏移，类型为LocalTensor，存储位置为UB（TPosition为VECIN/VECCALC/VECOUT）。<br><br>LocalTensor的起始地址需要按照32字节对齐。<br><br>该偏移量是相对于src的基地址而言的。每个元素值要大于等于0，单位为字节。取值要求见[约束说明](#约束说明)。 |
| repeatTime | 输入 | 指令迭代次数，每次迭代完成8个DataBlock的数据收集，取值范围：repeatTime∈[0,255]。|
| repeatParams | 输入 | 用于控制指令迭代的相关参数。<br><br>GatherRepeatParams参数说明请参考[表3](#table3)。 |

**表3**  GatherRepeatParams结构体内参数说明<a id="table3"></a>

| 参数名称 | 含义 |
| :------- | :--- |
| dstBlkStride | 单次迭代内，矢量目的操作数不同DataBlock间地址步长，单位为DataBlock。<br><br>**注**：当dstBlkStride值为0时，默认按照1来处理。 |
| dstRepStride | 相邻迭代间，矢量目的操作数相同DataBlock地址步长，单位为DataBlock。 |
| blockNumber | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| src0BlkStride | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| src1BlkStride | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| src0RepStride | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| src1RepStride | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| repeatStrideMode | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |
| strideSizeMode | **注**：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。 |

## 数据类型

<!-- npu="950" id9 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id9 -->

<!-- npu="A3" id10 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：uint16_t、uint32_t。
<!-- end id10 -->

<!-- npu="910b" id11 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：uint16_t、uint32_t。
<!-- end id11 -->

<!-- npu="310b" id12 -->
Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t。
<!-- end id12 -->

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[Unified Buffer地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 不支持源操作数与目的操作数使用同一块内存地址。
- offset的取值要求如下：
  - 取值应保证src元素类型位宽对齐。
  - 偏移地址后需要32字节对齐。
  - 偏移地址后不能超出UB大小数据的范围。
  - 地址偏移的取值范围：不能超出uint32_t的范围。
- repeatTime取值范围：repeatTime∈[0,255]。

<!-- npu="950,A3,910b" id13 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id14 -->
  - 针对如下型号，当参数repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id15 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
  <!-- end id17 -->
<!-- end id13 -->

<!-- npu="A3,910b" id18 -->
- 针对如下型号，当dstBlkStride取值为0时，默认按照1来处理。
  <!-- npu="A3" id19 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id19 -->
  <!-- npu="910b" id20 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id20 -->
<!-- end id18 -->
## 调用示例<a name="section11276201527"></a>

完整使用样例请参见[Gather类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)场景四。

```cpp
uint32_t bufferLen = 128;
AscendC::GatherRepeatParams params{1, 8};
uint8_t repeatTime = bufferLen * sizeof(uint16_t) / 256;
AscendC::Gatherb<uint16_t>(y_buf, x_buf, offset_buf, repeatTime, params); // repeatTime重复迭代次数，每次迭代完成8个datablock的数据收集
```

结果示例：

```plain
输入数据(offsetLocal): [224 192 160 128 96 64 32 0]
输入数据(srcLocal): [0 1 2 3 4 5 6 7 ... 120 121 122 123 124 125 126 127]
输出数据(dstGlobal):[
112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 
96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111
... 
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
]
```
