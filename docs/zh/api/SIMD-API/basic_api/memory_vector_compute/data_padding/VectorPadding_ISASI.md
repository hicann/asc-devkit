# VectorPadding\(ISASI\)<a name="ZH-CN_TOPIC_0000001870220489"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/VectorPadding_ISASI_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

根据padMode（pad模式）与padSide（pad方向）对源操作数按照DataBlock进行填充操作。

假设源操作数的一个DataBlock有16个数，DataBlock\[0:15\]=a\~p：

- padSide==false：从DataBlock的左边开始填充，即DataBlock的起始值方向\(a-\>p\)。
- padSide==true：从DataBlock的右边开始填充，即DataBlock的结束值方向\(p-\>a\)。
- padMode==0：用邻近数作为填充值，例如：aaa|abc\(padSide=false\)、nop|ppp\(padSide=true\)。
- padMode==1：用邻近DataBlock值对称填充，例如：cba|abc\(padSide=false\)、nop|pon\(padSide=true\)。
- padMode==2：用邻近DataBlock值填充，偏移一个数，做对称填充。例如：
  - padSide=false：xcb|abc，xcb被填充，填充过程为：a被丢弃，对称填充，x处填充0。
  - padSide=true：nop|onx，onx被填充，填充过程为：p被丢弃，对称填充，x处填充0。

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算

    ```cpp
    template <typename T>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint32_t count)
    ```

- tensor高维切分计算
  - mask逐bit模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - mask连续模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 操作数数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../mask_operations/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| ------ | ------ | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要按照32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要按照32字节对齐。源操作数的数据类型需要与目的操作数保持一致。 |
| padMode | 输入 | padding模式，类型为uint8_t，取值范围为[0,2]。<br>&bull; 0：用邻近数作为填充值。<br>&bull; 1：用邻近DataBlock值对称填充。<br>&bull; 2：用邻近DataBlock值填充，偏移一个数，做对称填充。 |
| padSide | 输入 | padding方向，类型为bool。<br>&bull; false：左边。<br>&bull; true：右边。 |
| count | 输入 | 参与计算的元素个数。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素，设置详见[掩码操作](../mask_operations/mask_operations.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元每次读取连续的256Bytes数据进行计算。 |
| repeatParams | 输入 | 控制操作数地址步长的参数，类型为[UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md)，包含相邻迭代间相同DataBlock及同一迭代内不同DataBlock的地址步长等参数。参数说明请参考[高维切分API](../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dim_split_api.md)。 |

<!-- npu="310p" id8 -->
## 数据类型

- 针对Atlas推理系列产品AI Core，T支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id8 -->

## 返回值说明<a name="section38228281712"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#alignment_requirements)。

-   mask仅控制目的操作数中的哪些元素要写入，源操作数的读取与mask无关。
-   count表示写入目的操作数中的元素总数，源操作数的读取与count无关。
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/VectorPadding_ISASI_res.md#id2 -->

## 调用示例<a name="section642mcpsimp"></a>

样例的srcLocal和dstLocal均为half类型。

更多样例可参考[LINK](../basic_arithmetic/more_examples.md)。

- tensor高维切分计算样例-mask连续模式

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor高维切分计算样例-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor前n个数据计算样例

    ```cpp
    uint8_t padMode = 0;
    bool padSide = false;
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, 512);
    ```

结果示例如下：

```text
// 以srcLocal的一个DataBlock的值为例，有16个数
输入数据(srcLocal): [6.938 -8.86 -0.2263 ... 1.971 1.778]
输出数据(dstLocal):
[6.938 6.938 6.938 ... 6.938 6.938]
```
