# DataCopy (GMToL1 In-Path Conversion - ND2NZ Copy)<a id="ZH-CN_TOPIC_0000002569070913"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:08:28.621Z -->

## Applicable Products<a id="zh-cn_topic_0000002566538879_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a id="zh-cn_topic_0000002566538879_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API mainly copies a cube from Global Memory to the L1 Buffer (with **TPosition** being **A1**/**B1**), and supports ND-to-NZ format conversion during data copy.

The ND-to-NZ format conversion is equivalent to the DN-to-ZN format conversion, as shown in [Figure 1](#zh-cn_topic_0000002566538879_fig444462417355).

**Figure 1** ND2NZ and DN2ZN conversion diagram<a id="zh-cn_topic_0000002566538879_fig444462417355"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_copy.png)

## Prototype<a id="zh-cn_topic_0000002566538879_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```

<!-- npu="950" id10 -->
For Ascend 950PR/Ascend 950DT in particular, please refer to the following prototype:

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
```
<!-- end id10 -->

## Parameters<a id="zh-cn_topic_0000002566538879_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. For supported data types, please refer to [Data Types](#zh-cn_topic_0000002566538879_section4219135304818). |
| enableSmallC0 | SmallC0 mode switch: When **dValue** is less than or equal to 4, **C0_SIZE** is padded to 4 * sizeof(T) bytes.<br>Disabled by default. Different models have different degrees of support for **enableSmallC0**. Please refer to [enableSmallC0 Parameter Support Degree Description](#enablesmallc0-parameter-support-description). |

### **enableSmallC0** Parameter Support Description

<!-- npu="950" id11 -->
- For Ascend 950PR/Ascend 950DT, the **enableSmallC0** parameter is newly added, and this parameter must be configured when calling this API;
<!-- end id11 -->

- Other models do not support this parameter.

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, type is **LocalTensor**, storage location is L1 Buffer (**TPosition** is A1/B1), and the destination address requires 32-byte alignment. |
| src | Input | Source operand, type is **GlobalTensor**, storage location is Global Memory, and the source address requires 1-byte alignment. |
| intriParams | Input | ND2NZ copy parameter, type is **Nd2NzParams**.<br>For the parameter description of **Nd2NzParams**, please refer to [Table 4](#zh-cn_topic_0000002566538879_table144203616291). |

**Table 3** **Nd2NzParams** Structure Parameter Definition<a id="zh-cn_topic_0000002566538879_table144203616291"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| ndNum | Number of ND matrices in the source cube. Value range: **ndNum**∈[0, 4095].<br>**Note:** **ndNum** = 0 indicates not executing the copy, and this API will be considered as **NOP** (no operation). |
| nValue | Number of rows of the ND cube in the source cube. Value range: **nValue**∈[0, 16384].<br>**Note:** **nValue** = 0 indicates not executing the copy, and this API will be considered as **NOP** (no operation). |
| dValue | Number of columns of the ND cube in the source cube. Value range: **dValue**∈[0, 65535]. When **dValue** * sizeof(T) does not meet 32-byte alignment, zeros will be padded in the destination cube to align to 32 bytes.<br>**Note:** **dValue** = 0 indicates not executing the copy, and this API will be considered as **NOP** (no operation). |
| srcNdCubeStride | Offset between the start addresses of adjacent ND matrices in the source cube. Value range: **srcNdCubeStride**∈[0, 65535]. Unit: number of elements.<br>&nbsp;&nbsp;&bull; When **ndNum** = 1, **srcNdCubeStride** is meaningless; set it to 0.<br>&nbsp;&nbsp;&bull; When **ndNum** ≠ 1 and **srcNdCubeStride** = 0, it indicates that the first ND cube in the source cube is copied repeatedly. |
| srcDValue | Number of elements contained in one row of the source cube. Value range: **srcDValue**∈[1, 65535]. Unit: number of elements. |
| dstNzC0Stride | After ND-to-NZ conversion, offset between the start addresses of adjacent Z matrices in the destination NZ cube. Value range: **dstNzC0Stride**∈[1, 16384]. Unit: **C0_SIZE** (32 bytes). |
| dstNzNStride | Offset between the start addresses of adjacent rows in the NZ cube in the destination cube. Value range: **dstNzNStride**∈[1, 16384]. Unit: **C0_SIZE** (32 bytes). |
| dstNzCubeStride | Offset between the start addresses of adjacent NZ matrices in the destination cube. Value range: **dstNzCubeStride**∈[0, 16384]. Unit: number of elements.<br>&nbsp;&nbsp;&bull; When **ndNum** = 1, **dstNzCubeStride** is meaningless; set it to 0.<br>&nbsp;&nbsp;&bull; When **ndNum** ≠ 1 and **dstNzCubeStride** = 0, it indicates that each NZ cube copied into the destination cube overwrites the first NZ cube. |

## Data Types<a id="zh-cn_topic_0000002566538879_section4219135304818"></a>

The data types supported by the source cube and the destination cube are consistent.

<!-- npu="950" id12 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32.
<!-- end id12 -->

<!-- npu="A3" id13 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: b4 (int4b_t), int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, bfloat16_t, float.
<!-- end id13 -->

<!-- npu="910b" id14 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: b4 (int4b_t), int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, bfloat16_t, float.
<!-- end id14 -->

<!-- npu="310p" id15 -->
For Atlas inference products AI Core, the supported data types are: int16_t, uint16_t, int32_t, uint32_t, half, float.
<!-- end id15 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002566538879_section2045914466492"></a>

- When the input data type is **b4**, data is processed based on the **b8** type during ND2NZ. Therefore, the parameters must be set based on the **b8** type.
- The source address in **Global Memory** must be 1-byte aligned, and the destination address in **L1 Buffer** must be 32-byte aligned.
- When any of the **Nd2NzParams** structure parameters **ndNum**, **nValue**, or **dValue** is 0, the instruction is not executed, and this API is considered as **NOP** (no operation).
- Data copied to **L1 Buffer** must not overlap. If overlapping writes exist, the hardware does not generate any warning or error, and the write order of overlapping data is not guaranteed.
- The values of the **Nd2NzParams** structure parameters must be within the value range:

    **Table 4** Value range of the **Nd2NzParams** structure parameters

    | Parameter | Value Range |
    | ---------- | ---------- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdCubeStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzCubeStride | [0, 16384] |

<!-- npu="310p" id18 -->
- For the AI Core of Atlas inference products, when using the ND2NZ copy API over the **Global Memory** -> **Local Memory** path, 8K of UB space must be reserved as the temporary data storage area for the API.
<!-- end id18 -->

## Key Features

### Continuous Copy<a id="zh-cn_topic_0000002566538879_section7495192151713"></a>

Taking the half data type as an example, [Figure 2](#zh-cn_topic_0000002566538879_fig7420114233419) shows the ND2NZ continuous copy diagram. The **Nd2NzParams** structure parameter configuration is described as follows:

- **ndNum** = 1, indicating that the number of ND matrices to be copied is 1.
- **nValue** = 16, the number of rows of an ND cube.
- **dValue** = 23, the number of columns of an ND cube. When dValue * sizeof(T) does not meet 32-byte alignment, zeros will be padded in the destination cube to align to 32 bytes.
- **srcNdCubeStride** = 0. Since there is only one ND cube, this value is 0.
- **srcDValue** = 32, indicating the number of elements in a row of the source cube, that is, 32 elements of the half data type.
- **dstNzC0Stride** = 16, the offset of the start address of adjacent Z matrices in the NZ cube, where the offset is 16 C0_SIZE.
- **dstNzNStride** = 1, indicating the offset of the start address of adjacent rows in the NZ cube.
- **dstNzCubeStride** = 0. Since there is only one NZ cube, this value is 0.

**Figure 2** ND2NZ conversion diagram (continuous copy)<a id="zh-cn_topic_0000002566538879_fig7420114233419"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_continuous_copy.png)

### Non-continuous Copy<a id="zh-cn_topic_0000002566538879_section1721231581717"></a>

Taking the half data type as an example, [Figure 3](#zh-cn_topic_0000002566538879_fig19978184883316) shows the ND2NZ non-continuous copy diagram. The **Nd2NzParams** structure parameter configuration is described as follows:

- **ndNum** = 2, indicating that the number of ND matrices to be copied is 2.
- **nValue** = 8, the number of rows of an ND cube.
- **dValue** = 23, the number of columns of an ND cube. When dValue * sizeof(T) does not meet 32-byte alignment, zeros will be padded in the destination cube to align to 32 bytes.
- **srcNdCubeStride** = 384, indicating the address offset of the start of adjacent ND matrices. Each row has 32 elements and there are 12 rows in total, so the offset is 32 * 12 = 384 elements.
- **srcDValue** = 32, indicating the number of elements in one row of the source cube, that is, 32 elements of the half data type.
- **dstNzC0Stride** = 37, the address offset of the start of adjacent Z matrices in the NZ cube, with an offset of 37 C0_SIZE.
- **dstNzNStride** = 2, indicating the address offset of the start of adjacent rows in the NZ cube.
- **dstNzCubeStride** = 320, the address offset of the start of adjacent NZ matrices. Each row has 16 elements and there are 20 rows in total, so the offset is 16 * 20 = 320 elements.

**Figure 3** ND2NZ conversion diagram (non-continuous copy)<a id="zh-cn_topic_0000002566538879_fig19978184883316"></a>

![](../../../../figures/datacopy_gm2l1_nd2nz_noncontinuous_copy.png)

### SmallC0 Mode

The following figure shows the ND2NZ conversion diagram in **enableSmallC0** enable mode:

**Figure 4** ND2NZ conversion diagram in **enableSmallC0** enable mode (half data type)<a name="fig07641913195410"></a>  

![](../../../../figures/small_c0_nd2nz_half.png "ND2NZ conversion diagram in enableSmallC0 enable mode (half data type)")

## Example<a id="zh-cn_topic_0000002566538879_section088124295117"></a>

In the following example, **DataCopy** is used to perform on-the-fly ND2NZ data copy in the scenario where cube A is not transposed and the data type is half.

The following figure shows the data layout changes during the copy process:

![](../../../../figures/datacopy_gm2l1_nd2nz_demo.png)

The sample code snippet is as follows. Only part of the code in the sample is shown. For the complete sample, see [DataCopy_GM2L1 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
// m=40,k=56,fractalShape[0] = 16,fractalShape[1] = 16,fractalSize = 16 * fractalShape[1]
AscendC::Nd2NzParams nd2nzA1Params;
// Number of ND matrices to be transferred.
nd2nzA1Params.ndNum = 1;
// Rows of the ND cube.
nd2nzA1Params.nValue = m;
// Columns of the ND cube.
nd2nzA1Params.dValue = k;
// Only one ND cube is transferred, so this parameter is invalid.
nd2nzA1Params.srcNdCubeStride = 0;
// Number of elements in one row of the source cube.
nd2nzA1Params.srcDValue = k;
// The following parameter takes the aligned length of cube A in the height direction on the L1 buffer.
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzCubeStride = 0;
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```
