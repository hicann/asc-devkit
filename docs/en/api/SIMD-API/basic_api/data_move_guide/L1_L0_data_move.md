# L1 and L0 Data Movement

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:04:03.958Z pushedAt=2026-10-26T07:41:44.269Z nl=2 -->

## Overview<a name="ZH-CN_TOPIC_0000002543262920"></a>

Data movement between the L1 Buffer and L0 Buffer provides flexible API support and can fully adapt to diverse computation scenarios (see [Table 1](#zh-cn_topic_0000002535057808_table12546123753110)). Specifically, data movement from L1 Buffer to L0A Buffer and from L1 Buffer to L0B Buffer is used for matrix computation (see [Matrix Computation Input](../cube_compute_ISASI/cube_compute_load/cube_compute_load.md)), and the L0C Buffer to L1 Buffer path is used for matrix computation output, completing the output process of matrix computation (see [Matrix Computation Output](../cube_compute_ISASI/cube_compute_store/cube_compute_store.md)).

**Table 1**  Overview of data movement APIs between the L1 Buffer and L0 Buffer<a name="zh-cn_topic_0000002535057808_table12546123753110"></a>

| Data Path | Function | Description |
|----------|------|------|
| L1 Buffer->L0A Buffer | 2D format fractal matrix transfer (LoadData (2D matrix transfer)) | Moves the 2D format data required for normal matrix computation. |
| L1 Buffer->L0A Buffer | 2D format fractal matrix transfer (LoadData (2D matrix transfer V2)) | Moves the 2D format data required for normal matrix computation. |
| L1 Buffer->L0A Buffer | 2D format fractal matrix transfer (LoadData (MX matrix transfer)) | Responsible for moving the left and right matrix data and the corresponding left and right quantization coefficient matrix data required for matrix computation. |
| L1 Buffer->L0A Buffer | 2D format fractal matrix transpose transfer (LoadDataWithTranspose) | Moves the 2D format data required for normal matrix computation. A transpose operation is performed during the transfer. |
| L1 Buffer->L0A Buffer | 3D format fractal matrix transfer (LoadData (convolution data movement)) | Used to perform Image to Column expansion on the NC1HWC0-format Feature Map, and then select the specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. |
| L1 Buffer->L0A Buffer | 3D format fractal matrix transfer (LoadDataWithStride) | Used to perform Image to Column expansion on the NC1HWC0-format Feature Map, and then select the specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. Supports configuring the K-axis offset of the output matrix. |
| L1 Buffer->L0B Buffer | 2D format fractal matrix transfer (LoadData (2D matrix transfer)) | Moves the 2D format data required for normal matrix computation. |
| L1 Buffer->L0B Buffer | 2D format fractal matrix transfer (LoadData (2D matrix transfer V2)) | Moves the 2D format data required for normal matrix computation. |
| L1 Buffer->L0B Buffer | 2D format fractal matrix transfer (LoadData (MX matrix transfer)) | Responsible for moving the left and right matrix data and the corresponding left and right quantization coefficient matrix data required for matrix computation. |
| L1 Buffer->L0B Buffer | 2D format fractal matrix transpose transfer (LoadDataWithTranspose) | Moves the 2D format data required for normal matrix computation. A transpose operation is performed during the transfer. |
| L1 Buffer->L0B Buffer | 3D format fractal matrix transfer (LoadData (convolution data movement)) | Used to perform Image to Column expansion on the NC1HWC0-format Feature Map, and then select the specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. |
| L1 Buffer->L0B Buffer | 3D format fractal matrix transfer (LoadDataWithStride) | Used to perform Image to Column expansion on the NC1HWC0-format Feature Map, and then select the specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. Supports configuring the K-axis offset of the output matrix. |
| L1 Buffer->L0B Buffer | Dense weight matrix transfer (LoadDataWithSparse) | Used to move the dense weight matrix stored in 512-byte units from the L1 Buffer to the L0B Buffer, and simultaneously move the index matrix in 128-byte units to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API). |
| L0C Buffer->L1 Buffer | L0C Buffer->L1 Buffer in-path quantization activation movement (DataCopy) | Supports combining multiple in-path capabilities. Different registers need to be configured, and the DataCopy instruction is used to enable different data transfer capabilities. |
| L0C Buffer->L1 Buffer | L0C Buffer->L1 Buffer in-path quantization activation movement (FixPipe) | Supports combining multiple in-path capabilities. The FixPipe API includes register configuration and data transfer capabilities. |

## L1ToL0A 2D Format Fractal Matrix Movement (LoadData (2D Matrix Movement))<a name="ZH-CN_TOPIC_0000002573902845"></a>

LoadData (2D matrix movement) implements data movement between the L1 Buffer and the L0A Buffer. It is responsible for moving the 2D format data required for normal matrix computation, moving data in units of 512-byte data fractal.

The specific data path supported are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D matrix movement)](../cube_compute_ISASI/cube_compute_load/LoadData_2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id1 -->
## L1ToL0A 2D Format Fractal Matrix Movement (LoadData (2D Matrix Movement V2))

LoadData (2D Matrix Movement V2) implements data movement from the L1 Buffer to the L0A Buffer. It is responsible for moving 2D format data required for normal matrix computation, moving data in units of 512-byte data fractals.

The supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D Matrix Movement V2)](../cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md).

src and dst are the source operand and destination operand, respectively; loadDataParams specifies the movement parameters.

- Supported only on Ascend 950PR&950DT products

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id1 -->

<!-- npu="950" id2 -->
## L1ToL0A 2D Format Fractal Matrix Movement (LoadData (MX Matrix Movement))

LoadData (MX matrix movement) is responsible for moving the left and right matrix data required for matrix computation and the corresponding left and right quantization coefficient matrix data. The left and right matrix data is moved in units of 512-byte data fractals, and the left and right quantization coefficient matrices are moved in units of 32-byte data fractals.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (MX matrix movement)](../cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md).

**src** and **srcMx** are the source operands, and **dst** is the destination operand; **loadDataParams** and **loadMxDataParams** are the movement parameters.

- LoadData (MX matrix movement) API (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T, typename U = T>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

- LoadData (MX matrix movement) API, which supports inconsistent data types between the source operand and the destination operand (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```
<!-- end id2 -->

## L1ToL0A 2D Format Fractal Matrix Movement (LoadDataWithTranspose)<a name="ZH-CN_TOPIC_0000002543422578"></a>

LoadDataWithTranspose implements data movement from the L1 Buffer to the L0A Buffer. It is responsible for the 2D format data movement required for normal matrix computation. A transpose operation is performed during the movement process, and the movement is carried out in units of 512-byte data fractals.

The specific data paths supported are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithTranspose](../cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md).

src and dst are the source operand and destination operand, respectively; loadDataParams specifies the movement parameters.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id10 -->
Supported only on Ascend 950PR&950DT products.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id10 -->

## L1ToL0A 3D Format Fractal Matrix Movement (LoadData (Convolution Data Movement))<a name="ZH-CN_TOPIC_0000002574022819"></a>

LoadData (convolution data movement) is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (convolution data movement)](../cube_compute_ISASI/cube_compute_load/LoadData_3D.md).

src and dst are the source operand and destination operand, respectively; loadDataParams is the movement parameter.

- LoadData (convolution data movement) v1 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
    ```

- LoadData (convolution data movement) v2 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```

<!-- npu="950" id3 -->
- LoadData (convolution data movement) v2Pro API (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
    ```
<!-- end id3 -->

<!-- npu="950" id1 -->
## L1ToL0A 3D Format Fractal Matrix Movement (LoadDataWithStride)<a name="ZH-CN_TOPIC_0000002573312819"></a>

**LoadDataWithStride** is used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. It supports configuring the K-axis direction offset of the output matrix.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0A Buffer
    - A1 -> A2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithStride](../cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Supported only on Ascend 950PR&950DT products

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>,typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadDataWithStride(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```
<!-- end id1 -->

## L1ToL0B 2D Format Fractal Matrix Movement (LoadData (2D Matrix Movement))<a name="ZH-CN_TOPIC_0000002543262922"></a>

**LoadData** (2D matrix movement) implements data movement from the L1 Buffer to the L0B Buffer. It is responsible for moving 2D format data required for normal matrix computation in units of 512-byte data fractals.

The supported data paths are as follows (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D matrix movement)](../cube_compute_ISASI/cube_compute_load/LoadData_2D.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** specifies the movement parameters.

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id4 -->
## L1ToL0B 2D Format Fractal Matrix Movement (LoadData (2D Matrix Movement V2))

**LoadData** (2D matrix movement V2) implements data movement from the L1 Buffer to the L0B Buffer. It is responsible for moving 2D format data required for normal matrix computation, moving data in units of 512-byte data fractal.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (2D matrix movement V2)](../cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Supported only on Ascend 950PR&950DT products

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id4 -->

<!-- npu="950" id5 -->
## L1ToL0B 2D Format Fractal Matrix Movement (LoadData (MX Matrix Movement))

LoadData (MX matrix movement) is responsible for moving the left and right matrix data required for matrix computation and the corresponding left and right quantization coefficient matrix data. The left and right matrix data are moved in units of 512-byte data fractals, and the left and right quantization coefficient matrix is moved in units of 32-byte data fractals.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (MX matrix movement)](../cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md).

src and srcMx are the source operands, and dst is the destination operand; loadDataParams and loadMxDataParams are the movement parameters.

- LoadData (MX matrix movement) API (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T, typename U = T>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```

- LoadData (MX matrix movement) API, which supports inconsistent data types between the source operand and the destination operand (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
    ```
<!-- end id5 -->

## L1ToL0B 2D Format Fractal Matrix Movement (LoadDataWithTranspose)<a name="ZH-CN_TOPIC_0000002573902847"></a>

**LoadDataWithTranspose** implements data movement from the L1 Buffer to the L0B Buffer. It is responsible for the 2D format data movement required for normal matrix computation. A transpose operation is performed during the movement process, and the movement is carried out in units of 512-byte data fractals.

The supported data paths are as follows (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithTranspose](../cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** specifies the movement parameters.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id11 -->
Only Ascend 950PR&950DT products support this.

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id11 -->

## L1ToL0B 3D Format Fractal Matrix Movement (LoadData (Convolution Data Movement))<a name="ZH-CN_TOPIC_0000002543422580"></a>

LoadData (convolution data movement) is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadData (convolution data movement)](../cube_compute_ISASI/cube_compute_load/LoadData_3D.md).

src and dst are the source operand and destination operand, respectively; loadDataParams specifies the movement parameters.

- LoadData (convolution data movement) v1 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
    ```

- LoadData (convolution data movement) v2 API

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```

<!-- npu="950" id6 -->
- LoadData (convolution data movement) v2Pro API (supported only by Ascend 950PR&950DT products)

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
    ```
<!-- end id6 -->

<!-- npu="950" id12 -->
## L1ToL0B 3D Format Fractal Matrix Movement (LoadDataWithStride)<a name="ZH-CN_TOPIC_0000002573312819"></a>

**LoadDataWithStride** is used to perform Image to Column expansion on a Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. It supports configuring the K-axis direction offset of the output matrix.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithStride](../cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md).

**src** and **dst** are the source operand and destination operand, respectively; **loadDataParams** is the movement parameter.

- Only supported on Ascend 950PR&950DT products

    ```cpp
    template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>,typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void LoadDataWithStride(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
    ```
<!-- end id12 -->

<!-- npu="A3,910b" id9 -->
## L1ToL0B Dense Weight Matrix Movement (LoadDataWithSparse)<a name="ZH-CN_TOPIC_0000002574022821"></a>

Used to move the dense weight matrix stored in L1 in 512-byte units to L0B, and simultaneously move the index matrix in 128-byte units to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API).

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L1 Buffer -> L0B Buffer
    - B1 -> B2

The moved data is used for [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [LoadDataWithSparse](../cube_compute_ISASI/cube_compute_load/LoadDataWithSparse.md).

- Only the following product models are supported:

    <!-- npu="A3" id7 -->
    Atlas A3 products;
    <!-- end id7 -->

    <!-- npu="910b" id8 -->
    Atlas A2 products;
    <!-- end id8 -->

    ```cpp
    template <typename T = int8_t, typename U = uint8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, uint8_t>::value, bool>::type = true>
    __aicore__ inline void LoadDataWithSparse(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx, const LoadData2dParams& loadDataParam)
    ```
<!-- end id9 -->

## DataCopy (L0CToL1 In-Path Quantization Activation Movement)<a name="ZH-CN_TOPIC_0000002543262924"></a>

This API mainly moves data from the L0C Buffer to the L1 Buffer and supports combinations of multiple in-path capabilities, which require different registers to be set.

The specific supported data paths are (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> L1 Buffer
    - CO1 -> C1

The moved data is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). For details about the API, see [DataCopy (L0C-to-L1 Data Movement)](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter.

```cpp
template <typename T, typename U>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
```

## Fixpipe (L0CToL1 In-Path Quantization Activation Movement)<a name="ZH-CN_TOPIC_0000002573902849"></a>

This API mainly implements data movement from the L0C Buffer to the L1 Buffer and supports combinations of multiple in-path capabilities. The API includes register configuration and data movement capabilities.

The specific supported data paths are as follows (represented by [logical location TPosition](../../basic_api/aux_data_structures/TPosition.md)):

- L0C Buffer -> L1 Buffer
    - CO1 -> C1

The moved data is the result of [matrix computation](../cube_compute_ISASI/cube_compute_ISASI.md). Taking the Ascend 950PR&950DT products as an example, the API example is as follows:

Note that the API prototypes may differ across product models. For details, see [Fixpipe (L0C to L1 Data Movement)](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1.md).

**src** and **dst** are the source operand and destination operand, respectively; **intriParams** is the movement parameter, and **cbufWorkspace** is the quantization parameter required when tensor quantization is enabled.

- Without enabling the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- With the in-path [tensor quantization](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
