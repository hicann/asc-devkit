# UB-to-GM Data Transfer with NZ2ND Conversion (DataCopy)<a name="ZH-CN_TOPIC_0000002391805265"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:31:51.849Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: Not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: Not supported
<!-- end id19 -->

## Description<a name="section12840195813362"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

This API is a software simulation API designed from the perspective of ease of use. It supports NZ-to-ND format conversion during data copy from Unified Buffer to Global Memory.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only by the AI Core of Atlas inference series products)
    <!-- end id1 -->

## Prototype<a name="section1792117555586"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
```

## Parameters<a name="section14983445508"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source or destination operand. For supported data types, refer to [Data Type](#section4219135304818). |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, whose type is [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md). |
| src | Input | Source operand, whose type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md). |
| intriParams | Input | Transfer parameters, whose type is Nz2NdParamsFull. For details about the parameters, refer to [Table 3](#table_nz2nd_3).<br>For the definition, refer to \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation. |

**Table 3** Parameter definitions in the Nz2NdParamsFull structure<a name="table_nz2nd_3"></a>

| Parameter | Description |
| :--- | :--- |
| ndNum | Number of NZ matrices to transfer. Value range: ndNum∈[0, 4095]. |
| nValue | Number of rows of the NZ matrix. Value range: nValue∈[1, 8192]. |
| dValue | Number of columns of the NZ matrix. Value range: dValue∈[1, 8192]. dValue must be a multiple of 16. |
| srcNdMatrixStride | Offset between adjacent source NZ matrices (head-to-head). Value range: srcNdMatrixStride∈[1, 512], in units of 256 (16 \* 16) elements. |
| srcNStride | Offset between adjacent Z arrangements of the same source NZ matrix (head-to-head). Value range: srcNStride∈[0, 4096], in units of 16 elements. |
| dstDStride | Offset between adjacent rows of the destination ND matrix (head-to-head). Value range: dstDStride∈[1, 65535], in units of elements. |
| dstNdMatrixStride | Offset in the destination ND matrix from adjacent source NZ matrices (head-to-head). Value range: dstNdMatrixStride∈[1, 65535], in units of elements. |

Taking the half data type as an example, the NZ2ND conversion diagram is as follows. The parameter values and explanations in the example are as follows:

- ndNum = 2, indicating that the number of source NZ matrices is 2 (NZ matrix 1 is A1\~A4 + B1\~B4, and NZ matrix 2 is C1\~C4 + D1\~D4).
- nValue = 4, the number of rows of the NZ matrix, that is, the matrix height is 4.
- dValue = 32, the number of columns of the NZ matrix, that is, the matrix width is 32 elements.
- srcNdMatrixStride = 1, indicating the offset between the start addresses of adjacent NZ matrices, that is, the distance of A1\~C1, which is 256 elements (16 DataBlocks \* 16 elements).
- srcNStride = 4, indicating the offset between adjacent Z arrangements of the same source NZ matrix, that is, the distance from A1 to B1, which is 64 elements (4 DataBlocks \* 16 elements).
- dstDStride = 160, indicating the offset between adjacent rows of a destination ND matrix, that is, the distance between A1 and A2, which is 10 DataBlocks, that is, 10 \* 16 = 160 elements.
- dstNdMatrixStride = 48, indicating the offset between the start of the x-th destination ND matrix and the start of the (x+1)-th destination ND matrix in dst, that is, the distance between A1 and C1, which is 3 DataBlocks, 3 \* 16 = 48 elements.

**Figure 1** NZ2ND conversion diagram (half data type)<a name="fig15851251122815"></a>  
![](../../../../figures/nz2nd_conversion_half.png "NZ2ND conversion diagram (half data type)")

Taking the float data type as an example, the NZ2ND conversion diagram is as follows. The parameter setting values and explanations in the example are as follows:

- ndNum = 2, indicating that the number of source NZ matrices is 2 (NZ matrix 1 is A1\~A8 + B1\~B8, and NZ matrix 2 is C1\~C8 + D1\~D8).
- nValue = 4, the number of rows of the NZ matrix, that is, the matrix height is 4.
- dValue = 32, the number of columns of the NZ matrix, that is, the matrix width is 32 elements.
- srcNdMatrixStride = 1, indicating the offset between the start addresses of adjacent NZ matrices, that is, the distance from A1 to C1, which is 256 elements (32 DataBlocks \* 8 elements).
- srcNStride = 4, indicating the offset between adjacent Z arrangements of the same source NZ matrix, that is, the distance from A1 to B1, which is 64 elements (8 DataBlocks \* 8 elements).
- dstDStride = 144, indicating the offset between adjacent rows of a destination ND matrix, that is, the distance between A1 and A3, which is 18 DataBlocks, that is, 18 \* 8 = 144 elements.
- dstNdMatrixStride = 40, indicating the offset between the start of the x-th destination ND matrix and the start of the (x+1)-th destination ND matrix in dst, that is, the distance between A1 and C1, which is 5 DataBlocks, 5 \* 8 = 40 elements.

**Figure 2**  NZ2ND conversion diagram (float data type)<a name="fig5586175192811"></a>  
![](../../../../figures/nz2nd_conversion_float.png "NZ2ND conversion diagram (float data type)")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT, supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training series products/Atlas A3 inference series products, supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training series products/Atlas A2 inference series products, supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference series products AI Core support the following data types: int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id5 -->

## Return Value<a name="section129001927113216"></a>

None

## Constraints<a name="section830051273220"></a>

- This API is a software simulation API designed from the perspective of ease of use.
- The values of the parameters in the Nz2NdParamsFull structure must be within their value ranges:

    **Table 4**  Value ranges of the Nz2NdParamsFull structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | ndNum | [0, 4095] |
    | nValue | [1, 8192] |
    | dValue | [1, 8192] |
    | srcNdMatrixStride | [1, 512] |
    | srcNStride | [0, 4096] |
    | dstDStride | [1, 65535] |
    | dstNdMatrixStride | [1, 65535] |

## Calling Example<a name="section2409153316111"></a>

For details about intriParams parameter parsing, refer to [Figure 1](#fig15851251122815).

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
