# UB与UB数据搬运<a name="ZH-CN_TOPIC_0000001499154365"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000002574022829"></a>

Unified Buffer（UB）之间的数据搬运提供了灵活的接口支持，共有两组接口：DataCopy提供了基础的数据搬运能力，支持连续和非连续数据搬运；Copy除了支持连续和非连续数据搬运，还提供了mask操作和DataBlock间隔操作的能力，支持在接口内部和接口外部设置mask。具体接口功能如下表所示。

**表 1**  UB与UB数据搬运接口概述

<a name="zh-cn_topic_0000002568294885_table12546123753110"></a>

| 数据通路 | 功能 | 描述 |
| :---: | :--- | :--- |
| UB->UB | 连续数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| UB->UB | 高维切分数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |
| UB->UB | 连续数据搬运(Copy) | 支持连续数据搬运，数据搬运时格式和内容保持不变。 |
| UB->UB | 掩码式高维数据搬运(Copy) | 支持mask操作和DataBlock间隔操作，支持在接口内部和接口外部设置mask。 |

## DataCopy（UB -> UB连续数据搬运）<a name="ZH-CN_TOPIC_0000002382907025"></a>

支持Unified Buffer与Unified Buffer之间的连续数据搬运，数据在传输过程中保持原始格式和内容不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（UB -> UB连续数据搬运）](../Memory矢量计算/数据搬运/DataCopy（UBToUB连续数据搬运）.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy（UB -> UB高维切分数据搬运）<a name="ZH-CN_TOPIC_0000002382908321"></a>

支持Unified Buffer与Unified Buffer之间的高维切分数据搬运，数据在传输过程中保持原始格式和内容不变。

高维切分数据搬运可通过配置数据块个数、单个数据块长度、地址偏移等搬运参数，同时支持非连续和连续的数据搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（UB -> UB高维切分数据搬运）](../Memory矢量计算/数据搬运/DataCopy（UBToUB高维切分数据搬运）.md)。

src和dst分别为源操作数和目的操作数；repeatParams为DataCopyParams类型的搬运参数，通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Copy（UB -> UB连续数据搬运）<a name="ZH-CN_TOPIC_0000002575088175"></a>

支持Unified Buffer和Unified Buffer之间的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[Copy（UB -> UB连续数据搬运）](../Memory矢量计算/数据搬运/Copy（UBToUB连续数据搬运）.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

```cpp
template <typename T, bool isSetMask = true>
__aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Copy（UB -> UB掩码式高维数据搬运）<a name="ZH-CN_TOPIC_0000002575088676"></a>

支持Unified Buffer和Unified Buffer之间的数据搬运，数据搬运时格式和内容保持不变，支持mask操作和DataBlock间隔操作。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[Copy（UB -> UB掩码式高维数据搬运）](../Memory矢量计算/数据搬运/Copy（UBToUB掩码式高维数据搬运）.md)。

src和dst分别为源操作数和目的操作数；mask用于控制每次迭代内参与计算的元素；repeatTime为重复迭代次数；repeatParams为CopyRepeatParams类型的搬运参数。

- mask逐bit模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

- mask连续模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```
