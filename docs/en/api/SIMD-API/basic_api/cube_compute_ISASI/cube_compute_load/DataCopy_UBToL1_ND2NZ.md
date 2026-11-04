# DataCopy (UBToL1 In-line Conversion - ND2NZ Copy)<a id="ZH-CN_TOPIC_0000002349187356"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:41:51.347Z pushedAt=2026-10-26T07:41:44.079Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ_res.md#id1 -->

## Description<a id="section12840195813362"></a>

> [!NOTE]Note
> This API is implemented by software simulation. It is built on the Matmul high-level API and uses the workspace GM space in the Matmul high-level API as the data relay space. Data is first moved into GM and then into the L1 Buffer. Therefore, when using this API, you must first use REGISTER_MATMUL to register the high-level API.

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports ND-to-NZ format conversion during data movement. Data is moved from the Unified Buffer (UB, with TPosition being VECIN/VECCALC/VECOUT) to the L1 Buffer, and the ND-to-NZ format conversion is completed during the movement.

## Prototype<a id="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
```

## Parameters<a id="section1251613311396"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#section4219135304818). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the LocalTensor type, stored in the L1 Buffer (TSCM). |
| src | Input | Source operand, of the LocalTensor type, stored in UB (TPosition is VECIN/VECCALC/VECOUT). |
| intriParams | Input | Copy parameters, of the [Nd2NzParams](#table844881954715) type.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** Nd2NzParams structure parameter definition<a id="table844881954715"></a>

| Parameter | Description |
| ---------- | ---------- |
| ndNum | Number of ND matrices to be transferred. Value range: ndNum∈[0, 4095]. |
| nValue | Number of rows of the ND matrix. Value range: nValue∈[0, 16384]. |
| dValue | Number of columns of the ND matrix. Value range: dValue∈[0, 65535]. |
| srcNdMatrixStride | Offset between the start addresses of adjacent ND matrices in the source operand. Value range: srcNdMatrixStride∈[0, 65535], in elements. |
| srcDValue | Offset between the start addresses of adjacent rows of the same ND matrix in the source operand. Value range: srcDValue∈[1, 65535], in elements. |
| dstNzC0Stride | After ND is converted to NZ format, one row in the source operand is converted to multiple rows in the destination operand. dstNzC0Stride indicates the offset between the start addresses of adjacent rows of the multiple rows that come from the same row of the source operand in the destination NZ matrix. Value range: dstNzC0Stride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzNStride | Offset between the start addresses of adjacent rows of the Z-shaped matrix in the destination NZ matrix. Value range: dstNzNStride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzMatrixStride | Offset between the start addresses of adjacent NZ matrices in the destination NZ matrix. Value range: dstNzMatrixStride∈[0, 16384], in elements. |

The ND2NZ conversion diagram is as follows. The parameter values set in the example and their explanations are as follows:

- ndNum = 2, indicating that the number of ND matrices to be transferred is 2 (ND matrix 1 is A1~A2 + B1~B2, and ND matrix 2 is C1~C2 + D1~D2).
- nValue = 2, the number of rows of the ND matrix, that is, the height of the matrix is 2.
- **dValue** = 24, the number of columns in the ND matrix, that is, the matrix width is 24 elements. When **dValue** does not satisfy 32-byte alignment, the insufficient part in the destination operand is padded with 0. For example, the blank part of the DataBlock where A2 resides in the figure is padded with 0.
- **srcNdMatrixStride** = 144, indicating the inter-address offset between the start addresses of adjacent ND matrices, that is, the distance from A1 to C1, which is 9 DataBlocks, 9 * 16 = 144 elements.
- **srcDValue** = 48, indicating the number of elements contained in one row, that is, the distance from A1 to B1, which is 3 DataBlocks, 3 * 16 = 48 elements.
- **dstNzC0Stride** = 11. After the ND-to-NZ format conversion, one row in the source operand is converted into multiple rows in the destination operand. For example, A1 and A2 in src form one row, while A1 and A2 in dst are divided into two rows. The offset between the start addresses of the multiple rows is the offset of A1 and A2 in dst, which is 11 DataBlocks.
- **dstNzNStride** = 2, indicating the offset in dst after the x-th row and the (x+1)-th row of an ND matrix in src are converted to the NZ format, that is, the offset between A1 and B1 in dst is 2 DataBlocks.
- **dstNzMatrixStride** = 96, indicating the offset between the start of the x-th ND matrix and the start of the (x+1)-th ND matrix in dst, that is, the distance between A1 and C1, which is 6 DataBlocks, 6 * 16 = 96 elements.

**Figure 1** ND2NZ conversion diagram (half data type)<a id="fig128961542184620"></a>

![](../../../../figures/nd2nz_conversion_half.png)

## Data Type<a id="section4219135304818"></a>

The source matrix and the destination matrix support the same data types.

<!-- npu="950" id10 -->
For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, bfloat16_t, float.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, bfloat16_t, float.
<!-- end id12 -->

## Return Value

None

## Constraints

- This API is implemented through software simulation. It first performs data fractal transformation, and then copies the data using move instructions.
- This API internally allocates a temporary UB space for data fractal transformation. Developers do not need to allocate it, but must reserve the temporary space. The space size is calculated as follows (in bytes):

$$
((dValue \times sizeof(T) / 32 - 1) \times dstNzC0Stride + (nValue - 1) \times dstNzNStride + 1) \times 32
$$

- This API supports only **ndNum** set to 1.
- The source address in the UB must be 32-byte aligned, and the destination address in the L1 Buffer must be 32-byte aligned.
- The **ndNum**, **nValue**, and **dValue** parameters of the **Nd2NzParams** structure cannot be set to 0.
- Data copied to the L1 Buffer must not overlap. If overlapping writes occur, the hardware does not generate any warning or error, and the write order of the overlapping data is not guaranteed.

<!-- npu="950" id13 -->
- For the Ascend 950PR&950DT products, the 1:2 hard channel mode from UB to L1 Buffer is supported. A 1:1 compatibility mode is also provided to resolve code migration compatibility issues for A2/A3. This mode relays data through Global Memory and is slightly less efficient.
<!-- end id13 -->

- The values of the **Nd2NzParams** structure parameters must be within their value ranges, as shown in [Table 4](#table_ub_nd2nz_range).

**Table 4** Value range of the Nd2NzParams structure parameters<a id="table_ub_nd2nz_range"></a>

|Parameter|Value Range|
|----------|----------|
|ndNum|[1]|
|nValue|[1, 16384]|
|dValue|[1, 65535]|
|srcNdMatrixStride|[0, 65535]|
|srcDValue|[1, 65535]|
|dstNzC0Stride|[1, 16384]|
|dstNzNStride|[1, 16384]|
|dstNzMatrixStride|[0, 16384]|

## Example<a id="section10309141400"></a>

The example scenario is shown in [Figure 1](#fig128961542184620), and the example code snippet is as follows:

```cpp
AscendC::Nd2NzParams nd2nzParams;
// Transfer 2 ND matrices.
nd2nzParams.ndNum = 2;
// Each ND matrix has a height of 2 rows.
nd2nzParams.nValue = 2;
// Each row is 24 half wide; any portion that is not 32B-aligned is padded with zeros at the destination.
nd2nzParams.dValue = 24;
// The start address interval between adjacent ND matrices on the source side is 9 DataBlocks, that is, 9 * 16 = 144 half-type data elements.
nd2nzParams.srcNdMatrixStride = 144;
// The start address interval between adjacent rows of the same ND matrix on the source side is 3 DataBlocks, that is, 3 * 16 = 48 half-type data elements.
nd2nzParams.srcDValue = 48;
// In the destination NZ, the interval between adjacent C0 blocks split from the same source row is 11 DataBlocks.
nd2nzParams.dstNzC0Stride = 11;
// In the destination NZ, the start address interval after conversion of adjacent source rows is 2 DataBlocks.
nd2nzParams.dstNzNStride = 2;
// The start address interval between adjacent NZ matrices on the destination side is 6 DataBlocks, that is, 6 * 16 = 96 half-type data elements.
nd2nzParams.dstNzMatrixStride = 96;

AscendC::DataCopy(dstLocal, srcLocal, nd2nzParams);
```
