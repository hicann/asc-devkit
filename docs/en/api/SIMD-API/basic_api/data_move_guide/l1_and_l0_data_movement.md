# L1 and L0 Data Movement

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:10:18.766Z -->

## Overview<a name="ZH-CN_TOPIC_0000002543262920"></a>

The data movement between the L1 Buffer and the L0 Buffer provides flexible API support and can fully adapt to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002535057808_table12546123753110)). Among them, the data movement from L1 Buffer to L0A Buffer and from L1 Buffer to L0B Buffer is used for cube computation (see [Cube Computation Input](../cube_compute_ISASI/cube_compute_load/matrix_computation_load.md)), and the data movement from L0C Buffer to L1 Buffer is used for cube computation output, completing the output process of cube computation (see [Cube Computation Output](../cube_compute_ISASI/cube_compute_store/matrix_computation_storage.md)).

**Table 1**  Overview of data movement APIs between the L1 Buffer and the L0 Buffer<a name="zh-cn_topic_0000002535057808_table12546123753110"></a>

| Data Path | Function | Description |
|----------|------|------|
| L1 Buffer->L0A Buffer | LoadData (L1 -> L0A-2D format fractal cube movement) | Responsible for moving the 2D format data required for ordinary cube computation. |
| L1 Buffer->L0A Buffer | LoadData (L1 -> L0A-2D format fractal cube movement V2) | Responsible for moving the 2D format data required for ordinary cube computation. |
| L1 Buffer->L0A Buffer | LoadData (L1 -> L0A-2D format fractal cube movement MX) | Responsible for moving the left and right cube data required for cube computation and the corresponding left and right quantization coefficient cube data. |
| L1 Buffer->L0A Buffer | LoadDataWithTranspose (L1 -> L0A-2D format fractal cube transpose movement) | Responsible for moving the 2D format data required for ordinary cube computation, with a transpose operation performed during the movement. |
| L1 Buffer->L0A Buffer | LoadData (L1 -> L0A-3D format fractal cube movement) | Used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select the specified data block from the expanded two-dimensional cube and move it to the corresponding memory location. |
| L1 Buffer->L0B Buffer | LoadData (L1 -> L0B-2D format fractal cube movement) | Responsible for moving the 2D format data required for ordinary cube computation. |
| L1 Buffer->L0B Buffer | LoadData (L1 -> L0B-2D format fractal cube movement V2) | Responsible for moving the 2D format data required for ordinary cube computation. |
| L1 Buffer->L0B Buffer | LoadData (L1 -> L0B-2D format fractal cube movement MX) | Responsible for moving the left and right cube data required for cube computation and the corresponding left and right quantization coefficient cube data. |
| L1 Buffer->L0B Buffer | LoadDataWithTranspose (L1 -> L0B-2D format fractal cube transpose movement) | Responsible for moving the 2D format data required for ordinary cube computation, with a transpose operation performed during the movement. |
| L1 Buffer->L0B Buffer | LoadData (L1 -> L0B-3D format fractal cube movement) | Used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select the specified data block from the expanded two-dimensional cube and move it to the corresponding memory location. |
| L1 Buffer->L0B Buffer | LoadDataWithSparse (L1 -> L0B-dense weight cube movement) | Used to move the dense weight cube stored in 512-byte units from the L1 Buffer to the L0B Buffer, while moving the index cube in 128-byte units to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API). |
| L0C Buffer->L1 Buffer | DataCopy (L0C -> L1-inline quantization activation movement) | Supports the combination of multiple inline capabilities, requiring different registers to be set to enable different data movement capabilities in conjunction with the DataCopy instruction. |
| L0C Buffer->L1 Buffer | Fixpipe (L0C -> L1-inline quantization activation movement) | Supports the combination of multiple inline capabilities. The FixPipe API includes register setting and data movement capabilities. |

## LoadData (L1 -> L0A-2D Format Fractal Cube Movement)<a name="ZH-CN_TOPIC_0000002573902845"></a>

**Load2D** implements data movement from the L1 Buffer to the L0A Buffer. It is responsible for moving 2D-format data required for ordinary cube computation, moving data in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D Format Fractal Cube Movement)](../cube_compute_ISASI/cube_compute_load/Load2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id1 -->
## LoadData (L1 -> L0A-2D Format Fractal Cube Movement V2)

**Load2DV2** implements data movement between the L1 Buffer and the L0A Buffer. It is responsible for moving 2D-format data required for ordinary cube computation, moving data in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D Format Fractal Cube Movement V2)](../cube_compute_ISASI/cube_compute_load/Load2DV2.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id1 -->

<!-- npu="950" id2 -->
## LoadData (L1 -> L0A-2D Format Fractal Cube Movement MX)

Load2DMX is responsible for the movement of the left and right cube data and the corresponding left and right quantization coefficient cube data required for cube computation. The left and right cube data is moved in units of 512-byte data fractals, and the left and right quantization coefficient matrices are moved in units of 32-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0A-2D Format Fractal Cube Movement MX)](../cube_compute_ISASI/cube_compute_load/Load2DMX.md).

src and srcMx are source operands, and dst is the destination operand; loadDataParams and loadMxDataParams are movement parameters.

- Load2DMX API (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T, typename U = T>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

- Load2Dv2MX API, which supports inconsistent data types between the source operand and the destination operand (supported only on Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```
<!-- end id2 -->

## LoadDataWithTranspose (L1 -> L0A-2D format fractal cube transpose movement)<a name="ZH-CN_TOPIC_0000002543422578"></a>

**LoadDataWithTranspose** implements data movement from the L1 Buffer to the L0A Buffer. It is responsible for the 2D-format data movement required by ordinary cube computation. A transpose operation is performed during the movement, which is carried out in units of 512-byte data fractals.

The specifically supported data paths are as follows (represented by [logical location **TPosition**](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithTranspose (L1 -> L0A-2D format fractal cube transpose movement)](../cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id10 -->

Only Ascend 950PR/Ascend 950DT: Supported

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id10 -->

## LoadData (L1 -> L0A-3D format fractal cube movement)<a name="ZH-CN_TOPIC_0000002574022819"></a>

Load3D is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional cube and move it to the corresponding memory location.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0A-3D format fractal cube movement)](../cube_compute_ISASI/cube_compute_load/Load3D.md).

src and dst are the source operand and destination operand, respectively; loadDataParams is the movement parameter.

- Load3Dv1 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
    ```

- Load3Dv2 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```

<!-- npu="950" id3 -->
- Load3Dv2Pro API (supported only by Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
    ```
<!-- end id3 -->

## LoadData (L1 -> L0B-2D Format Fractal Cube Movement)<a name="ZH-CN_TOPIC_0000002543262922"></a>

Load2D implements data movement between the L1 Buffer and the L0B Buffer. It is responsible for moving 2D-format data required for ordinary cube computation, moving data in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0B-2D Format Fractal Cube Movement)](../cube_compute_ISASI/cube_compute_load/Load2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id4 -->
## LoadData (L1 -> L0B-2D Format Fractal Cube Movement V2)

**Load2DV2** implements data movement between the L1 Buffer and the L0B Buffer. It is responsible for moving 2D format data required for ordinary cube computation, moving data in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0B-2D Format Fractal Cube Movement V2)](../cube_compute_ISASI/cube_compute_load/Load2DV2.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Supported only on Ascend 950PR/Ascend 950DT

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id4 -->

<!-- npu="950" id5 -->
## LoadData (L1 -> L0B-2D Format Fractal Cube Movement MX)

Load2DMX is responsible for moving the left and right cube data required for cube computation and the corresponding left and right quantization coefficient cube data. The left and right cube data is moved in units of 512-byte data fractals, and the left and right quantization coefficient matrices are moved in units of 32-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0B-2D Format Fractal Cube Movement MX)](../cube_compute_ISASI/cube_compute_load/Load2DMX.md).

**src** and **srcMx** are source operands, and **dst** is the destination operand; **loadDataParams** and **loadMxDataParams** are movement parameters.

- Load2DMX API (supported only by Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T, typename U = T>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

- Load2Dv2MX API, which supports inconsistent data types between the source operand and the destination operand (supported only by Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```
<!-- end id5 -->

## LoadDataWithTranspose (L1 -> L0B-2D Format Fractal Cube Transpose Movement)<a name="ZH-CN_TOPIC_0000002573902847"></a>

**LoadDataWithTranspose** implements data movement between the L1 Buffer and the L0B Buffer. It is responsible for the 2D-format data movement required by ordinary cube computation. A transpose operation is performed during the movement, which is carried out in units of 512-byte data fractals.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithTranspose (L1 -> L0B-2D Format Fractal Cube Transpose Movement)](../cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id11 -->

Supported only on Ascend 950PR/Ascend 950DT

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id11 -->

## LoadData (L1 -> L0B-3D Format Fractal Cube Movement)<a name="ZH-CN_TOPIC_0000002543422580"></a>

Load3D is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional cube and move it to the corresponding memory location.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (L1 -> L0B-3D Format Fractal Cube Movement)](../cube_compute_ISASI/cube_compute_load/Load3D.md).

src and dst are the source operand and destination operand, respectively; loadDataParams is the movement parameter.

- Load3Dv1 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
    ```

- Load3Dv2 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```

<!-- npu="950" id6 -->
- Load3Dv2Pro API (supported only by Ascend 950PR/Ascend 950DT)

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
    ```
<!-- end id6 -->

<!-- npu="A3,910b" id9 -->
## L1ToL0BLoadDataWithSparse (L1 -> L0B-dense weight cube movement)<a name="ZH-CN_TOPIC_0000002574022821"></a>

Used to move a dense weight cube stored in units of 512 bytes from L1 to L0B, and simultaneously move an index cube in units of 128 bytes to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API).

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [L1ToL0BLoadDataWithSparse (L1 -> L0B-dense weight cube movement)](../cube_compute_ISASI/cube_compute_load/LoadDataWithSparse.md).

- Only the following product models are supported:

    <!-- npu="A3" id7 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id7 -->

    <!-- npu="910b" id8 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id8 -->

    ```cpp
    template <typename T = int8_t, typename U = uint8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, uint8_t>::value, bool>::type = true>
    __aicore__ inline void LoadDataWithSparse(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx, const LoadData2dParams& loadDataParam)
    ```
<!-- end id9 -->

## DataCopy (L0C -> L1 - Inline Quantization Activation Movement)<a name="ZH-CN_TOPIC_0000002543262924"></a>

This API mainly implements data movement from the L0C Buffer to the L1 Buffer, and supports combinations of multiple inline capabilities, which require setting different registers.

The specifically supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L0C Buffer -> L1 Buffer
    - CO1 -> C1

The moved data is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (L0C -> L1 - Inline Quantization Activation Movement)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter.

```cpp
template <typename T, typename U>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
```

## Fixpipe (L0C -> L1 - inline quantization activation movement)<a name="ZH-CN_TOPIC_0000002573902849"></a>

This API mainly implements data movement from the L0C Buffer to the L1 Buffer and supports combinations of multiple inline capabilities. The API includes register configuration and data movement capabilities.

The specific supported data paths are (represented by [logical location TPosition](../aux_data_structures/TPosition.md)):

- L0C Buffer -> L1 Buffer
    - CO1 -> C1

The moved data is the result of [cube computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking Ascend 950PR/Ascend 950DT as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, see [Fixpipe (L0C -> L1 - inline quantization activation movement)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter, and **cbufWorkspace** is the quantization parameter required when tensor quantization is enabled.

- When the inline [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) feature is not enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- When the inline [tensor quantization](../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) feature is enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
