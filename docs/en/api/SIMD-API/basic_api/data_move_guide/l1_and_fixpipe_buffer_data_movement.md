# Data Movement Between L1 and Fixpipe Buffer

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T12:37:18.096Z -->

## DataCopy (L1 -> Fixpipe-Buffer - Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002543262934"></a>

**DataCopy** implements continuous data movement from the L1 Buffer to the Fixpipe Buffer, with the format and content remaining unchanged during the movement.

The supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> Fixpipe Buffer
    - C1 -> C2PIPE2GM

The data moved consists of [inline quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) and [inline ReLU](../cube_compute_ISASI/cube_store_key_features/on_the_fly_relu.md) parameters. For details about the API, see [DataCopy (L1 -> Fixpipe-Buffer - Continuous Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);
```

## DataCopy (L1 -> Fixpipe-Buffer High-Dimensional Partitioning Data Movement)<a name="ZH-CN_TOPIC_0000002573902863"></a>

**DataCopy** implements continuous and non-continuous data movement from the L1 Buffer to the Fixpipe Buffer, with the format and content unchanged during data movement.

High-dimensional partitioning refers to implementing non-continuous movement by configuring movement parameters such as the number of data blocks, the length of a single data block, and address offsets.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> Fixpipe Buffer
    - C1 -> C2PIPE2GM

The moved data consists of [inline quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) and [inline ReLU](../cube_compute_ISASI/cube_store_key_features/on_the_fly_relu.md) parameters. For details about the API, see [DataCopy (L1 -> Fixpipe-Buffer High-Dimensional Partitioning Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** specifies the movement parameters.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```
