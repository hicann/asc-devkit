# LoadData (MX Matrix Copy) <a id="ZH-CN_TOPIC_0000002517258774"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:49:31.661Z pushedAt=2026-10-26T07:41:44.086Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It transfers the left and right matrix data and the corresponding left and right quantization coefficient matrix data required for matrix computation. The left and right matrix data are transferred in units of 512-byte data fractals, and the left and right quantization coefficient matrices are transferred in units of 32-byte data fractals. The supported data paths are as follows:

- Left and right matrices: L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer.
- Quantization coefficient matrices: L1 Buffer -> L0A_MX Buffer, L1 Buffer -> L0B_MX Buffer.

The L0A_MX/L0B_MX Buffer is 4 KB in size, and its address mapping relationship with the L0A/L0B Buffer is as follows:

$$
L0A\_MX\ Buffer\ Address = L0A\ Buffer\ Address / 16
$$

$$
L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16
$$

- For the b4 data type, the left and right matrix data fractal is a 16×64 matrix in the L0A Buffer and a 64×16 matrix in the L0B Buffer.
- For the b8 data type, the left and right matrix data fractal is a 16×32 matrix in the L0A Buffer and a 32×16 matrix in the L0B Buffer.
- The quantization coefficient matrix fractal is a matrix with a fixed data type of fp8_e8m0_t and a fractal size of 16×2.

## Function Prototype<a id="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
```


## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description<a name="table07381635103112"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| T | T indicates the data type of **src**.<br>Supported data types: fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t. |
| U | U indicates the data type of **dst**.<br>Supported data types: fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t. |

**Table 2** General parameter description<a name="table18368155193919"></a>

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **LocalTensor**.<br>The continuous data arrangement order is determined by the **TPosition** of the destination operand. The specific constraints are as follows:<br>&nbsp;&nbsp;&bull; A2: ZZ format/NZ format; the corresponding fractal size is 16 \* (32 bytes / sizeof(T)).<br>&nbsp;&nbsp;&bull; B2: ZN format; the corresponding fractal size is (32 bytes / sizeof(T))  \* 16. <br>The supported physical storage locations are L0A Buffer (**TPosition**: A2)/L0B Buffer (**TPosition**: B2).<br>**Note: The addresses of L0A_MX Buffer and L0B_MX Buffer have a fixed proportional relationship with the addresses of L0A/L0B. The API derives them automatically based on the L0A/L0B addresses, so no user configuration is required.** |
| src | Input | Source operand, of type **LocalTensor**.<br>The data type must be consistent with **dst**.  <br>The supported physical storage location is L1 Buffer (**TPosition**: A1/B1). |
| srcMx | Input | Source operand, of type **LocalTensor**, supporting only the fp8_e8m0_t type. |
| loadDataParams | Input | LoadData parameter structure, of type **LoadData2DParamsV2**. For details, see the parameter description of the **LoadData2DParamsV2** structure in [LoadData (2D matrix copy V2)](LoadData_2D_V2.md). <br>**This structure is used to control the copy of the left and right matrix data.**  <br>|
| loadMxDataParams | Input | LoadData parameter structure, of type **LoadData2DMxParams**. For details, see [Table 3](#table15901153712305).<br>**This structure is used to control the copy of the left and right quantization coefficient matrices.**  <br>For the definitions of the above structure parameters, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** LoadData2DMxParams structure parameter description<a name="table15901153712305"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| xStartPosition | Start position of the source matrix along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32-byte fractal). |
| yStartPosition | Start position of the source matrix along the Y-axis, that is, the K dimension direction, in units of 32 bytes. |
| xStep | Copy length of the source matrix along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32-byte fractal). Value range: xStep∈[0, 255].<br>**Note: xStep=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| yStep | Copy length of the source matrix along the Y-axis, that is, the K dimension direction, in units of 32 bytes. Value range: yStep∈[0, 255].<br>**Note: yStep=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Interval between the start address of the previous fractal and the start address of the next fractal along the X direction of the source matrix, in units of 32 bytes. |
| dstStride | Interval between the start address of the previous fractal and the start address of the next fractal along the X direction of the destination matrix, in units of 32 bytes. |

The following uses a specific example to explain the parameters of the **LoadData2DMxParams** structure. Assume that the shape of matrix A is (M, K), then the shape of the ScaleA matrix is (M, K/32), the data type of ScaleA is fp8_e8m0_t, and the fractal layout of the ScaleA matrix is shown in [Figure 1](#fig138710913432).

**Figure 1** Fractal layout of ScaleA in L0A Buffer<a id="fig138710913432"></a>

![](../../../../figures/scalea_l0a_fractal.png "Fractal layout of ScaleA in L0A Buffer")

The following figure shows the configuration parameters during the transfer of ScaleA from the L1 Buffer to the L0A Buffer. Each row is 32 bytes, corresponding to one fractal in [Figure 1](#fig138710913432). xStep is the number of fractals in the M dimension, for example, xStep = M / 16 = 3 in the figure. yStep is the number of 32-byte units in the K dimension, for example, yStep = K / 32 / 2 = 21 in the figure. srcStride and dstStride follow the same rule, representing the number of 32-byte units in the K dimension.

**Figure 2**  Configuration parameters for transferring ScaleA from the L1 Buffer to the L0A Buffer

![](../../../../figures/LoadData_2D_MX_nd2nz.png "Configuration parameters for transferring ScaleA from the L1 Buffer to the L0A Buffer")

## Data Type

Supported data types: fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, fp8_e4m3fn_t.

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Key Features

The key features related to left and right matrix copy are the same as those of LoadData (2D matrix copy V2). For details, see [Key Features of LoadData (2D Matrix Copy V2)](LoadData_2D_V2.md).

The transfer features of the quantization coefficient matrix are described as follows. The start address is calculated as follows:

$$
startAddr = srcAddr + (xStartPosition \times \lvert srcStride \rvert + yStartPosition) \times 32B
$$

A quantization coefficient matrix fractal is a matrix with a fixed data type of fp8_e8m0_t and a fractal size of 16×2. The transfer diagram is as follows:

**Figure 3** Quantization coefficient matrix transfer diagram

![](../../../../figures/load2dmx_scale_transfer_demo.png "Quantization coefficient matrix transfer diagram")

## Example

For a complete example, see [LoadData (MX matrix copy) sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0).
