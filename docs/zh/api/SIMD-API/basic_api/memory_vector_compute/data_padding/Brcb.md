# Brcb<a name="ZH-CN_TOPIC_0000002543424524"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/Brcb_res.md#id1 -->

## 功能说明<a name="section17600329101418"></a>

头文件路径为：`"basic_api/kernel_operator_vec_brcb_intf.h"`。

Brcb接口对输入数据，每一次取输入张量中的8个数进行填充操作，将每个数填充到结果张量的一个DataBlock（32字节）中。

接口计算原理和参考伪代码如下：

```python
import numpy as np

def Brcb(src, dst, repeatTime, repeatParams):
    outputType = np.dtype("uint16")
    oneDataBlockSize = 32
    if repeatParams.dstBlkStride == 0:
        repeatParams.dstBlkStride = 1

    for i in range(repeatTime):
        for j in range(8):
            srcEleIndex = i * 8 + j
            dstBlockStartIndex = (oneDataBlockSize * i * repeatParams.dstRepStride + oneDataBlockSize * j * repeatParams.dstBlkStride) // outputType.itemsize
            dstBlockEndIndex = (oneDataBlockSize * i * repeatParams.dstRepStride + oneDataBlockSize * j * repeatParams.dstBlkStride + oneDataBlockSize) // outputType.itemsize
            dst[dstBlockStartIndex : dstBlockEndIndex] = src[srcEleIndex]
```

## 函数原型<a name="section15660625202219"></a>

```cpp
template <typename T>
__aicore__ inline void Brcb(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams)
```

## 参数说明<a name="section1619484392111"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 操作数数据类型。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br> LocalTensor的起始地址需要按照32字节对齐。|
| src | 输入 | 源操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br> 需要按照32字节对齐。<br> 注：每一次迭代读取src中的8个元素，所以src的元素个数不小于8 * repeatTime。|
| repeatTime | 输入 | 指令迭代次数，每次迭代完成8个DataBlock的数据收集，取值范围：repeatTime∈[0,255]。|
| repeatParams | 输入 | 用于控制指令迭代的相关参数，类型为BrcbRepeatParms。<br> BrcbRepeatParams参数说明请参考[表3 BrcbRepeatParams结构体参数说明](#table3)。|

**表3**  BrcbRepeatParams结构体参数说明<a id="table3"></a>

| 参数名称 | 含义 |
| ------ | ------ |
| dstBlkStride | 单次迭代内，矢量目的操作数不同DataBlock间地址步长，单位为DataBlock。|
| dstRepStride | 相邻迭代间，矢量目的操作数相同DataBlock地址步长，单位为DataBlock。|
| blockNumber | 注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。|
| src1BlkStride | 注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。|
| src1RepStride | 注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。|
| repeatStrideMode | 注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。|
| strideSizeMode | 注：预留的扩展参数，当前因后续架构升级，该参数已废弃，不对其进行业务处理。|

## 数据类型

<!-- npu="950" id10 -->
- 针对Ascend 950PR/Ascend 950DT，T支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
<!-- end id10 -->
<!-- npu="A3" id11 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id11 -->
<!-- npu="910b" id12 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，T支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id12 -->
<!-- npu="310p" id13 -->
- 针对Atlas 推理系列产品AI Core，T支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id13 -->
<!-- npu="x90" id14 -->
- 针对Kirin X90，T支持的数据类型为：uint16_t、half、uint32_t、float。
<!-- end id14 -->
<!-- npu="9030" id15 -->
- 针对Kirin 9030，T支持的数据类型为：uint16_t、half、uint32_t、float。
<!-- end id15 -->

## 返回值说明<a name="section459672612511"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- 不支持源操作数与目的操作数使用同一块内存地址。
- BrcbRepeatParams结构体内参数dstBlkStride不支持设置为0，若设置成0可能产生未定义行为。
<!-- npu="310p" id16 -->
- 针对Atlas 推理系列产品AI Core，使用时需要预留8KB的Unified Buffer（UB）空间，作为接口的临时数据存放区。
<!-- end id16 -->
<!-- npu="A3,910b,950" id17 -->
- 当参数repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id18 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id19 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id19 -->
    <!-- npu="910b" id20 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id20 -->
  <!-- end id18 -->
  <!-- npu="950" id21 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../reg_vector_compute/reg_vector_compute.md)实现兼容，当参数repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id21 -->
<!-- end id17 -->
## 关键特性说明

源操作数不支持Stride参数配置，始终采用连续读。

### 目的操作数连续

利用dstBlkStride和dstRepStride参数进行连续写，从源操作数取8个half数据类型的元素填充到连续的8个DataBlock中。

dstBlkStride为1：说明同一迭代内目的操作数中相邻DataBlock地址连续。

dstRepStride为8：说明相邻迭代间目的操作数中首尾DataBlock地址连续。

**图1**  目的操作数连续

![目的操作数连续](../../../../figures/Brcb_1.png)

### 目的操作数非连续

利用dstBlkStride和dstRepStride参数进行跳写，从源操作数取8个half数据类型的元素填充到非连续的8个DataBlock中。

dstBlkStride为2：说明同一迭代内目的操作数中相邻DataBlock地址非连续（间隔1个DataBlock）。

dstRepStride为16：说明相邻迭代间目的操作数中首尾DataBlock地址非连续（间隔1个DataBlock）。

**图2**  目的操作数非连续

![目的操作数非连续](../../../../figures/Brcb_2.png)

## 调用示例<a name="section11276201527"></a>

本调用示例中只展示完整样例中的接口部分代码。如果您需要运行示例代码，请直接参考完整样例[Brcb样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/brcb)进行编译执行。

```cpp
// repeatTime = 2, 128 elements one repeat, 256 elements total
// srcLocal数据类型为half，dstLocal数据类型为half
// dstBlkStride = 1, no gap between blocks in one repeat
// dstRepStride = 8, no gap between repeats
AscendC::Brcb(dstLocal, srcLocal, 2, {1, 8});
```

结果示例如下：

```plain
输入数据(srcLocal):
[1 2 3 ... 16]
输出数据(dstLocal)初始值:
[0. 0. 0. 0. 0. 0. ... 0.]
进行Brcb计算后，输出数据(dstLocal):
[1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ... 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]
```
