# UB-to-UB Data Transfer<a name="ZH-CN_TOPIC_0000001499154365"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:10:27.359Z -->

## Overview<a name="ZH-CN_TOPIC_0000002574022829"></a>

Data transfer between Unified Buffers (UBs) provides flexible API support through two groups of APIs: **DataCopy** provides basic data transfer capabilities, supporting continuous and non-continuous data transfer; **Copy** supports continuous and non-continuous data transfer and additionally provides mask operations and **DataBlock** interval operations, supporting mask configuration both inside and outside the API. The specific API functions are shown in the following table.

**Table 1**  Overview of UB-to-UB data transfer APIs

<a name="zh-cn_topic_0000002568294885_table12546123753110"></a>

| Data Path | Function | Description |
| :---: | :--- | :--- |
| UB->UB | DataCopy (UB -> UB - continuous data transfer) | Provides basic data transfer capabilities. Data during transmission preserves its original format and content, supporting continuous data transfer. |
| UB->UB | DataCopy (UB -> UB - high-dimensional splitting data transfer) | Provides basic data transfer capabilities. Data during transmission preserves its original format and content, supporting continuous and non-continuous data transfer. |
| UB->UB | Copy (UB -> UB - continuous data transfer) | Supports continuous data transfer, with format and content preservation during data transfer. |
| UB->UB | Copy (UB -> UB - masked high-dimensional data transfer) | Supports mask operations and **DataBlock** interval operations, supporting mask configuration both inside and outside the API. |

## DataCopy (UB -> UB - Continuous Data Transfer)<a name="ZH-CN_TOPIC_0000002382907025"></a>

Supports continuous data transfer between Unified Buffers, with the original format and content preserved during transmission.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

The transferred data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, refer to [DataCopy (UB -> UB - Continuous Data Transfer)](../memory_vector_compute/data_move/Copy_UBToUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be transferred continuously.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (UB -> UB - High-Dimensional Splitting Data Transfer)<a name="ZH-CN_TOPIC_0000002382908321"></a>

Supports high-dimensional splitting data transfer between Unified Buffers, with the data during transmission preserving its original format and content.

High-dimensional splitting data transfer can be configured with transfer parameters such as the number of data blocks, the length of a single data block, and address offsets, supporting both non-contiguous and contiguous data transfer.

The specific supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

The transferred data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, refer to [DataCopy (UB -> UB - High-Dimensional Splitting Data Transfer)](../memory_vector_compute/data_move/DataCopy_UBToUB_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **repeatParams** is a transfer parameter of the **DataCopyParams** type, through which the data block size, number of blocks, interval, and other information can be configured, supporting both non-contiguous and contiguous transfer.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Copy (UB -> UB Continuous Data Transfer)<a name="ZH-CN_TOPIC_0000002575088175"></a>

Supports continuous data transfer between Unified Buffers, with format and content preservation during data transfer.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

The transferred data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, refer to [Copy (UB -> UB continuous data transfer)](../memory_vector_compute/data_move/Copy_UBToUB_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements transferred continuously.

```cpp
template <typename T, bool isSetMask = true>
__aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Copy (UB -> UB - Masked High-Dimensional Data Transfer)<a name="ZH-CN_TOPIC_0000002575088676"></a>

Supports data transfer between Unified Buffers. The format and content remain unchanged during the transfer, and mask operations and DataBlock interval operations are supported.

The specifically supported data paths are as follows (represented by the [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

The transferred data is used for [vector computation](../memory_vector_compute/memory_vector_computation.md). For the specific API, refer to [Copy (UB -> UB - masked high-dimensional data transfer)](../memory_vector_compute/data_move/Copy_UBToUB_mask_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively. **mask** controls the elements involved in computation within each iteration. **repeatTime** is the number of repeated iterations. **repeatParams** is the transfer parameter of the **CopyRepeatParams** type.

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
