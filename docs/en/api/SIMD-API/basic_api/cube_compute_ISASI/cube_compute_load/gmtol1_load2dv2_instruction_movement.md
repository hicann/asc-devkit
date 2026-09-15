# LoadData (GMToL1-2D Cube Load V2)<a id="ZH-CN_TOPIC_0000002594327301"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:23:47.526Z -->

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
## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It loads 2D-format data required for ordinary cube computation from Global Memory to the L1 Buffer in units of 512-byte data fractals (`TPosition` is A1/B1).

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Meaning |
| ---------- | ---------- |
| T | Data type of the source operand and the destination operand. |

**Table 2** General parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is **LocalTensor**. The supported physical storage location is L1 Buffer (**TPosition** is A1/B1).<br>There is no format requirement for data fractal; generally, it is in NZ format. In NZ format, the corresponding fractal size is 16 \* (32 bytes / sizeof(T)). |
| src | Input | Source operand, whose type is **GlobalTensor**. The data type must be consistent with **dst**. |
| loadDataParams | Input | LoadData parameter structure, whose type is **LoadData2DParamsV2**. For details, see [Table 3](#table49630346128). |

**Table 3** Description of parameters in the LoadData2DParamsV2 structure<a id="table49630346128"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| mStartPosition | Taking an M*K cube as an example, the start position of the source cube along the M axis direction, in units of 16 elements. |
| kStartPosition | Taking an M*K cube as an example, the start position of the source cube along the K axis direction, in units of 32 bytes. |
| mStep | Taking an M*K cube as an example, the load length of the source cube along the M axis direction, in units of 16 elements. Value range: mStep∈[0, 255].<br>**Note: mStep=0 means no load is performed, and the API is treated as a NOP (no operation).** |
| kStep | Taking an M*K cube as an example, the load length of the source cube along the K axis direction, in units of 32 bytes. Value range: kStep∈[0, 255].<br>**Note: kStep=0 means no load is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Taking an M*K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the source cube, in units of 512 bytes. |
| dstStride | Taking an M*K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination cube, in units of 512 bytes. |
| ifTranspose | Whether to enable the transpose function to transpose each fractal cube, defaulting to **false**.<br>&nbsp;&nbsp;&bull; **true**: Enabled.<br>&nbsp;&nbsp;&bull; **false**: Disabled.<br>Note: In this path scenario (GM->L1 Buffer), transpose is not supported, and this parameter is meaningless. Keep the default value. |
| sid | Reserved parameter. Set it to 0. |

Taking the **half** data type with transpose disabled as an example, the description of the **LoadData2DParamsV2** structure parameter value settings and the corresponding schematic diagram are as follows:

- **mStartPosition** = 2, the parameter unit is 16 elements, 2*16=32, therefore represents that the start position of the source cube along the M axis direction is the 32nd element.
- **kStartPosition** = 2, the parameter unit is 32 bytes, the corresponding element count is 2*32/2=32, therefore represents that the start position of the source cube along the K axis direction is the 32nd element.
- **mStep** = 2, with a parameter unit of 16 elements. 2*16=32, which therefore represents a load length of 32 elements along the M-axis direction of the source cube.
- **kStep** = 3, with a parameter unit of 32 bytes. The corresponding element count is 3*32/2=48, which therefore represents a load length of 48 elements along the K-axis direction of the source cube.
- **srcStride** = 5, with a parameter unit of 512 bytes. The corresponding element count is 5*512/2=1280, which therefore represents an interval of 1280 elements between the start address of the previous fractal and the start address of the next fractal along the K direction of the source cube.
- **dstStride** = 3, with a parameter unit of 512 bytes. The corresponding element count is 3*512/2=768, which therefore represents an interval of 768 elements between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination cube.
- **ifTranspose** = false, indicating that transpose is disabled.
- **sid** = 0, a reserved parameter. Configure it to 0.

**Figure 1** Example of LoadData2DParamsV2 structure parameters (taking the half data type and disabled transpose as an example)<a id="fig13901164574218"></a>

![Example of LoadData2DParamsV2 structure parameters (taking the half data type and disabled transpose as an example)](../../../../figures/load_data_2d_params_v2_half.png)

## Data Type<a id="section4219135304818"></a>

Supported data types: uint8_t, int8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint32_t, int32_t, float.

## Return Value

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- Transpose is not supported in this path scenario.

## Example<a id="section6461234123118"></a>

Take the scenario shown in [Figure 1](#fig13901164574218) as an example. The source cube is half data arranged in Nz format on GM. Starting from the second fractal on the M axis and the second 32B block on the K axis, load two M-direction fractals and three K-direction 32B blocks to the L1 Buffer.

```cpp
constexpr uint32_t fractalElemCount = 256;
constexpr uint32_t srcElemCount = 25 * fractalElemCount;
constexpr uint32_t dstElemCount = 9 * fractalElemCount;

// Source operand: half data stored on GM in Nz fractal layout. One 512B fractal contains 256 half elements.
// The GM shape is 80 * 80, with five small fractals in each of the M and K directions. Therefore, reserve 5 * 5 = 25 512B fractals.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, srcElemCount);

// Destination operand: L1 Buffer. The destination cube consists of 3 * 3 small fractals. Therefore, reserve nine 512B fractals.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::LoadData2DParamsV2 loadDataParams;
// The M-axis start position of the source cube is the 32nd element, that is, 2 * 16 elements.
loadDataParams.mStartPosition = 2;
// The K-axis start position of the source cube is the 32nd half, that is, 2 * 32B.
loadDataParams.kStartPosition = 2;
// Load 32 elements in the M direction, that is, 2 * 16 elements.
loadDataParams.mStep = 2;
// Load 48 half elements in the K direction, that is, 3 * 32B.
loadDataParams.kStep = 3;
// The GM source cube has five small fractals in the M direction.
loadDataParams.srcStride = 5;
// The destination A1 cube has 3 small fractals in the M direction.
loadDataParams.dstStride = 3;
// Disable transpose.
loadDataParams.ifTranspose = false;
// Reserved parameter, fixed to 0.
loadDataParams.sid = 0;

AscendC::LoadData(dstLocal, srcGm, loadDataParams);
```
