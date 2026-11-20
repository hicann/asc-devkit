# Data Movement Between L1 and BiasTable Buffer

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T05:08:47.008Z pushedAt=2026-11-18T01:25:06.878Z nl=2 -->

## DataCopy (Continuous Data Movement from L1 to BiasTable Buffer)<a name="ZH-CN_TOPIC_0000002574022837"></a>

DataCopy implements continuous data movement from the L1 Buffer to the BiasTable Buffer, with the format and content remaining unchanged during the movement.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> BiasTable Buffer
    - C1 -> C2

The moved data is used for matrix computation. For details about the API, see [DataCopy (L1ToBiasTable-Buffer Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (High-Dimensional Splitting Data Movement from L1 to BiasTable Buffer)<a name="ZH-CN_TOPIC_0000002543262936"></a>

**DataCopy** implements continuous and non-continuous data movement from the L1 Buffer to the BiasTable Buffer, with the format and content unchanged during data movement.

High-dimensional slicing means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> BiasTable Buffer
    - C1 -> C2

The moved data is used for matrix computation. For details about the API, see [DataCopy (L1ToBiasTable-Buffer Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the movement parameter.

- Continuous or non-continuous movement scenario (high-dimensional splitting)

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- Continuous or non-continuous movement scenario, supporting inconsistent data types between the source operand and the destination operand

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams)
    ```
