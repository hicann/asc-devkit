# Data Movement Between L1 or L0C and UB

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:08:04.055Z -->

## Overview<a name="ZH-CN_TOPIC_0000002574022823"></a>

Data movement between L1 Buffer/L0C Buffer and Unified Buffer provides flexible API support that can fully adapt to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002534897870_table1417935217588)). Among them, data movement from Unified Buffer to L1 Buffer is used for cube computation (see [Cube Computation Data Loading](../cube_compute_ISASI/cube_compute_fractal_intro/Cube Computation Data Loading.md)), while data movement from L0C Buffer to Unified Buffer and from L1 Buffer to Unified Buffer is used to move cube computation results out, completing the output flow of cube computation (see [Cube Computation Data Moving Out](../cube_compute_ISASI/cube_compute_fractal_intro/Cube Computation Data Moving Out.md)).

**Table 1**  Overview of data movement APIs between L1 Buffer/L0C Buffer and Unified Buffer<a name="zh-cn_topic_0000002534897870_table1417935217588"></a>

| Data Path | Function | Description |
|----------|------|------|
| Unified Buffer->L1 Buffer | **DataCopy** (UB -> L1 - continuous data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting continuous data movement. |
| Unified Buffer->L1 Buffer | **DataCopy** (UB -> L1 - high-dimensional split data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting continuous and non-continuous data movement. |
| Unified Buffer->L1 Buffer | In-path conversion - [ND2NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#ZH-CN_TOPIC_0000002568950893>) movement | Supports conversion from [ND](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section958745018719>) to [NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113>) format during data movement. |
| Unified Buffer->L1 Buffer | **DataCopyPad** (UB -> L1 - unaligned data movement) | Supports unaligned data movement, allowing developers to fill data as needed. |
| L0C Buffer->Unified Buffer | **DataCopy** (L0C -> UB - in-path conversion and quantization) | Supports the combination of multiple in-path capabilities. The **DataCopy** API includes register setting and data movement capabilities. |
| L0C Buffer->Unified Buffer | **Fixpipe** (L0C -> UB - in-path quantization activation movement) | Supports the combination of multiple in-path capabilities. The **FixPipe** API includes register setting and data movement capabilities. |
| L1 Buffer->Unified Buffer | **DataCopyL1ToUB** (L1 -> UB - continuous data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting continuous data movement. |
| L1 Buffer->Unified Buffer | **DataCopyL1ToUB** (L1 -> UB - high-dimensional split data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting continuous and non-continuous data movement. |

## DataCopy (UB -> L1 - Continuous Data Movement)

**DataCopy** implements continuous data movement from Unified Buffer to L1 Buffer, with the format and content remaining unchanged during data movement.

The supported data path is as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> L1 Buffer
    - UB -> C1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (UB -> L1 - Continuous Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements for continuous movement.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy (UB -> L1 High-Dimensional Split Data Movement)

**DataCopy** implements continuous data movement and non-continuous data movement from Unified Buffer to L1 Buffer. The format and content remain unchanged during data movement.

High-dimensional split means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> L1 Buffer
    - UB -> C1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (UB -> L1 High-Dimensional Split Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## DataCopy (UB -> L1 - In-Path Conversion - ND2NZ Movement)

This API mainly moves data from the Unified Buffer to the L1 Buffer and supports conversion from the [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format during data movement.

The specifically supported data paths (represented by the [logical position TPosition](../aux_data_structures/TPosition.md)) are as follows:

- Unified Buffer -> L1 Buffer
    - UB -> C1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (UB -> L1 - In-Path Conversion - ND2NZ Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the ND2NZ movement configuration parameter.

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
```

## UBToL1DataCopyPad (UB -> L1 - Unaligned Data Movement)

This API mainly implements data movement from the Unified Buffer to the L1 Buffer, and supports unaligned data movement during data transfer, allowing developers to fill data as needed.

The specific supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Unified Buffer -> L1 Buffer
    - UB -> C1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyPad (UB -> L1 - Unaligned Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** and **nd2nzParams** are movement parameters.

```cpp
template <typename T>
__aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const Nd2NzParams& nd2nzParams)
```

<!-- npu="310p" id1 -->
## DataCopy (L0C -> UB - In-Path Quantization Activation Movement)

This API moves data from the L0C Buffer to the Unified Buffer and supports combinations of multiple in-path capabilities. The API includes register configuration and data movement capabilities.

The specific supported data paths (expressed as [logical position TPosition](../aux_data_structures/TPosition.md)) are as follows:

- L0C Buffer -> Unified Buffer
    - CO1 -> UB

The data moved is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (L0C -> UB - In-Path Quantization Activation Movement)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB.md).

This API is supported only on Atlas inference products AI Core.

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** and **enhancedParams** are the movement parameters.

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

## L0CToUBFixpipe (L0C -> UB - In-Path Quantization Activation Movement)

This API mainly moves data from the L0C Buffer to the Unified Buffer and supports the combination of multiple in-path capabilities. The API includes register configuration and data movement capabilities.

The specific supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L0C Buffer -> Unified Buffer
    - CO1 -> UB

The moved data is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking Ascend 950PR/Ascend 950DT as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, see [Fixpipe (L0C -> UB - In-Path Quantization Activation Movement)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter, and **cbufWorkspace** is the quantization parameter required when tensor quantization is enabled.

- When the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) function is not enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- When the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) function is enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

<!-- npu="950" id2 -->
## DataCopyL1ToUB (L1 -> UB - Continuous Data Movement)

**DataCopyL1ToUB** implements continuous data movement from the L1 Buffer to the Unified Buffer. The format and content remain unchanged during data movement.

The supported data paths are as follows (represented by the [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> Unified Buffer
    - C1 -> UB

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyL1ToUB (L1 -> UB - Continuous Data Movement)](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md).

This API is supported only on Ascend 950PR/Ascend 950DT.

**src** and **dst** are the source operand and destination operand, respectively. **count** is the number of elements to be moved continuously.

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyL1ToUB (L1 -> UB - High-Dimensional Split Data Movement)

**DataCopyL1ToUB** implements continuous data movement and non-continuous data movement from the L1 Buffer to the Unified Buffer. The format and content remain unchanged during data movement.

High-dimensional split means that non-continuous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> Unified Buffer
    - C1 -> UB

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyL1ToUB (L1 -> UB - High-Dimensional Split Data Movement)](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md).

This API is supported only on Ascend 950PR/Ascend 950DT.

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** specifies the movement parameters.

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```
<!-- end id3 -->
