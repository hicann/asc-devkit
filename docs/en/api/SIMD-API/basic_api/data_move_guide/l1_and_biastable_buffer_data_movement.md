# Data Movement Between L1 and BiasTable Buffer

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T12:36:34.371Z -->

## DataCopy (L1 -> BiasTable-Buffer-Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002574022837"></a>

DataCopy implements continuous data movement from the L1 Buffer to the BiasTable Buffer, with the format and content remaining unchanged during the movement.

The supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> BiasTable Buffer
    - C1 -> C2

The moved data is used for cube computation. For details about the API, see [DataCopy (L1 -> BiasTable-Buffer-Continuous Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md).

src and dst are the source operand and destination operand, respectively; count is the number of elements to be moved continuously.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (L1 -> BiasTable-Buffer-High-Dimensional Splitting Data Movement)<a name="ZH-CN_TOPIC_0000002543262936"></a>

DataCopy implements continuous data movement and non-continuous data movement from the L1 Buffer to the BiasTable Buffer. The format and content remain unchanged during data movement.

High-dimensional splitting refers to the ability to implement non-continuous movement by configuring movement parameters such as the number of data blocks, the length of a single data block, and address offsets.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> BiasTable Buffer
    - C1 -> C2

The moved data is used for cube computation. For details about the API, see [DataCopy (L1 -> BiasTable-Buffer-High-Dimensional Splitting Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToBiasTable.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** specifies the movement parameters.

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
