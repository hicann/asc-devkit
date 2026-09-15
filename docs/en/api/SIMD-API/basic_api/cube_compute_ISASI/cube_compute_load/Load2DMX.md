# LoadData (MX Cube Transfer)<a id="ZH-CN_TOPIC_0000002517258774"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T16:28:10.591Z -->

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

It is responsible for transferring the left and right cube data required for cube computation and the corresponding left and right quantization coefficient cube data. The left and right cube data is transferred in units of 512-byte data fractals, and the left and right quantization coefficient matrices are transferred in units of 32-byte data fractals. The supported data paths are as follows:

- Left and right matrices: L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer.
- Quantization coefficient matrices: L1 Buffer -> L0A_MX Buffer, L1 Buffer -> L0B_MX Buffer.

The L0A_MX/L0B_MX Buffer is 4 KB in size, and its address mapping relationship with the L0A/L0B Buffer address is as follows:

$$
L0A\_MX\ Buffer\ Address = L0A\ Buffer\ Address / 16
$$

$$
L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16
$$

- For the b4 data type, the left and right cube data fractal is a 16×64 cube in the L0A Buffer and a 64×16 cube in the L0B Buffer.
- For the b8 data type, the left and right cube data fractal is a 16×32 cube in the L0A Buffer and a 32×16 cube in the L0B Buffer.
- The quantization coefficient cube fractal is a cube with a fixed data type of fp8_e8m0_t and a fractal size of 16×2.

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
```


## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description<a name="table07381635103112"></a>

| Parameter | Description |
| ---------- | ---------- |
| T | **T** indicates the data type of **src**.<br>Supported data types: fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t. |
| U | **U** indicates the data type of **dst**.<br>Supported data types: fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t. |

**Table 2** General parameter description<a name="table18368155193919"></a>

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **LocalTensor**.<br>The contiguous data arrangement is determined by the **TPosition** of the destination operand, with the following constraints:<br>&nbsp;&nbsp;&bull; A2: ZZ format/NZ format; the corresponding fractal size is 16 \* (32 bytes / sizeof(T)).<br>&nbsp;&nbsp;&bull; B2: ZN format; the corresponding fractal size is (32 bytes / sizeof(T)) \* 16. <br>The supported physical storage locations are L0A Buffer (**TPosition**: A2)/L0B Buffer (**TPosition**: B2).<br>**Note: The addresses of L0A_MX Buffer and L0B_MX Buffer have a fixed proportional relationship with those of L0A/L0B. The API derives them from the L0A/L0B addresses automatically, so no user configuration is required.** |
| src | Input | Source operand, of type **LocalTensor**.<br>The data type must be consistent with that of **dst**.  <br>The supported physical storage location is L1 Buffer (**TPosition**: A1/B1). |
| srcMx | Input | Source operand, of type **LocalTensor**, supporting only the fp8_e8m0_t type. |
| loadDataParams | Input | LoadData parameter structure, of type **LoadData2DParamsV2**. For details, see the parameter description of the **LoadData2DParamsV2** structure in [LoadData (2D cube transfer V2)](Load2DV2.md). <br>**This structure is used to control the transfer of left and right cube data.**  <br>|
| loadMxDataParams | Input | LoadData parameter structure, of type **LoadData2DMxParams**. For details, see [Table 3](#table15901153712305).<br>**This structure is used to control the transfer of the left and right quantization coefficient matrices.**  <br>For the definitions of the above structure parameters, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_mm.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |

**Table 3** Parameter description of the LoadData2DMxParams structure<a name="table15901153712305"></a>

| Parameter | Description |
| ---------- | ---------- |
| xStartPosition | Start position of the source cube along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32-byte fractal). |
| yStartPosition | Start position of the source cube along the Y-axis, that is, the K dimension direction, in units of 32 bytes. |
| xStep | Transfer length of the source cube along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32-byte fractal). Value range: xStep∈[0, 255].<br>**Note: xStep=0 means no transfer is performed, and the API is treated as a NOP (no operation).** |
| yStep | Transfer length of the source cube along the Y-axis, that is, the K dimension direction, in units of 32 bytes. Value range: yStep∈[0, 255].<br>**Note: yStep=0 means no transfer is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Interval between the start address of the previous fractal and that of the next fractal along the X direction of the source cube, in units of 32 bytes. |
| dstStride | Interval between the start address of the previous fractal and that of the next fractal along the X direction of the destination cube, in units of 32 bytes. |

The following uses a specific example to explain the parameters of the **LoadData2DMxParams** structure. Assume that the shape of cube A is (M, K), then the shape of the ScaleA cube is (M, K/32), the data type of ScaleA is fp8_e8m0_t, and the fractal layout of the ScaleA cube is shown in [Figure 1](#fig138710913432).

**Figure 1** Fractal layout of ScaleA in L0A Buffer<a id="fig138710913432"></a>

![](../../../../figures/scale_a_l0a_fractal_layout.png "Fractal layout of ScaleA in L0A Buffer")

The following figure shows the configuration parameters during the transfer of ScaleA from the L1 Buffer to the L0A Buffer. Each row is 32 bytes, corresponding to one fractal in [Figure 1](#fig138710913432). **xStep** is the number of fractals in the M dimension, for example, xStep = M / 16 = 3 in the figure. **yStep** is the number of 32-byte units in the K dimension, for example, yStep = K / 32 / 2 = 21 in the figure. **srcStride** and **dstStride** are similar, indicating the number of 32-byte units in the K dimension.

**Figure 2** Configuration parameters for transferring ScaleA from the L1 Buffer to the L0A Buffer

![](../../../../figures/nd2nz_diagram.png "Configuration parameters for transferring ScaleA from the L1 Buffer to the L0A Buffer")

## Data Type

Supported data types: fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, fp8_e4m3fn_t.

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Key Features

The key features related to left and right cube transfer are consistent with those of Load2DV2. For details, see [Key Features of Load2DV2](Load2DV2.md).

The transfer features of the quantization coefficient cube are described as follows. The start address is calculated using the following formula:

$$
startAddr = srcAddr + (xStartPosition \times \lvert srcStride \rvert + yStartPosition) \times 32B
$$

The quantization coefficient cube fractal is a cube with a fixed data type of fp8_e8m0_t and a fractal size of 16×2. The transfer diagram is as follows:

**Figure 3** Quantization coefficient cube transfer diagram

![](../../../../figures/load2dmx_scale_transfer_demo.png "Quantization coefficient cube transfer diagram")

## Example

For the complete example, see [Load2DMX sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_2dmx_l12l0).
