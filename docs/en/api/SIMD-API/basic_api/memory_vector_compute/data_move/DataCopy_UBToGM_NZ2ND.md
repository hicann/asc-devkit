# DataCopy (UBToGM inline conversion NZ2ND transfer)<a name="ZH-CN_TOPIC_0000002391805265"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:42:08.013Z pushedAt=2026-10-28T03:14:39.497Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id13 -->
- Ascend 950PR&950DT products: supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 products: supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 products: supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: not supported
<!-- end id19 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_UBToGM_NZ2ND_res.md#id1 -->

## Description<a name="section12840195813362"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API is a software simulation API designed for ease of use. It supports NZ-to-ND format conversion during data movement from Unified Buffer (UB) to Global Memory.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id1 -->

## Prototype<a name="section1792117555586"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
```

## Parameters<a name="section14983445508"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md) type. |
| src | Input | Source operand, of the [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md) type. |
| intriParams | Input | Copy parameters, of the Nz2NdParamsFull type. For details about the parameters, see [Table 3](#table_nz2nd_3).<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3**  Parameter definition of the Nz2NdParamsFull structure<a name="table_nz2nd_3"></a>

| Parameter | Description |
| :--- | :--- |
| ndNum | Number of NZ matrices to be transferred. Value range: ndNum∈[0, 4095]. |
| nValue | Number of rows of the NZ matrix. Value range: nValue∈[1, 8192]. |
| dValue | Number of columns of the NZ matrix. Value range: dValue∈[1, 8192]. dValue must be a multiple of 16. |
| srcNdMatrixStride | Offset between adjacent source NZ matrices (head to head). Value range: srcNdMatrixStride∈[1, 512], in units of 256 (16 \* 16) elements. |
| srcNStride | Offset between adjacent Z layouts of the same source NZ matrix (head to head). Value range: srcNStride∈[0, 4096], in units of 16 elements. |
| dstDStride | Offset between adjacent rows of the destination ND matrix (head to head). Value range: dstDStride∈[1, 65535], in elements. |
| dstNdMatrixStride | Offset in the destination ND matrix from adjacent source NZ matrices (head to head). Value range: dstNdMatrixStride∈[1, 65535], in elements. |

Taking the half data type as an example, the NZ2ND conversion diagram is as follows. The parameter values set in the example and their explanations are as follows:

- ndNum = 2, indicating that the number of source NZ matrices is 2 (NZ matrix 1 is A1\~A4 + B1\~B4, and NZ matrix 2 is C1\~C4 + D1\~D4).
- nValue = 4, the number of rows of the NZ matrix, that is, the height of the matrix is 4.
- dValue = 32, the number of columns of the NZ matrix, that is, the width of the matrix is 32 elements.
- srcNdMatrixStride = 1, indicating the offset between the start addresses of adjacent NZ matrices, that is, the distance of A1\~C1, which is 256 elements (16 DataBlocks \* 16 elements).
- srcNStride = 4, indicating the offset between adjacent Z layouts of the same source NZ matrix, that is, the distance from A1 to B1, which is 64 elements (4 DataBlocks \* 16 elements).
- dstDStride = 160, indicating the offset between adjacent rows of a destination ND matrix, that is, the distance between A1 and A2, which is 10 DataBlocks, that is, 10 \* 16 = 160 elements.
- dstNdMatrixStride = 48, indicating the offset between the start of the x-th destination ND matrix and the start of the (x+1)-th destination ND matrix in dst, that is, the distance between A1 and C1, which is 3 DataBlocks, 3 \* 16 = 48 elements.

**Figure 1**  NZ2ND conversion diagram (half data type)<a name="fig15851251122815"></a>  
![](../../../../figures/nz2nd_conversion_half.png "NZ2ND conversion diagram (half data type)")

Taking the float data type as an example, the NZ2ND conversion diagram is as follows. The parameter values and explanations in the example are as follows:

- ndNum = 2, indicating that the number of source NZ matrices is 2 (NZ matrix 1 is A1\~A8 + B1\~B8, and NZ matrix 2 is C1\~C8 + D1\~D8).
- nValue = 4, the number of rows of the NZ matrix, that is, the height of the matrix is 4.
- dValue = 32, the number of columns of the NZ matrix, that is, the width of the matrix is 32 elements.
- srcNdMatrixStride = 1, indicating the offset between the start addresses of adjacent NZ matrices, that is, the distance from A1 to C1, which is 256 elements (32 DataBlocks \* 8 elements).
- srcNStride = 4, indicating the offset between adjacent Z layouts of the same source NZ matrix, that is, the distance from A1 to B1, which is 64 elements (8 DataBlocks \* 8 elements).
- dstDStride = 144, indicating the offset between adjacent rows of a destination ND matrix, that is, the distance between A1 and A3, which is 18 DataBlocks, that is, 18 \* 8 = 144 elements.
- dstNdMatrixStride = 40, indicating the offset between the start of the x-th destination ND matrix and the start of the (x+1)-th destination ND matrix in dst, that is, the distance between A1 and C1, which is 5 DataBlocks, 5 \* 8 = 40 elements.

**Figure 2**  NZ2ND conversion diagram (float data type)<a name="fig5586175192811"></a>  
![](../../../../figures/nz2nd_conversion_float.png "NZ2ND conversion diagram (float data type)")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, and complex64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 products, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 products, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core, the supported data types are: int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id5 -->

## Return Value<a name="section129001927113216"></a>

None

## Constraints<a name="section830051273220"></a>

- This API is a software simulation API designed for ease of use.
- The values of the Nz2NdParamsFull structure parameters must be within the value ranges:

    **Table 4** Value ranges of the Nz2NdParamsFull structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | ndNum | [0, 4095] |
    | nValue | [1, 8192] |
    | dValue | [1, 8192] |
    | srcNdMatrixStride | [1, 512] |
    | srcNStride | [0, 4096] |
    | dstDStride | [1, 65535] |
    | dstNdMatrixStride | [1, 65535] |

## Examples<a name="section2409153316111"></a>

For intriParams parameter parsing, refer to [Figure 1](#fig15851251122815).

```cpp
// srcLocal is a LocalTensor of the half type, and dstGlobal is a GlobalTensor of the half type.
AscendC::Nz2NdParamsFull intriParams{1, 32, 32, 1, 32, 32, 1};
// Local Memory -> Global Memory
AscendC::DataCopy(dstGlobal, srcLocal, intriParams);
```

Result example:

```text
Input data (srcLocal): [1 2 3 ... 1024]
Output data (dstGlobal):[1 2 ... 15 16 513 514 ... 527 528 17 18 ... 31 32 529 530 ... 543 544 ...497 498 ...  511 512  1009 1010... 1023 1024]
```
