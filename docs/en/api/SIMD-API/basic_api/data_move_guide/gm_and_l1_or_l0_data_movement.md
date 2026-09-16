# GM and L1 or L0 Data Movement

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:07:37.442Z -->

## Overview<a name="ZH-CN_TOPIC_0000002543422572"></a>

Data movement between Global Memory and L1 Buffer/L0 Buffer provides flexible, layered API support that can fully adapt to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002565857683_table12546123753110)). Among them, data movement between Global Memory and L1 Buffer, Global Memory->L0A Buffer, and Global Memory->L0B Buffer is used for cube computation (refer to [Cube Computation Data Loading](../cube_compute_ISASI/cube_compute_load/matrix_computation_load.md)), and L0C Buffer->Global Memory is used to move cube computation results out, completing the output flow of cube computation (refer to [Cube Computation Data Storing](../cube_compute_ISASI/cube_compute_store/matrix_computation_storage.md)). By flexibly configuring structure parameters, developers can precisely control key attributes such as the number of data blocks, data block length, and address intervals, thereby implementing high-dimensional partitioning data movement, fully unleashing the hardware's data movement and computation collaboration capabilities, and significantly improving operator execution efficiency and overall performance.

**Table 1**  Overview of data movement APIs between Global Memory and L1 Buffer/L0 Buffer<a name="zh-cn_topic_0000002565857683_table12546123753110"></a>

| Data Path | Function | Description |
|----------|------|------|
| Global Memory and L1 Buffer | **DataCopy** (GM and L1 - continuous data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting continuous data movement. |
| Global Memory and L1 Buffer | **DataCopy** (GM and L1 - high-dimensional partitioning data movement) | Provides basic data movement capability. Data remains in its original format and content during transmission, supporting both continuous and non-continuous data movement. |
| Global Memory->L1 Buffer | **DataCopy** (GM -> L1 - inline conversion - [ND2NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#ZH-CN_TOPIC_0000002568950893) movement) | Supports conversion from [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section958745018719) to [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) format during data movement. |
| Global Memory->L1 Buffer | **DataCopy** (GM -> L1 - inline conversion - [DN2NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#ZH-CN_TOPIC_0000002568950893) movement) | Supports conversion from [DN](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section958745018719) to [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) format during data movement. |
| Global Memory->L1 Buffer | **DataCopyPad** (GM -> L1 - unaligned data movement) | Supports unaligned data movement and allows developers to fill data as needed. |
| Global Memory->L1 Buffer | **LoadData** (GM -> L1 - [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) data movement - 2D) | Responsible for completing the data movement of [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) matrices. |
| Global Memory->L1 Buffer | **LoadData** (GM -> L1 - [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) data movement - 2DV2) | Responsible for completing the data movement of [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section363412741113) matrices. |
| Global Memory->L0A Buffer | **LoadData** (GM -> L0A - 2D format fractal cube movement) | Responsible for completing the movement of 2D format data required for ordinary cube computation. |
| Global Memory->L0B Buffer | **LoadData** (GM -> L0B - 2D format fractal cube movement) | Responsible for completing the movement of 2D format data required for ordinary cube computation. |
| L0C Buffer->Global Memory | **DataCopy** (L0C -> GM - inline quantization activation movement) | Supports combinations of multiple inline capabilities. Different registers need to be set, and the **DataCopy** API is used to enable different data movement capabilities. |
| L0C Buffer->Global Memory | **Fixpipe** (L0C -> GM - inline quantization activation movement) | Supports combinations of multiple inline capabilities. The **FixPipe** API includes register setting and data movement capabilities. |

## DataCopy (GM and L1 - Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002574022813"></a>

DataCopy implements continuous data movement between Global Memory and L1 Buffer, with the format and content remaining unchanged during data movement.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopy (GM and L1 - Continuous Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements for continuous movement.

- Global Memory -> L1 Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- L1 Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## DataCopy (GM and L1 High-Dimensional Partitioning Data Movement)<a name="ZH-CN_TOPIC_0000002543262916"></a>

**DataCopy** implements continuous and non-continuous data movement between Global Memory and L1 Buffer. The format and content remain unchanged during data movement.

High-dimensional partitioning means that non-continuous movement can be achieved by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The specifically supported data paths are as follows (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopy (GM and L1 High-Dimensional Partitioning Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the movement parameter.

- Global Memory -> L1 Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- L1 Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const  LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## DataCopy (GM -> L1 - inline conversion - ND2NZ movement)<a name="ZH-CN_TOPIC_0000002573902841"></a>

This API mainly moves data from Global Memory to the L1 Buffer and supports converting the data from the [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format during data movement.

The specifically supported data path is (represented by the [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopy (GM -> L1 - inline conversion - ND2NZ movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the ND2NZ movement configuration parameter.

- The **enableSmallC0** mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- The **enableSmallC0** mode is supported (only Ascend 950PR/Ascend 950DT support it).

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

<!-- npu="950" id2 -->
## DataCopy (GM -> L1 - Inline Conversion - DN2NZ Movement)

This API mainly moves data from Global Memory to the L1 Buffer and supports converting the data from the [DN](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md) format during data movement.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopy (GM -> L1 - Inline Conversion - DN2NZ Movement)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_DN2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the DN2NZ movement configuration parameter.

- Supports the enableSmallC0 mode (supported only on Ascend 950PR/Ascend 950DT).

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
    ```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyPad (GM -> L1 Unaligned Data Movement)

This API mainly moves data from Global Memory to L1 Buffer and supports unaligned data movement during the transfer, allowing developers to fill data as needed.

The specifically supported data path is (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopyPad (GM -> L1 Unaligned Data Movement)](../cube_compute_ISASI/cube_compute_load/DataCopyPad_GMToL1.md).

**src** and **dst** are the source operand and destination operand respectively; **dataCopyParams** and **padParams** are movement parameters.

- Supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id3 -->

## LoadData (GM -> L1-2D Cube Movement)<a name="ZH-CN_TOPIC_0000002543422574"></a>

This API mainly moves data from Global Memory to the L1 Buffer. It performs the movement of 2D format data required for ordinary cube computation, moving data in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [LoadData (GM -> L1-2D Cube Movement)](../cube_compute_ISASI/cube_compute_load/gmtol1_load2d_instruction_movement.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id4 -->
## LoadData (GM -> L1-2D Cube Movement V2)

This API mainly moves data from Global Memory to the L1 Buffer. It is responsible for moving the 2D format data required for ordinary cube computation, performing the movement in units of data fractals of 512 bytes.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [LoadData (GM -> L1-2D Cube Movement V2)](../cube_compute_ISASI/cube_compute_load/gmtol1_load2dv2_instruction_movement.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id4 -->

## Load2D (GM -> L0A-2D Format Fractal Cube Movement)<a name="ZH-CN_TOPIC_0000002574022815"></a>

This API mainly moves data from Global Memory to the L0A Buffer. It is responsible for moving the 2D format data required for ordinary cube computation, performing the movement in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical position **TPosition**](../aux_data_structures/TPosition.md)):

- Global Memory -> L0A Buffer
    - GM -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [LoadData (GM -> L0A-2D Format Fractal Cube Movement)](../cube_compute_ISASI/cube_compute_load/Load2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## Load2D (GM -> L0B-2D Format Fractal Cube Movement)<a name="ZH-CN_TOPIC_0000002543262918"></a>

This API mainly moves data from Global Memory to the L0B Buffer, and is responsible for moving the 2D format data required for ordinary cube computation, performing movement in units of data fractals with a size of 512 bytes.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- Global Memory -> L0B Buffer
    - GM -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [LoadData (GM -> L0B-2D Format Fractal Cube Movement)](../cube_compute_ISASI/cube_compute_load/Load2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## DataCopy (L0C to GM - Inline Quantization Activation Movement)<a id="datacopy-l0c-gm-quantization-activation"></a>

This API mainly moves data from the L0C Buffer to Global Memory and supports combinations of multiple inline capabilities, which require different registers to be configured.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L0C Buffer -> Global Memory
    - CO1 -> GM

The moved data is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For API specifics, refer to [DataCopy (L0C -> GM - Inline Quantization Activation Movement)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter.

```cpp
template <typename T, typename U>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
```

## Fixpipe (L0C to GM - Inline Quantization Activation Movement)<a id="fixpipe-l0c-gm-quantization-activation"></a>

This API mainly moves data from the L0C Buffer to Global Memory and supports combinations of multiple inline capabilities. The API includes register configuration and data movement capabilities.

The specifically supported data paths are (represented by [logical position TPosition](../aux_data_structures/TPosition.md)):

- L0C Buffer -> Global Memory
    - CO1 -> GM

The moved data is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking Ascend 950PR/Ascend 950DT as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, refer to [Fixpipe (L0C -> GM - Inline Quantization Activation Movement)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter, and **cbufWorkspace** is the quantization parameter required when tensor quantization is enabled.

- When the inline [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) feature is not enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- When the inline [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) feature is enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
