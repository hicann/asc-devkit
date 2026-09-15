# LoadData (2D Cube Transfer V2)

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T16:33:04.264Z -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: Not supported
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
## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It transfers 2D-format data required for ordinary cube computation, operating in units of 512-byte data fractals, and supports the following data paths:

- L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer.

For different data types, the cube corresponding to each data fractal is as follows:

- For the b4 data type, each data fractal is a 16×64 cube in the L0A Buffer and a 64×16 cube in the L0B Buffer.
- For the b8 data type, each data fractal is a 16×32 cube in the L0A Buffer and a 32×16 cube in the L0B Buffer.
- For the b16 data type, each data fractal is a 16×16 cube.
- For the b32 data type, each data fractal is a 16×8 cube in the L0A Buffer and an 8×16 cube in the L0B Buffer.

## Function Prototype

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Meaning |
| ---------- | ------ |
| T | Data type of the source operand and destination operand. |

**Table 2** Common parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ----------- | ------ |
| dst | Output | Destination operand, of type **LocalTensor**.<br>For fractal constraints, see [cube computation input transfer constraints](cube computation input transfer constraints.md).<br>For start address alignment constraints, see [alignment constraints](cube computation input transfer constraints.md).<br>The data type must be consistent with that of **src**.<br>Supported physical storage locations are L0A Buffer (TPosition: A2)/L0B Buffer (TPosition: B2). |
| src | Input | Source operand, of type **LocalTensor**.<br>For fractal constraints, see [cube computation input transfer constraints](cube computation input transfer constraints.md).<br>For start address alignment constraints, see [alignment constraints](cube computation input transfer constraints.md).<br>The data type must be consistent with that of **dst**.<br>Supported physical storage location is L1 Buffer (TPosition: A1/B1). |
| loadDataParams | Input | **LoadData** parameter structure, of type **LoadData2DParamsV2**. For details, see [Table 3](#table_load2dv2_params). |

**Table 3** Parameter description of the LoadData2DParamsV2 structure<a id="table_load2dv2_params"></a>

| Parameter | Meaning |
| ---------- | ------ |
| mStartPosition | Taking an M×K cube as an example, the start position of the source cube along the M axis, in units of 16 elements. |
| kStartPosition | Taking an M×K cube as an example, the start position of the source cube along the K axis, in units of 32 bytes. |
| mStep | Taking an M×K cube as an example, the transfer length of the source cube along the M axis, in units of 16 elements. Value range: mStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, **mStep** must satisfy the following additional constraints in addition to the value range:<br>&nbsp;&nbsp;&bull;When the data type is b4, **mStep** must be a multiple of 4.<br>&nbsp;&nbsp;&bull;When the data type is b8, **mStep** must be a multiple of 2.<br>&nbsp;&nbsp;&bull;When the data type is b16, **mStep** must be a multiple of 1.<br>&nbsp;&nbsp;&bull;When the data type is b32, **mStep** has no additional constraint.<br>**Note: mStep=0 means no transfer is performed, and the API is treated as a NOP (no operation).** |
| kStep | Taking an M×K cube as an example, the transfer length of the source cube along the K axis, in units of 32 bytes. Value range: kStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, **kStep** must satisfy the following additional constraints in addition to the value range:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, **kStep** has no additional constraint.<br>&nbsp;&nbsp;&bull;When the data type is b32, **kStep** must be a multiple of 2.<br>**Note: kStep=0 means no transfer is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Taking an M×K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the source cube, in units of 512 bytes. |
| dstStride | Taking an M×K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination cube, in units of 512 bytes. |
| ifTranspose | Whether to enable the transpose function to transpose each fractal cube, defaulting to **false**:<br>&nbsp;&nbsp;&bull; **true**: Enabled.<br>&nbsp;&nbsp;&bull; **false**: Disabled.<br>Note: Transpose can be enabled only on the L1 Buffer (TPosition: A1) -> L0A Buffer (TPosition: A2) and L1 Buffer (TPosition: B1) -> L0B Buffer (TPosition: B2) paths. When the transpose function is enabled, the b4, b8, b16, and b32 data types are supported. |
| sid | Reserved parameter. Set it to 0.<br>Note: This parameter is retained for compatibility with APIs of earlier products. Ascend 950PR/Ascend 950DT products do not process it. |

Taking the half data type without enabling transpose as an example, the parameter value settings of the LoadData2DParamsV2 structure and the corresponding diagram are as follows:

- **mStartPosition** = 2, with the parameter unit being 16 elements. 2*16=32, so it indicates that the start position of the source cube along the M axis is the 32nd element.
- **kStartPosition** = 2, with the parameter unit being 32 bytes. The corresponding number of elements is 2*32/2=32, so it indicates that the start position of the source cube along the K axis is the 32nd element.
- **mStep** = 2, with a parameter unit of 16 elements. 2*16=32, so the move length along the M-axis of the source cube is 32 elements.
- **kStep** = 3, with a parameter unit of 32 bytes, corresponding to 3*32/2=48 elements, so the move length along the K-axis of the source cube is 48 elements.
- **srcStride** = 5, with a parameter unit of 512 bytes, corresponding to 5*512/2=1280 elements, so the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the source cube is 1280 elements.
- **dstStride** = 3, with a parameter unit of 512 bytes, corresponding to 3*512/2=768 elements, so the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination cube is 768 elements.
- **ifTranspose** = false, indicating that transpose is not enabled.
- **sid** = 0, a reserved parameter. Set it to 0.

**Figure 1** Example of the LoadData2DParamsV2 structure parameters (using the half data type without enabling transpose as an example)

![](../../../../figures/load_data_2d_params_v2_half.png)

## Data Type

Supported data types: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value

None

## Constraints

- When **mStep** and **kStep** are both 0, no transfer is performed, and the API is treated as a NOP (no operation).
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned; when the source address or destination address is in the L1 Buffer, the address must be 32-byte aligned.
- For cube transpose operations, different data types require different **mStep** and **kStep** constraints: for the b4 data type, **mStep** must be a multiple of 4; for the b8 data type, **mStep** must be a multiple of 2; for the b16 data type, **mStep** must be a multiple of 1; for the b32 data type, **kStep** must be a multiple of 2.

## Key Features

### Start Position Calculation for Non-transpose Transfer

Use **mStep** and **kStep** to transfer data blocks in multiple directions. Take the [M, K] NZ fractal as an example:

- **mStartPosition** is 2, indicating that the transfer start position in the M direction is 16 × 2 = 32 elements away from the start position of the source operand along the M axis.
- **kStartPosition** is 2, indicating that the transfer start position in the K direction is 32B × 2 = 64B away from the start position of the source operand along the K axis.
- **srcStride** is 5, indicating that in the source operand, the interval between the start address of the previous fractal and that of the next fractal in the K direction is 5.
- **dstStride** is 3, indicating that in the destination operand, the interval between the start address of the previous fractal and that of the next fractal in the K direction is 3.

The start address is calculated as follows:

$$
startAddr = srcAddr + (kStartPosition \times \lvert srcStride \rvert + mStartPosition) \times 512B
$$

Therefore, the final transfer start position is the thirteenth fractal cube (the fractal cube at the start address of the source operand is counted as the first one).

**Figure 2** Transfer start position diagram

![](../../../../figures/load2dv2_start_addr_demo.png "transfer start position diagram")

### Transpose Scenario Loading

For cube transpose operations on b4/b8/b16/b32, the constraints on **mStep** and **kStep** must be satisfied:

- For the b4 data type, **mStep** must be a multiple of 4.
- For the b8 data type, **mStep** must be a multiple of 2.
- For the b16 data type, **mStep** must be a multiple of 1.
- For the b32 data type, **kStep** must be a multiple of 2.

The transpose diagrams for each data type are as follows:

**Figure 3** Transpose diagram for the b4 data type

![](../../../../figures/load2dv2_l12l0_trans_b4.png "Transpose diagram for the b4 data type")

**Figure 4** Transpose diagram for the b8 data type

![](../../../../figures/load2dv2_l12l0_trans_b8.png "b8 data type transpose diagram")

**Figure 5** b16 data type transpose diagram

![](../../../../figures/load2dv2_l12l0_trans_b16.png "b16 data type transpose diagram")

**Figure 6** b32 data type transpose diagram

![](../../../../figures/load2dv2_l12l0_trans_b32.png "b32 data type transpose diagram")

## Example

The following code snippet shows only part of the sample code. For the complete usage sample, see [Load2DV2 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_2dv2_l12l0).

```cpp
// Load2DV2: Nz -> Nz
uint32_t m = 40;
uint32_t k = 70;
uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t fractalSize = fractalShape[0] * fractalShape[1];
uint16_t mStep = (m + fractalShape[0] - 1) / fractalShape[0];

AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = mStep;
loadDataParams.kStep = (k + fractalShape[1] - 1) / fractalShape[1];
loadDataParams.srcStride = mStep;
loadDataParams.dstStride = mStep;
loadDataParams.ifTranspose = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
