# Gather<a name="ZH-CN_TOPIC_0000002541491859"></a>

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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gather_res.md#id1 -->

## 功能说明<a name="section17600329101418"></a>

头文件路径为：`"basic_api/kernel_operator_vec_gather_intf.h"`。

Gather接口接受输入张量（src）、地址偏移张量（srcOffset）和基地址（srcBaseAddr），根据基地址和地址偏移量确定输入张量的索引，将输入张量中对应元素收集到结果张量（dst）中。

接口支持使用前n个数据计算和高维切分计算的方式，前n个数据计算原理和参考伪代码如下：

```python
import numpy as np

def Gather(dst, src, count, srcOffset, srcBaseAddr):
    inputType = np.dtype("uint16")
    for i in range(count):
        dst[i] = src[(srcBaseAddr + srcOffset[i]) // inputType.itemsize]
```

完整示例请参考：[Gather真值计算](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)。

## 函数原型<a name="section15660625202219"></a>

- tensor前n个数据计算。

  ```cpp
  template <typename T> 
  __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint32_t count) 
  ```

- tensor高维切分计算。
  - mask逐bit模式。

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src,const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

  - mask连续模式。

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

## 参数说明<a name="section1619484392111"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| :------- | :-------- | :--- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要按照32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要按照32字节对齐。<br><br>数据类型和dst保持一致。 |
| srcOffset | 输入 | 每个元素在src中对应的地址偏移，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>LocalTensor的起始地址需要32字节对齐。<br><br>该偏移量是相对于src的起始基地址srcBaseAddr而言的。单位为字节。取值要求见[约束说明](#约束说明)。 |
| srcBaseAddr | 输入 | 用于指定Gather操作中源操作数的起始位置，单位为字节。取值应保证src元素类型位宽对齐，否则会导致非预期行为。 |
| count | 输入 | 参与计算的元素个数。<br><br>参数取值范围和操作数的数据类型有关，数据类型不同，能够处理的元素个数最大值不同，最大处理的数据量不能超过UB大小限制。 |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。<br><br>设置详见[掩码操作](../mask_operations/mask_operations.md)。 |
| repeatTime | 输入 | 指令迭代次数。针对不同的型号，每个迭代处理的DataBlock可能存在差异，详见[约束说明](#约束说明)。|
| dstRepStride | 输入 | 相邻迭代间的地址步长，单位是DataBlock（32Bytes）。 |

## 数据类型
<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。数据类型int8\_t、uint8\_t仅支持tensor前n个数据计算接口。
<!-- end id8 -->

<!-- npu="A3" id9 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id10 -->

<!-- npu="310b" id11 -->
Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id11 -->

<!-- npu="310p" id12 -->
Atlas 推理系列产品AI Core，支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id12 -->

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[Unified Buffer地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- 不支持源操作数与目的操作数使用同一块内存地址。
- srcOffset的取值要求如下：
  - 取值应保证src元素类型位宽对齐。
  - 偏移地址后不能超出Unified Buffer（UB）大小数据的范围。
  <!-- npu="950,310b" id13 -->
  - 针对以下型号，地址偏移的取值范围：当操作数为8位时，取值范围为[0, 2^16-1]；当操作数为16位时，取值范围为[0, 2^17-1]，当操作数为32位或者64位时，不能超出uint32_t的范围。
    <!-- npu="950" id14 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id14 -->
    <!-- npu="310b" id15 -->
    - Atlas 200I/500 A2 推理产品
    <!-- end id15 -->
  <!-- end id13 -->

  <!-- npu="A3,910b,310p" id16 -->
  - 针对以下型号，地址偏移的取值范围：不能超出uint32_t的范围。
    <!-- npu="A3" id17 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id17 -->
    <!-- npu="910b" id18 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id18 -->
    <!-- npu="310p" id19 -->
    - Atlas 推理系列产品AI Core
    <!-- end id19 -->
  <!-- end id16 -->
- srcBaseAddr的取值应保证src元素类型位宽对齐，否则会导致非预期行为，程序没有崩溃报错，但数据发生错位，导致结果张量不符合预期。
- repeatTime取值范围：repeatTime∈[0,255]。

<!-- npu="950,A3,910b" id20 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id21 -->
  - 针对如下型号，当参数count或repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id22 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id23 -->
  <!-- end id21 -->

  <!-- npu="950" id24 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../reg_vector_compute/reg_vector_compute.md)API实现兼容，当参数count或repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
  <!-- end id24 -->
<!-- end id20 -->

<!-- npu="950,310b" id25 -->
- 针对以下型号，当操作数为**8位**时，每次迭代完成**4个DataBlock**的数据收集；其他情况下，每次迭代完成8个DataBlock的数据收集。
  <!-- npu="950" id26 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id26 -->
  <!-- npu="310b" id27 -->
  - Atlas 200I/500 A2 推理产品
  <!-- end id27 -->
<!-- end id25 -->

<!-- npu="A3,910b,310p" id28 -->
- 针对以下型号，每次迭代完成8个DataBlock的数据收集。
  <!-- npu="A3" id29 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id29 -->
  <!-- npu="910b" id30 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id30 -->
  <!-- npu="310p" id31 -->
  - Atlas 推理系列产品AI Core
  <!-- end id31 -->
<!-- end id28 -->


## 调用示例<a name="section11276201527"></a>

本样中只展示部分关键代码。如果您需要运行样例代码，请将该代码段拷贝并替换[Gather类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)场景三中Compute函数的部分代码即可。

- tensor高维切分计算样例-mask连续模式。

  ```cpp
  // repeatTime = 4, mask = 128, 128 elements one repeat, 512 elements total
  // srcLocal数据类型为half，srcOffsetLocal数据类型为uint32_t，dstLocal数据类型为half
  // srcBaseAddr = 0, srcLocal的起始基地址为0
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, 128, 4, 8);
  ```

- tensor高维切分计算样例-mask逐bit模式。

  ```cpp
  uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
  // repeatTime = 4, 128 elements one repeat, 512 elements total
  // srcLocal数据类型为half，srcOffsetLocal数据类型为uint32_t，dstLocal数据类型为half
  // srcBaseAddr = 0, srcLocal的起始基地址为0
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, mask, 4, 8);
  ```

- tensor前n个数据计算样例。

  ```cpp
  uint32_t count = 512;    // 参与计算的元素个数
  // srcLocal数据类型为half，srcOffsetLocal数据类型为uint32_t，dstLocal数据类型为half
  // srcBaseAddr = 0, srcLocal的起始基地址为0
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, count);
  ```

结果示例如下：

```plain
输入数据srcOffsetLocal:
[254 252 250 ... 4 2 0]
输入数据srcLocal（128个half类型数据）: 
[0 1 2 ... 125 126 127]
输出数据(dstLocal)初始值:
[0. 0. 0. 0. 0. 0. ... 0.]
进行Gather计算后，输出数据(dstLocal):
[127 126 125 ... 2 1 0]
```
