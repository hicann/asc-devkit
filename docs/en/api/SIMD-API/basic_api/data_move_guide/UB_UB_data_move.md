# UB-to-UB Data Movement<a name="ZH-CN_TOPIC_0000001499154365"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T05:15:32.707Z pushedAt=2026-11-18T01:25:06.882Z nl=2 -->

## Overview<a name="ZH-CN_TOPIC_0000002574022829"></a>

Data movement between Unified Buffers (UBs) provides flexible API support through two groups of APIs: **DataCopy** provides basic data movement capabilities and supports continuous and non-continuous data movement; **Copy** supports continuous and non-continuous data movement and additionally provides mask operations and DataBlock interval operations, supporting mask configuration both inside and outside the API. The specific API functions are shown in the following table.

**Table 1**  Overview of UB-to-UB Data Movement APIs

<a name="zh-cn_topic_0000002568294885_table12546123753110"></a>

| Data Path | Function | Description |
| :---: | :--- | :--- |
| UB->UB | continuous data movement (**DataCopy**) | Provides basic data movement capabilities. Data retains its original format and content during transmission, supporting continuous data movement. |
| UB->UB | high-dimensional splitting data movement (**DataCopy**) | Provides basic data movement capabilities. Data retains its original format and content during transmission, supporting continuous and non-continuous data movement. |
| UB->UB | continuous data movement (**Copy**) | Supports continuous data movement, with the format and content remaining unchanged during data movement. |
| UB->UB | masked high-dimensional data movement (**Copy**) | Supports mask operations and DataBlock interval operations, and supports setting the mask inside and outside the API. |

## DataCopy (UB -> UB Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002382907025"></a>

Supports continuous data movement between UB and UB, with the original format and content of the data preserved during transmission.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (UB -> UB Continuous Data Movement)](../memory_vector_compute/data_move/DataCopy_UBToUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (UB -> UB High-Dimensional Splitting Data Movement)<a name="ZH-CN_TOPIC_0000002382908321"></a>

Supports high-dimensional splitting data movement between UB and UB, with the original format and content of the data preserved during transmission.

High-dimensional splitting data movement can be configured with movement parameters such as the number of data blocks, the length of a single data block, and the address offset, and supports both non-contiguous and contiguous data movement.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [DataCopy (UB -> UB High-Dimensional Splitting Data Movement)](../memory_vector_compute/data_move/DataCopy_UBToUB_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **repeatParams** is a movement parameter of the **DataCopyParams** type, through which information such as the size, number, and interval of the data blocks to be moved can be configured, and both non-continuous and continuous movement are supported.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Copy (UB -> UB Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002575088175"></a>

Supports continuous data movement between UB and UB, with the format and content of the data preserved during movement.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [Copy (UB -> UB continuous data movement)](../memory_vector_compute/data_move/Copy_UBToUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

```cpp
template <typename T, bool isSetMask = true>
__aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Copy (UB -> UB Masked High-Dimensional Data Movement)<a name="ZH-CN_TOPIC_0000002575088676"></a>

Supports data movement between UB and UB, with the format and content of the data preserved during movement. It supports mask operations and DataBlock interval operations.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

The moved data is used for [vector computation](../memory_vector_compute/memory_vector_compute.md). For the specific API, see [Copy (UB -> UB masked high-dimensional data movement)](../memory_vector_compute/data_move/Copy_UBToUB_mask_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **mask** controls the elements that participate in computation within each iteration. **repeatTime** is the repeat iteration count. **repeatParams** is the movement parameter of the **CopyRepeatParams** type.

- **mask** bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

- **mask** continuous mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```
