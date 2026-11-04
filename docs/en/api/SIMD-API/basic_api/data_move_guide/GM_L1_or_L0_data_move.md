# GM and L1 or L0 Data Move

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:00:49.547Z pushedAt=2026-10-26T07:41:44.262Z nl=2 -->

## Overview<a name="ZH-CN_TOPIC_0000002543422572"></a>

Data movement between Global Memory and L1 Buffer/L0 Buffer provides flexible, layered API support that can fully adapt to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002565857683_table12546123753110)). Specifically, data move between Global Memory and L1 Buffer, Global Memory->L0A Buffer, and Global Memory->L0B Buffer is used for matrix computation (see [Loading for Matrix Computation](../cube_compute_ISASI/cube_compute_load/cube_compute_load.md)), and L0C Buffer->Global Memory is used to move out matrix computation results to complete the output flow of matrix computation (see [Storing for Matrix Computation](../cube_compute_ISASI/cube_compute_store/cube_compute_store.md)). By flexibly configuring structure parameters, developers can precisely control key attributes such as the number of data blocks, data block length, and address intervals, implementing high-dimensional split data move, fully unleashing the hardware's data movement and computation collaboration capability, and significantly improving operator execution efficiency and overall performance.

**Table 1**  Overview of data move APIs between Global Memory and L1 Buffer/L0 Buffer<a name="zh-cn_topic_0000002565857683_table12546123753110"></a>

| Data Path | Function | Description |
|----------|------|------|
| Global Memory and L1 Buffer | Continuous data movement (DataCopy) | Provides basic data movement capability. Data keeps its original format and content unchanged during transmission, supporting continuous data movement. |
| Global Memory and L1 Buffer | High-dimensional splitting data movement (DataCopy) | Provides basic data movement capability. Data keeps its original format and content unchanged during transmission, supporting continuous and non-continuous data movement. |
| Global Memory->L1 Buffer | Inline conversion - [ND2NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#ZH-CN_TOPIC_0000002568950893) movement (DataCopy) | Supports conversion from [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section958745018719) to [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) format during data movement. |
| Global Memory->L1 Buffer | Inline conversion - [DN2NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#ZH-CN_TOPIC_0000002568950893) movement (DataCopy) | Supports conversion from [DN](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section958745018719) to [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) format during data movement. |
| Global Memory->L1 Buffer | Unaligned data move (DataCopyPad) | Supports unaligned data movement, allowing developers to fill data as needed. |
| Global Memory->L1 Buffer | [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) data movement (LoadData (2D matrix movement)) | Handles data movement of [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) matrices. |
| Global Memory->L1 Buffer | [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) data movement (LoadData (2D matrix movement V2)) | Handles data movement of [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113) matrices. |
| Global Memory->L0A Buffer | 2D format fractal matrix move (LoadData (2D matrix movement)) | Handles the movement of 2D format data required for ordinary matrix computation. |
| Global Memory->L0B Buffer | 2D format fractal matrix move (LoadData (2D matrix movement)) | Handles the movement of 2D format data required for ordinary matrix computation. |
| L0C Buffer->Global Memory | Inline quantization activation movement (DataCopy) | Supports combinations of multiple inline capabilities, requiring different registers to be set to enable different data move capabilities in conjunction with the DataCopy API. |
| L0C Buffer->Global Memory | Inline quantization activation movement (FixPipe) | Supports combinations of multiple inline capabilities. The FixPipe API includes register setting and data movement capabilities. |

## DataCopy (Continuous Data Move Between GM and L1)<a name="ZH-CN_TOPIC_0000002574022813"></a>

**DataCopy** implements continuous data move between Global Memory and L1 Buffer. The format and content remain unchanged during the data move.

The supported data paths are as follows (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the APIs, see [DataCopy (Continuous Data Move from GM to L1)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md) and [DataCopy (Continuous Data Move from L1 to GM)](../cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_continuous.md).

**src** and **dst** are the source operand and destination operand, respectively; **count** is the number of elements to be moved continuously.

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

## **DataCopy** (high-dimensional split data move between GM and L1)<a name="ZH-CN_TOPIC_0000002543262916"></a>

**DataCopy** implements continuous and non-contiguous data movement between Global Memory and L1 Buffer, while keeping the format and content unchanged.

High-dimensional split means that non-contiguous movement can be implemented by configuring movement parameters such as the number of data blocks, the length of a single data block, and the address offset.

The supported data paths are as follows (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the APIs, see [DataCopy (GMToL1 high-dimensional split data move)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md) and [DataCopy (L1ToGM high-dimensional split data move)](../cube_compute_ISASI/cube_compute_store/DataCopy_L1ToGM_highdim_split.md).

**src** and **dst** are the source operand and destination operand, respectively; **repeatParams** is the move parameter.

- Global Memory -> L1 Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- L1 Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## DataCopy (GMToL1 Inline Conversion - ND2NZ Move)<a name="ZH-CN_TOPIC_0000002573902841"></a>

This API mainly moves data from Global Memory to the L1 Buffer and supports conversion from the [ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format during the data move.

The specific supported data paths are as follows (indicated by the [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (GMToL1 Inline Conversion - ND2NZ Move)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the ND2NZ move configuration parameter.

- The enableSmallC0 mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- The enableSmallC0 mode is supported (supported only by Ascend 950PR&950DT products).

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

<!-- npu="950" id2 -->
## DataCopy (GMToL1 Inline Conversion-DN2NZ Move)<a name="ZH-CN_TOPIC_0000002573902899"></a>

This API mainly moves data from Global Memory to L1 Buffer and supports conversion from the [DN](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format to the [NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md) format during the data move.

The specific supported data paths are (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (GMToL1 Inline Conversion-DN2NZ Move)](../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_DN2NZ.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the DN2NZ move configuration parameter.

- Supports the enableSmallC0 mode (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
    ```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyPad (GM-to-L1 unaligned data move)<a name="ZH-CN_TOPIC_0000002543262891"></a>

This API mainly moves data from Global Memory to the L1 Buffer and supports unaligned data movement during the transfer, allowing developers to fill data as needed.

The supported data paths are as follows (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopyPad (GM-to-L1 unaligned data move)](../cube_compute_ISASI/cube_compute_load/DataCopyPad_GMToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **dataCopyParams** and **padParams** are copy parameters.

- Supported only on Ascend 950PR&950DT products

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id3 -->

## GMToL1 NZ Data Move (LoadData (2D Matrix Move))<a name="ZH-CN_TOPIC_0000002543422574"></a>

This API mainly moves data from Global Memory to the L1 Buffer. It is responsible for moving 2D-format data required for ordinary matrix computation, moving data in units of 512-byte data fractals.

The specific supported data paths are as follows (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (GMToL1-2D matrix move)](../cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the move parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id4 -->
## GMToL1 NZ Data Move (LoadData (2D Matrix Move V2))

This API mainly moves data from Global Memory to the L1 Buffer. It is responsible for moving 2D-format data required for ordinary matrix computation, with a data fractal of 512 bytes as the move unit.

The supported data paths are as follows (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L1 Buffer
    - GM -> A1/B1

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (GMToL1-2D Matrix Move V2)](../cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the move parameter.

- Supported only on Ascend 950PR&950DT products

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id4 -->

## GMToL0A 2D Format Fractal Matrix Move (LoadData (2D Matrix Move))<a name="ZH-CN_TOPIC_0000002574022815"></a>

This API mainly moves data from Global Memory to the L0A Buffer. It is responsible for moving the 2D format data required for ordinary matrix computation in units of 512-byte data fractals.

The specific supported data paths are (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L0A Buffer
    - GM -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D matrix move)](../cube_compute_ISASI/cube_compute_load/LoadData_2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the move parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## GMToL0B 2D format fractal matrix transfer (LoadData (2D matrix transfer))<a name="ZH-CN_TOPIC_0000002543262918"></a>

This API mainly moves data from Global Memory to the L0B Buffer, and is responsible for moving the 2D format data required for ordinary matrix computation, in units of 512-byte data fractals.

The specifically supported data paths are (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- Global Memory -> L0B Buffer
    - GM -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D matrix move)](../cube_compute_ISASI/cube_compute_load/LoadData_2D.md).

src and dst are the source operand and destination operand, respectively; loadDataParams is the move parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## DataCopy (L0CToGM Inline Quantization Activation Move)<a name="ZH-CN_TOPIC_0000002573902843"></a>

This API mainly moves data from the L0C Buffer to Global Memory and supports combinations of multiple inline capabilities, which require different registers to be set.

The specific supported data paths are (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> Global Memory
    - CO1 -> GM

The moved data is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (L0C to GM Data Move)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the move parameter.

```cpp
template <typename T, typename U>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
```

## Fixpipe (L0CToGM Inline Quantization Activation Move)<a name="ZH-CN_TOPIC_0000002543422576"></a>

This API mainly moves data from the L0C Buffer to Global Memory and supports combinations of multiple inline capabilities. The API includes register setting and data move capabilities.

The specific supported data paths are (indicated by [logical position TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> Global Memory
    - CO1 -> GM

The moved data is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking the Ascend 950PR&950DT products as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, see [Fixpipe (L0C to GM Data Move)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the move parameter, and **cbufWorkspace** is the quantization parameter required when tensor quantization is enabled.

- When the inline [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) function is not enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- When the on-the-fly [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) function is enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
