# DataCopy (GMToL1 Inline Conversion - DN2NZ Data Movement)<a id="ZH-CN_TOPIC_0000002563847574"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:59:45.453Z -->

## Applicable Products<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a id="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Inline format conversion data movement, which is suitable for performing DN-to-NZ format conversion during data movement. Data is moved from Global Memory to the L1 Buffer (with TPosition being A1/B1), and the DN-to-NZ format conversion is completed during the movement.

The DN-to-NZ format conversion is equivalent to the ND-to-ZN format conversion, as shown in the following figure:

**Figure 1** DN-to-NZ format conversion diagram

![](../../../../figures/gm2l1_dn2nz_concept.png "DN-to-NZ format conversion diagram")

## Function Prototype<a id="section1954364615315"></a>

```cpp
template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand or destination operand. |
| enableSmallC0 | SmallC0 mode switch: when **dValue** is less than or equal to 4, **C0_SIZE** is padded to 4 * sizeof(T) bytes. Disabled by default. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **LocalTensor**, stored in L1 Buffer (**TPosition** is A1/B1). |
| src | Input | Source operand, of type **GlobalTensor**.<br>The L2 Cache control modes supported by [SetL2CacheHint](../../data_structures/GlobalTensor/SetL2CacheHint.md) are as follows: **CacheMode::CACHE_MODE_DISABLE**, **CacheMode::CACHE_MODE_NORMAL**. |
| intriParams | Input | Transfer parameters, of type **Dn2NzParams**. For details about the parameters, see [Table 3](#table9182515919). |

**Table 3** Dn2NzParams structure parameter definition<a id="table9182515919"></a>

| Parameter | Description |
| ---------- | ---------- |
| dnNum | Number of DN matrices to transfer. Value range: dnNum∈[0, 4095].<br>**Note: dnNum=0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| nValue | Number of columns of the DN cube. Value range: nValue∈[0, 16384].<br>**Note: nValue=0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| dValue | Number of rows of the DN cube. Value range: dValue∈[0, 2^21-1].<br>**Note: dValue=0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| srcDnCubeStride | Offset between the start addresses of adjacent DN matrices in the source operand. Value range: srcDnCubeStride∈[0, 2^40-1], in elements.<br>&nbsp;&nbsp;&bull; When dnNum = 1, srcDnCubeStride is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When dnNum ≠ 1 and srcDnCubeStride = 0, it indicates that each DN cube transferred into the source cube is read from the first DN cube. |
| srcDValue | Offset between the start addresses of adjacent rows of the same DN cube in the source operand. Value range: srcDValue∈[1, 2^40-1], in elements. |
| dstNzC0Stride | After DN is converted to NZ format, one column in the source operand is converted to multiple rows in the destination operand. dstNzC0Stride indicates the offset between the start addresses of adjacent rows of the multiple rows of data from the same column of the source operand in the destination NZ cube. Value range: dstNzC0Stride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzNStride | Offset between the start addresses of adjacent rows of the Z-shaped cube in the destination NZ cube. Value range: dstNzNStride∈[1, 16384], unit: C0_SIZE (32 bytes). |
| dstNzCubeStride | Offset between the start addresses of adjacent NZ matrices in the destination NZ cube. Value range: dstNzCubeStride∈[0, 16384], in elements.<br>&nbsp;&nbsp;&bull; When dnNum = 1, dstNzCubeStride is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When dnNum ≠ 1 and dstNzCubeStride = 0, it indicates that each NZ cube transferred into the destination cube overwrites the first NZ cube. |

**Continuous transfer**

Taking the half data type as an example, the following figure shows the schematic diagram of DN2NZ continuous transfer. The parameter configuration of the Dn2NzParams structure is described as follows:

- dnNum = 1, indicating that the number of DN matrices to transfer is 1.
- **nValue** = 16, the number of columns of one DN cube.
- **dValue** = 23, the number of rows of one DN cube. When dValue \* sizeof\(T\) does not meet 32-byte alignment, zeros will be padded in the destination cube to align to 32 bytes.
- **srcDnCubeStride** = 0. There is only one DN cube, so this value is 0.
- **srcDValue** = 16, indicating the number of elements contained in one row of the source cube, that is, 16 elements of the half data type.
- **dstNzC0Stride** = 16, the address offset of the start address of adjacent Z matrices in the NZ cube. The offset is 16 C0\_SIZE, that is, 16 \* 32 bytes.
- **dstNzNStride** = 1, indicating the offset of the start address of adjacent rows in the NZ cube.
- **dstNzCubeStride** = 0. There is only one input DN cube, so this value is 0.

**Figure 2** DN2NZ conversion diagram (continuous transfer)

![](../../../../figures/gm2l1_dn2nz_continuous.png "DN2NZ conversion diagram (continuous transfer)")

**Non-continuous transfer**

Taking the half data type as an example, the following figure shows the schematic diagram of DN2NZ non-continuous transfer. The parameter configuration of the **Dn2NzParams** structure is described as follows:

- **dnNum** = 2, indicating that the number of DN matrices to be transferred is 2.
- **nValue** = 8, the number of columns of one DN cube.
- **dValue** = 23, the number of rows of one DN cube. When dValue \* sizeof\(T\) does not meet the 32-byte alignment, zeros will be padded in the destination cube to align to 32 bytes.
- **srcDnCubeStride** = 400, indicating the address offset between the start addresses of adjacent DN matrices. Each row has 16 elements and there are 25 rows in total, so the offset is 16 \* 25 = 400 elements.
- **srcDValue** = 16, indicating the number of elements contained in one row of the source cube, that is, 16 elements of the half data type.
- **dstNzC0Stride** = 37, the address offset between the start addresses of adjacent Z matrices in the NZ cube, with an offset of 37 C0\_SIZE.
- **dstNzNStride** = 2, indicating the address offset between the start addresses of adjacent rows in the NZ cube.
- **dstNzCubeStride** = 320, the address offset between the start addresses of adjacent NZ matrices. Each row has 16 elements and there are 20 rows in total, so the offset is 16 \* 20 = 320 elements.

**Figure 3** DN2NZ conversion diagram (non-continuous transfer)

![](../../../../figures/gm2l1_dn2nz.png "DN2NZ conversion diagram (non-continuous transfer)")

**SmallC0 Mode**

When **dValue** is less than or equal to 4, the SmallC0 mode can be enabled. **C0_SIZE** is aligned to 4 \* sizeof\(T\) bytes. When dValue \* sizeof\(T\) does not meet the 4 \* sizeof\(T\) alignment, zeros will be padded in the destination cube to align to 4 \* sizeof\(T\).

If the entire destination cube is not aligned to **C0_SIZE** (32 bytes), zeros are padded at the end of the destination cube until it is aligned to 32 bytes. In this mode, the **dstNzNStride** and **dstNzC0Stride** parameters do not need to be configured.

**Figure 4** DN2NZ conversion diagram (SmallC0 mode)

![](../../../../figures/gm2l1_dn2nz_smallc0.png "DN2NZ conversion diagram (SmallC0 mode)")

## Data Type<a id="section4219135304818"></a>

Supported data types: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, int16_t, uint16_t, int32_t, uint32_t, half, bfloat16_t, float.

## Return Value

None

## Constraints

- When the input data type is **b4**, data is processed based on the **b8** type during DN2NZ conversion. Therefore, parameters must be set according to the **b8** type, and **nValue** must be a multiple of 2.
- The source address in **Global Memory** must be 1-byte aligned, and the destination address in **L1 Buffer** must be 32-byte aligned.
- When any of the **Dn2NzParams** structure parameters **dnNum**, **nValue**, or **dValue** is 0, the instruction is not executed, and this API is considered as **NOP** (no operation).
- Data transferred to **L1 Buffer** must not overlap. If overlapping writes occur, the hardware does not generate any warning or error, and the write order of the overlapping data is not guaranteed.
- The values of the **Dn2NzParams** structure parameters must be within the value range. The value ranges of the parameters are shown in [Table 4](#table_dn2nz_range).

**Table 4**  Value ranges of **Dn2NzParams** structure parameters<a id="table_dn2nz_range"></a>

|Parameter|Value Range|
|----------|----------|
|dnNum|[0, 4095]|
|nValue|[0, 16384]|
|dValue|[0, 2^21-1]|
|srcDnCubeStride|[0, 2^40-1]|
|srcDValue|[1, 2^40-1]|
|dstNzC0Stride|[1, 16384]|
|dstNzNStride|[1, 16384]|
|dstNzCubeStride|[0, 16384]|

## Example<a id="section122101199486"></a>

In the following example, **DataCopy** is used to perform inline DN2NZ data transfer in the scenario where cube A is transposed and the data type is half.

The following figure shows the data layout changes during the transfer:

![](../../../../figures/datacopy_gm2l1_dn2nz_demo.png)

The sample code snippet is as follows. For the complete sample, see scenario 3 in the [data_copy_gm2l1 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1).

```cpp
// m=40,k=56,fractalShape[0] = 16,fractalShape[1] = 16,fractalSize = 16 * fractalShape[1] 
AscendC::Dn2NzParams dn2nzA1Params; 
// Number of DN matrices to transfer.
dn2nzA1Params.dnNum = 1; 
// Number of columns of the DN cube.
dn2nzA1Params.nValue = 40; 
// Number of rows of the DN cube.
dn2nzA1Params.dValue = 56; 
// Only one DN cube is transferred, so this parameter is invalid.
dn2nzA1Params.srcDnCubeStride = 0; 
// Offset between the start addresses of adjacent rows of the same DN cube in the source operand.
dn2nzA1Params.srcDValue = 40; 
dn2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]); 
dn2nzA1Params.dstNzNStride = 1; 
dn2nzA1Params.dstNzCubeStride = 0; 
AscendC::DataCopy(a1Local, aGM, dn2nzA1Params);

```
