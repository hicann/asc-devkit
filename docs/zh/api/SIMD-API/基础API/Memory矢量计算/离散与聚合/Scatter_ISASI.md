# Scatter\(ISASI\)<a name="ZH-CN_TOPIC_0000001862055393"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

<!-- npu="A3,910b" id8 -->
>[!NOTE]说明
>
>该API不支持Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品，如果需要在上述AI处理器实现数据离散功能，建议参考[Scatter兼容样例](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/scatter)进行适配。
<!-- end id8 -->

## 功能说明<a name="section17600329101418"></a>

头文件路径为：`"basic_api/kernel_operator_vec_scatter_intf.h"`。

给定一个连续的输入张量和一个目的地址偏移张量，Scatter指令根据偏移地址生成新的结果张量后将输入张量分散到结果张量中。

将源操作数src中的元素按照指定的位置（由dstOffset和dstBaseAddr共同作用）分散到目的操作数dst中。

## 函数原型<a name="section15660625202219"></a>

-   tensor前n个数据计算

    ```cpp
    template <typename T>
    __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint32_t count)
    ```

-   tensor高维切分计算
    -   mask逐bit模式

        ```cpp
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint8_t srcRepStride)
        ```

    -   mask连续模式

        ```cpp
        template <typename T>
        __aicore__ inline void Scatter(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset, const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint8_t srcRepStride)
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
| dstOffset | 输入 | 每个元素在dst中对应的地址偏移，类型为LocalTensor，存储位置为UB（TPosition为VECIN/VECCALC/VECOUT）。<br><br>LocalTensor的起始地址需要32字节对齐。<br><br>该偏移量是相对于dst的起始基地址dstBaseAddr而言的。单位为字节。取值要求见[约束说明](#约束说明)。|
| dstBaseAddr | 输入 | 用于指定dst的起始偏移地址，单位为字节。取值应保证dst元素类型位宽对齐，否则会导致非预期行为。 |
| count | 输入 | 参与计算的元素个数。<br><br>参数取值范围和操作数的数据类型有关，数据类型不同，能够处理的元素个数最大值不同，最大处理的数据量不能超过UB大小限制。 |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。<br><br>设置详见[掩码操作](../掩码操作/掩码操作.md)。 |
| repeatTime | 输入 | 指令迭代次数。针对不同的型号，每个迭代处理的DataBlock可能存在差异，详见[约束说明](#约束说明)。|
| srcRepStride | 输入 | 相邻迭代间的地址步长，单位是DataBlock（32Bytes）。 |

## 数据类型

<!-- npu="950" id9 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。int8\_t、uint8\_t数据类型仅支持tensor前n个数据计算接口。
<!-- end id9 -->

<!-- npu="310b" id10 -->
Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id10 -->

<!-- npu="310p" id11 -->
Atlas 推理系列产品AI Core，支持的数据类型为：uint16_t、half、uint32_t、float。
<!-- end id11 -->

## 约束说明<a name="section633mcpsimp"></a>

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
-   操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#section668772811100)。

- dstOffset的取值要求如下：
  - 偏移地址不能有相同值，如果存在2个或者多个偏移重复的情况，行为是不可预期的。 
  - 取值应保证dst元素类型位宽对齐。
  - 偏移地址后不能超出UB大小数据的范围。
  <!-- npu="950,310b" id12 -->
  - 针对以下型号，地址偏移的取值范围：当操作数为8位时，取值范围为[0, 2^16-1]；当操作数为16位时，取值范围为[0, 2^17-1]，当操作数为32位或者64位时，不能超出uint32_t的范围。
    <!-- npu="950" id13 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id13 -->
    <!-- npu="310b" id14 -->
    - Atlas 200I/500 A2 推理产品
    <!-- end id14 -->
  <!-- end id12 -->

  <!-- npu="310p" id15 -->
  - 针对以下型号，地址偏移的取值范围：不能超出uint32_t的范围。
    <!-- npu="310p" id16 -->
    - Atlas 推理系列产品AI Core
    <!-- end id16 -->
  <!-- end id15 -->
- dstBaseAddr的取值应保证dst元素类型位宽对齐，否则会导致非预期行为，程序没有崩溃报错，但数据发生错位，导致结果张量不符合预期。
- repeatTime取值范围：repeatTime∈[0,255]。
<!-- npu="950" id17 -->
- 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数count或repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
<!-- end id17 -->

<!-- npu="950,310b" id18 -->
- 针对以下型号，当操作数为**8位**时，每次迭代完成**4个DataBlock**的数据收集；其他情况下，每次迭代完成8个DataBlock的数据收集。
  <!-- npu="950" id19 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id19 -->
  <!-- npu="310b" id20 -->
  - Atlas 200I/500 A2 推理产品
  <!-- end id20 -->
<!-- end id18 -->

<!-- npu="310p" id21 -->
- 针对以下型号，每次迭代完成8个DataBlock的数据收集。
  <!-- npu="310p" id22 -->
  - Atlas 推理系列产品AI Core
  <!-- end id22 -->
<!-- end id21 -->

## 调用示例<a name="section11276201527"></a>

完整使用样例请参见[Scatter兼容性样例](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/scatter)。

```cpp
uint32_t COUNT = 128;
AscendC::Scatter(dstLocal, srcLocal, dstOffsetLocal, (uint32_t)0, COUNT); // dstOffsetLocal用于存储源操作数的每个元素在dst中对应的地址偏移
```

结果示例：

```plain
输入数据dstOffsetLocal:
[254 252 250 ... 4 2 0]
输入数据srcLocal（128个half类型数据）: 
[0 1 2 ... 125 126 127]
输出数据dstGlobal:
[127 126 125 ... 2 1 0]
```
