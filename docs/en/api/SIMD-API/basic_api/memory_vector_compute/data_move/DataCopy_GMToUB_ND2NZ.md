# DataCopy (GM to UB Inline Conversion ND2NZ Data Movement)<a name="ZH-CN_TOPIC_0000002349187356"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:37:51.544Z pushedAt=2026-10-28T03:14:39.492Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id16 -->
- Ascend 950PR&950DT products: supported
<!-- end id16 -->
<!-- npu="A3" id17 -->
- Atlas A3 products: supported
<!-- end id17 -->
<!-- npu="910b" id18 -->
- Atlas A2 products: supported
<!-- end id18 -->
<!-- npu="310b" id19 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id19 -->
<!-- npu="310p" id20 -->
- Atlas inference products AI Core: supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products Vector Core: not supported
<!-- end id21 -->
<!-- npu="910" id22 -->
- Atlas training products: not supported
<!-- end id22 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_ND2NZ_res.md#id1 -->

## Description<a name="section12840195813362"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API is a software simulation API designed for ease of use. It supports ND-to-NZ format conversion during data movement from Global Memory to Unified Buffer (UB).

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN

## Prototype<a name="section1954364615315"></a>

- The enableSmallC0 mode is not supported.

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- The enableSmallC0 mode is supported (only supported by Ascend 950PR&950DT products).

    ```cpp
    // This function prototype is supported only on Ascend 950PR&950DT products.
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

## Parameters<a name="section1251613311396"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#section4219135304818). |
| enableSmallC0 | SmallC0 mode switch: when dValue is less than or equal to 4, C0_SIZE is padded to 4 * sizeof(T) bytes.<br>Disabled by default. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md) type. |
| src | Input | Source operand, of the [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md) type. |
| intriParams | Input | Copy parameters, of the Nd2NzParams type. For details about the parameters, see [Table 3](#table_nd2nz_3).<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3**  Nd2NzParams structure parameter definition<a name="table_nd2nz_3"></a>

| Parameter | Description |
| :--- | :--- |
| ndNum | Number of ND matrices to be transferred. Value range: ndNum∈[0, 4095]. |
| nValue | Number of rows of the ND matrix. Value range: nValue∈[0, 16384]. |
| dValue | Number of columns of the ND matrix. Value range: dValue∈[0, 65535]. |
| srcNdMatrixStride | Offset between the start addresses of adjacent ND matrices in the source operand. Value range: srcNdMatrixStride∈[0, 65535], in elements. |
| srcDValue | Offset between the start addresses of adjacent rows of the same ND matrix in the source operand. Value range: srcDValue∈[1, 65535], in elements. |
| dstNzC0Stride | After ND is converted to NZ format, one row in the source operand is converted to multiple rows in the destination operand. dstNzC0Stride indicates the offset between the start addresses of adjacent rows of the multiple rows that come from the same row of the source operand in the destination NZ matrix. Value range: dstNzC0Stride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzNStride | Offset between the start addresses of adjacent rows of the Z-shaped matrix in the destination NZ matrix. Value range: dstNzNStride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzMatrixStride | Offset between the start addresses of adjacent NZ matrices in the destination NZ matrix. Value range: dstNzMatrixStride∈[1, 65535], in elements. |

The ND2NZ conversion diagram is as follows. The parameter values set in the example and their explanations are as follows:

- ndNum = 2, indicating that the number of ND matrices to be transferred is 2 \(ND matrix 1 is A1\~A2 + B1\~B2, and ND matrix 2 is C1\~C2 + D1\~D2\).
- nValue = 2, the number of rows of the ND matrix, that is, the height of the matrix is 2.
- **dValue** = 24, the number of columns in the ND matrix, that is, the matrix width is 24 elements. When **dValue** does not satisfy 32-byte alignment, the insufficient part in the destination operand is padded with 0. For example, the blank part of the DataBlock where A2 resides in the figure is padded with 0.
- srcNdMatrixStride = 144, indicating the inter-address offset between the start addresses of adjacent ND matrices, that is, the distance of A1\~C1, which is 9 DataBlocks, 9 \* 16 = 144 elements.
- srcDValue = 48, indicating the number of elements contained in one row, that is, the distance from A1 to B1, which is 3 DataBlocks, 3 \* 16 = 48 elements.
- **dstNzC0Stride** = 11. After the ND-to-NZ format conversion, one row in the source operand is converted into multiple rows in the destination operand. For example, A1 and A2 in src form one row, while A1 and A2 in dst are divided into two rows. The offset between the start addresses of the multiple rows is the offset of A1 and A2 in dst, which is 11 DataBlocks.
- **dstNzNStride** = 2, indicating the offset in dst after the x-th row and the (x+1)-th row of an ND matrix in src are converted to the NZ format, that is, the offset between A1 and B1 in dst is 2 DataBlocks.
- dstNzMatrixStride = 96, indicating the offset between the start of the x-th ND matrix and the start of the (x+1)-th ND matrix in dst, that is, the distance between A1 and C1, which is 6 DataBlocks, 6 \* 16 = 96 elements.

**Figure 1**  ND2NZ conversion diagram (half data type)<a name="fig128961542184620"></a>  
![](../../../../figures/nd2nz_conversion_half.png "ND2NZ conversion diagram (half data type)")

The ND2NZ conversion diagram in enableSmallC0 enabled mode is as follows:

**Figure 2**  ND2NZ conversion diagram in enableSmallC0 enable mode (half data type)<a name="fig07641913195410"></a>  
![](../../../../figures/enable_smallc0_nd2nz_conversion_half.png "ND2NZ conversion diagram in enableSmallC0 enable mode (half data type)")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, and complex32.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- For the Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id5 -->

## Return Value<a name="section446456163012"></a>

None

## Constraints<a name="section1140315215118"></a>

- This API is a software-simulated API designed for ease of use.
- The values of the **Nd2NzParams** structure parameters must be within the value ranges:

    **Table 4**  Value ranges of the Nd2NzParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdMatrixStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzMatrixStride | [1, 65535] |

<!-- npu="310p" id6 -->
- For the AI Core of Atlas inference products, reserve 8 KB of UB space as the temporary data storage area for the API.
<!-- end id6 -->

## Examples<a name="section10309141400"></a>

For intriParams parameter parsing, see [Figure 1](#fig128961542184620).

```cpp
// dstLocal is a LocalTensor of the half type on the UB, and srcGlobal is a GlobalTensor of the half type.
AscendC::Nd2NzParams intriParams{1, 32, 32, 0, 32, 32, 1, 0};
// Global Memory -> Local Memory
AscendC::DataCopy(dstLocal, srcGlobal, intriParams);
```

Result example:

```text
Input data (srcGlobal): [1 2 3 ... 1024]
Output data (dstLocal):[1 2 ... 15 16 33 34 ... 47 48 65 66 ... 79 80 97 98 ... 111 112 ... 1009 1010... 1023 1024]
```
