# L1 or L0C and UB Data Movement

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:05:59.001Z pushedAt=2026-10-26T07:41:44.267Z nl=2 -->

## Overview<a name="ZH-CN_TOPIC_0000002574022823"></a>

Data movement between L1 Buffer/L0C Buffer and Unified Buffer (UB) provides flexible API support that fully adapts to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002534897870_table1417935217588)). Specifically, UB->L1 Buffer data movement is used for matrix computation (see [matrix computation load-in](../cube_compute_ISASI/cube_compute_load/cube_compute_load.md)), and L0C Buffer->UB and L1 Buffer->UB are used to move matrix computation results out, completing the output flow of matrix computation (see [matrix computation store-out](../cube_compute_ISASI/cube_compute_store/cube_compute_store.md)).

**Table 1**  Overview of data movement APIs between L1 Buffer/L0C Buffer and UB<a name="zh-cn_topic_0000002534897870_table1417935217588"></a>

| Data Path | Function | Description |
|----------|------|------|
| UB->L1 Buffer | continuous data movement (**DataCopy**) | Provides basic data movement capability. Data remains unchanged in its original format and content during transmission, supporting continuous data movement. |
| UB->L1 Buffer | high-dimensional sliced data movement (**DataCopy**) | Provides basic data movement capability. Data remains unchanged in its original format and content during transmission, supporting continuous and non-continuous data movement. |
| UB->L1 Buffer | in-path conversion - [ND2NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#ZH-CN_TOPIC_0000002568950893>) movement (**DataCopy**) | Supports conversion from [ND](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section958745018719>) to [NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113>) format during data movement. |
| UB->L1 Buffer | unaligned data movement (**DataCopyPad**) | Supports unaligned data movement and allows developers to fill data as needed. |
| L0C Buffer->UB | in-path conversion and quantization (**DataCopy**) | Supports combinations of multiple in-path capabilities. The **DataCopy** API includes register configuration and data movement capabilities. |
| L0C Buffer->UB | in-path quantization activation movement (**FixPipe**) | Supports combinations of multiple in-path capabilities. The **FixPipe** API includes register configuration and data movement capabilities. |
| L1 Buffer->UB | continuous data movement (**DataCopyL1ToUB**) | Provides basic data movement capability. Data remains unchanged in its original format and content during transmission, supporting continuous data movement. |
| L1 Buffer->UB | high-dimensional sliced data movement (**DataCopyL1ToUB**) | Provides basic data movement capability. Data remains unchanged in its original format and content during transmission, supporting continuous and non-continuous data movement. |

## DataCopy (UBToL1 Continuous Data Movement)

**DataCopy** implements continuous data movement from UB to the L1 Buffer, with the format and content remaining unchanged during data movement.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> L1 Buffer
    - UB -> C1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (UBToL1 Continuous Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements for continuous movement.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (UBToL1 High-Dimensional Sliced Data Movement)

DataCopy implements continuous data movement and non-continuous data movement from UB to L1 Buffer, with the format and content unchanged during data movement.

High-dimensional slicing means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> L1 Buffer
    - UB -> C1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (UBToL1 High-Dimensional Sliced Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## DataCopy (UBToL1 In-Path Conversion - ND2NZ Data Movement)

This API mainly implements data movement from UB to L1 Buffer and supports conversion from the [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format during data movement.

The specific data paths supported are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> L1 Buffer
    - UB -> C1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For detailed API description, see [DataCopy (UBToL1 In-Path Conversion - ND2NZ Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the ND2NZ movement configuration parameter.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
```

## DataCopyPad (Unaligned Data Movement from UB to L1)

This API mainly implements data movement from UB to L1 Buffer, and supports unaligned data movement during data transfer, allowing developers to fill data as needed.

The specific supported data paths are (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- UB -> L1 Buffer
    - UB -> C1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyPad (Unaligned Data Movement from UB to L1)](../cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** and **nd2nzParams** are movement parameters.

```cpp
template <typename T>
__aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const Nd2NzParams& nd2nzParams)
```

<!-- npu="310p" id1 -->
## DataCopy (L0CToUB In-Path Quantization Activation Movement)

This API mainly moves data from the L0C Buffer to the UB and supports the combination of multiple in-path capabilities. The API includes register configuration and data movement capabilities.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> UB
    - CO1 -> UB

The moved data is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (L0C to UB data movement)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB.md).

This API is supported only on the AI Core of Atlas inference products.

**src** and **dst** are the source operand and destination operand, respectively. **intriParams** and **enhancedParams** are movement parameters.

- The source operand and destination operand have the same type.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

- The source operand and destination operand have different types.

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```
<!-- end id1 -->

## Fixpipe (L0CToUB In-Path Quantization Activation Movement)

This API mainly implements data movement from the L0C Buffer to the UB and supports combinations of multiple in-path capabilities. The API includes register setting and data movement capabilities.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> UB
    - CO1 -> UB

The data to be moved is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking the Ascend 950PR&950DT products as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, see [Fixpipe (L0C to UB Data Movement)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md).

src and dst are the source operand and destination operand, respectively; intriParams is the movement parameter, and cbufWorkspace is the quantization parameter required when tensor quantization is enabled.

- When the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) function is not enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- When the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) function is enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

<!-- npu="950" id2 -->
## DataCopyL1ToUB (L1ToUB Continuous Data Movement)

DataCopyL1ToUB implements continuous data movement from the L1 Buffer to the UB, with the format and content remaining unchanged during data movement.

The supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> UB
    - C1 -> UB

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyL1ToUB (L1 to UB Data Movement)](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md).

This API is supported only on Ascend 950PR&950DT products.

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyL1ToUB (L1ToUB high-dimensional sliced data movement)

**DataCopyL1ToUB** implements continuous data movement and non-continuous data movement from the L1 Buffer to the UB. The format and content remain unchanged during data movement.

High-dimensional slicing means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The specific supported data paths are as follows (represented by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> UB
    - C1 -> UB

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyL1ToUB (L1-to-UB data movement)](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md).

This API is supported only by Ascend 950PR&950DT products.

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the movement parameter.

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```
<!-- end id3 -->
