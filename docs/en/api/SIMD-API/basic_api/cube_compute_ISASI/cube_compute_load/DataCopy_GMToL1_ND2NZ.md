# DataCopy (GMToL1 Inline Conversion - ND2NZ Copy)<a id="ZH-CN_TOPIC_0000002569070913"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:33:45.346Z pushedAt=2026-10-26T07:41:44.071Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002566538879_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002566538879_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly copies a matrix from Global Memory to the L1 Buffer (with TPosition being A1/B1), and supports ND-to-NZ format conversion during the data copy.

The ND-to-NZ format conversion is equivalent to the DN-to-ZN format conversion, as shown in [Figure 1](#zh-cn_topic_0000002566538879_fig444462417355).

**Figure 1** ND2NZ and DN2ZN conversion diagram<a id="zh-cn_topic_0000002566538879_fig444462417355"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_copy.png)

## Prototype<a id="zh-cn_topic_0000002566538879_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```

<!-- npu="950" id10 -->
Specifically for the Ascend 950PR&950DT products, the function prototype is as follows:

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```
<!-- end id10 -->

## Parameters<a id="zh-cn_topic_0000002566538879_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002566538879_section4219135304818). |
| enableSmallC0 | SmallC0 mode switch: when dValue is less than or equal to 4, C0_SIZE is padded to 4 * sizeof(T) bytes.<br>Disabled by default. The support for enableSmallC0 varies by model. For details, see [enableSmallC0 Parameter Support](#enablesmallc0-parameter-support). |

### enableSmallC0 Parameter Support

<!-- npu="950" id11 -->
- For the Ascend 950PR&950DT products, the **enableSmallC0** parameter is newly added. When calling this API, you must configure the **enableSmallC0** parameter.
<!-- end id11 -->

- Other models do not support this parameter.

**Table 2** Parameters

| Parameter Name | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type, stored in the L1 Buffer (**TPosition** is A1/B1). The destination address must be 32-byte aligned. |
| src | Input | Source operand, of the **GlobalTensor** type, stored in Global Memory. The source address must be 1-byte aligned. |
| intriParams | Input | ND2NZ copy parameters, of the **Nd2NzParams** type.<br>For details about the **Nd2NzParams** parameters, see [Table 4](#zh-cn_topic_0000002566538879_table144203616291). |

**Table 3** Nd2NzParams Structure Parameter Definition<a id="zh-cn_topic_0000002566538879_table144203616291"></a>

| Parameter Name | Meaning |
| ---------- | ---------- |
| ndNum | Number of ND matrices in the source matrix. Value range: ndNum∈[0, 4095].<br>**Note: When ndNum = 0, no copy is performed, and this API is treated as a NOP (No Operation).** |
| nValue | Number of rows of the ND matrix in the source matrix. Value range: nValue∈[0, 16384].<br>**Note: When nValue = 0, no copy is performed, and this API is treated as a NOP (No Operation).** |
| dValue | Number of columns of the ND matrix in the source matrix. Value range: dValue∈[0, 65535]. When dValue * sizeof(T) does not satisfy 32-byte alignment, the destination matrix is zero-padded to 32-byte alignment.<br>**Note: When dValue = 0, no copy is performed, and this API is treated as a NOP (No Operation).** |
| srcNdMatrixStride | Address offset between the start addresses of adjacent ND matrices in the source matrix. Value range: srcNdMatrixStride∈[0, 65535], in elements.<br>&nbsp;&nbsp;&bull; When ndNum = 1, srcNdMatrixStride is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When ndNum ≠ 1 and srcNdMatrixStride = 0, the first ND matrix of the source matrix is copied repeatedly. |
| srcDValue | Number of elements contained in one row of the source matrix. Value range: srcDValue∈[1, 65535], in elements. |
| dstNzC0Stride | After ND-to-NZ conversion, the address offset between the start addresses of adjacent Z matrices in the destination NZ matrix. Value range: dstNzC0Stride∈[1, 16384], in C0_SIZE (32 bytes). |
| dstNzNStride | Address offset between the start addresses of adjacent rows in the NZ matrix of the destination matrix. Value range: dstNzNStride∈[1, 16384], in C0_SIZE (32 bytes). |
| dstNzMatrixStride | Address offset between the start addresses of adjacent NZ matrices in the destination matrix. Value range: dstNzMatrixStride∈[0, 16384], in elements.<br>&nbsp;&nbsp;&bull; When ndNum = 1, dstNzMatrixStride is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When ndNum ≠ 1 and dstNzMatrixStride = 0, each NZ matrix copied into the destination matrix overwrites the first NZ matrix. |

## Data Type<a id="zh-cn_topic_0000002566538879_section4219135304818"></a>

The data types supported by the source matrix and the destination matrix must be consistent.

<!-- npu="950" id12 -->
For the Ascend 950PR&950DT products, the supported data types are: **bool**, **int8_t**, **uint8_t**, **fp4x2_e2m1_t**, **fp4x2_e1m2_t**, **hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**, **fp8_e8m0_t**, **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**, **complex32**.
<!-- end id12 -->

<!-- npu="A3" id13 -->
For the Atlas A3 products, the supported data types are: **b4** (**int4b_t**), **int8_t**, **uint8_t**, **int16_t**, **uint16_t**, **int32_t**, **uint32_t**, **half**, **bfloat16_t**, **float**.
<!-- end id13 -->

<!-- npu="910b" id14 -->
For the Atlas A2 products, the supported data types are: **b4** (**int4b_t**), **int8_t**, **uint8_t**, **int16_t**, **uint16_t**, **int32_t**, **uint32_t**, **half**, **bfloat16_t**, **float**.
<!-- end id14 -->

<!-- npu="310p" id15 -->
For the Atlas inference products AI Core, the supported data types are: **int16_t**, **uint16_t**, **int32_t**, **uint32_t**, **half**, **float**.
<!-- end id15 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002566538879_section2045914466492"></a>

- When the input data type is b4, the data is processed based on the b8 type during ND2NZ. Therefore, the parameters must be set according to the b8 type.
- The source address in Global Memory must be 1-byte aligned, and the destination address in L1 Buffer must be 32-byte aligned.
- When any of the **Nd2NzParams** structure parameters **ndNum**, **nValue**, or **dValue** is 0, the instruction is not executed, and the API is treated as a NOP (No Operation).
- The data copied to L1 Buffer must not overlap. If overlapping writes exist, the hardware does not generate any warning or error, and the write order of the overlapping data is not guaranteed.
- The values of the **Nd2NzParams** structure parameters must be within the value ranges:

    **Table 4** Value ranges of the Nd2NzParams structure parameters

    | Parameter Name | Value Range |
    | ---------- | ---------- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdMatrixStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzMatrixStride | [0, 16384] |

<!-- npu="310p" id18 -->
- For Atlas inference products AI Core, when using the ND2NZ copy API over the Global Memory -\> Local Memory path, reserve 8K of Unified Buffer (UB) space as the temporary data storage area for the API.
<!-- end id18 -->

## Key Features

### Continuous Copy<a id="zh-cn_topic_0000002566538879_section7495192151713"></a>

Taking the half data type as an example, [Figure 2](#zh-cn_topic_0000002566538879_fig7420114233419) shows the ND2NZ continuous copy diagram. The **Nd2NzParams** structure parameters are configured as follows:

- **ndNum** = 1, indicating that the number of ND matrices to be copied is 1.
- **nValue** = 16, the number of rows in one ND matrix.
- **dValue** = 23, the number of columns in one ND matrix. When dValue * sizeof(T) does not satisfy 32-byte alignment, zeros are padded in the destination matrix to align to 32 bytes.
- **srcNdMatrixStride** = 0. Since there is only one ND matrix, this value is 0.
- **srcDValue** = 32, indicating the element count in one row of the source matrix, that is, 32 elements of the half data type.
- **dstNzC0Stride** = 16, the address offset between the start addresses of adjacent Z matrices in the NZ matrix, with an offset of 16 **C0_SIZE**.
- **dstNzNStride** = 1, indicating the address offset between the start addresses of adjacent rows in the NZ matrix, with an offset of 1 **C0_SIZE**.
- **dstNzMatrixStride** = 0. Since there is only one NZ matrix, this value is 0.

**Figure 2** ND2NZ conversion diagram (continuous copy)<a id="zh-cn_topic_0000002566538879_fig7420114233419"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_continuous_copy.png)

### Non-contiguous Copy<a id="zh-cn_topic_0000002566538879_section1721231581717"></a>

Taking the half data type as an example, [Figure 3](#zh-cn_topic_0000002566538879_fig19978184883316) shows the ND2NZ non-contiguous copy diagram. The **Nd2NzParams** structure parameters are configured as follows:

- **ndNum** = 2, indicating that the number of ND matrices to be copied is 2.
- **nValue** = 8, the number of rows in one ND matrix.
- **dValue** = 23, the number of columns in one ND matrix. When **dValue** * sizeof(T) does not satisfy 32-byte alignment, zeros are padded in the destination matrix to align to 32 bytes.
- **srcNdMatrixStride** = 384, indicating the address offset between the start addresses of adjacent ND matrices. Each row has 32 elements and there are 12 rows in total, so the offset is 32 * 12 = 384 elements.
- **srcDValue** = 32, indicating the number of elements in one row of the source matrix, that is, 32 elements of the half data type.
- **dstNzC0Stride** = 37, the address offset between the start addresses of adjacent Z matrices in the NZ matrix, with an offset of 37 **C0_SIZE**.
- **dstNzNStride** = 2, indicating the address offset between the start addresses of adjacent rows in the NZ matrix, with an offset of 2 **C0_SIZE**.
- **dstNzMatrixStride** = 320, the address offset between the start addresses of adjacent NZ matrices. Each row has 16 elements and there are 20 rows in total, so the offset is 16 * 20 = 320 elements.

**Figure 3** ND2NZ conversion diagram (non-contiguous copy)<a id="zh-cn_topic_0000002566538879_fig19978184883316"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_noncontinuous_copy.png)

### SmallC0 Mode

The ND2NZ conversion diagram in enableSmallC0 enabled mode is as follows:

**Figure 4** ND2NZ conversion diagram in enableSmallC0 enabled mode (half data type)<a name="fig07641913195410"></a>  

![](../../../../figures/nd2nz_small_c0_half.png "ND2NZ conversion diagram in enableSmallC0 mode (half data type)")

## Example<a id="zh-cn_topic_0000002566538879_section088124295117"></a>

In the following example, **DataCopy** is used to perform inline ND2NZ data copy in a scenario where matrix A is not transposed and the data type is half.

The following figure shows the data layout changes during the copy process:

![](../../../../figures/datacopy_gm2l1_nd2nz_demo.png)

The sample code snippet is as follows. Only part of the code in the sample is shown. For the complete sample, see [DataCopy_GM2L1 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
// m=40,k=56,fractalShape[0] = 16,fractalShape[1] = 16,fractalSize = 16 * fractalShape[1]
AscendC::Nd2NzParams nd2nzA1Params;
// Number of ND matrices to transfer.
nd2nzA1Params.ndNum = 1;
// Number of rows in the ND matrix.
nd2nzA1Params.nValue = m;
// Number of columns in the ND matrix.
nd2nzA1Params.dValue = k;
// Only one ND matrix is transferred, so this parameter is invalid.
nd2nzA1Params.srcNdMatrixStride = 0;
// Number of elements in one row of the source matrix.
nd2nzA1Params.srcDValue = k;
// This parameter is the aligned length of the A matrix in the height direction on the L1 Buffer.
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```
