# Basic API Migration Guide<a id="ZH-CN_TOPIC_0000002470508258"></a>

<!-- md-trans-meta sourceCommit=7ed7279ea7c037482ecde991f38c4310e3431914 translatedAt=2026-10-23T10:54:48.823Z pushedAt=2026-11-03T07:05:13.252Z nl=2 -->

This section describes the impact of chip changes in [NPU architecture version 3510](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) on basic API compatibility, and provides a compatibility solution for adapting basic APIs from architecture 2201 to architecture 3510.

**Table 1**  Basic API compatibility issue index

| Functional Classification | Compatibility Issue | Involved APIs | Migration Guide |
|---------|-----------|---------|---------|
| Vector computation | Subnormal floating-point numbers are treated as 0 during computation | Exp, Ln, Reciprocal, Sqrt, Rsqrt, Div | [Configuring the subnormal computation mode](#subnormal-migration) |
| Data movement | Direct movement from L1 Buffer to GM is not supported | DataCopy | [Adapting the L1 Buffer-to-GM movement path](#data-copy-l1-to-gm-migration) |
| Data movement | Setting the L1 Buffer boundary value is not supported | SetLoadDataBoundary, LoadData | [Manually splitting LoadData and implementing address wraparound](#set-load-data-boundary-migration) |
| Cube computation | The Cube compute unit does not support the int4b_t data type | LoadData, Mmad, LoadDataWithTranspose | [Converting int4b_t to int8_t before cube computation](#cube-int4-migration) |
| Cube computation | The L0A Buffer fractal changes from Zz to Nz | LoadData, Mmad, LoadDataWithTranspose | [Adapting L0A Buffer splitting by Nz fractal](#l0a-fractal-migration) |
| Cube computation | The 4:2 structured sparsity feature is not supported | LoadDataWithSparse, MmadWithSparse | [Changing sparse cube computation to dense cube computation](#structured-sparsity-migration) |
| Cube computation | Direct movement from GM to L0A Buffer or L0B Buffer is not supported | LoadData | [Relaying through L1 Buffer](#gm-to-l0a-l0b-migration) |
| Cube computation | Direct initialization of L0A Buffer or L0B Buffer is not supported | Fill, LoadData | [Initializing L1 Buffer first and then moving](#fill-l0a-l0b-migration) |
| Cube computation | The Fixpipe native parameter structure and the units of some parameters change | Fixpipe | [Switching the Fixpipe parameter structure](#fixpipe-params-migration) |
| System variable access | Monitoring UB reads and writes within a specified range is not supported | CheckLocalMemoryIA | [Deleting the CheckLocalMemoryIA call](#check-local-memory-ia-migration) |

## Vector Computation<a id="vector-compute"></a>

### Deleting the Subnormal Hardware Computation Feature<a id="subnormal-migration"></a>

**Note:** A subnormal floating-point number is a floating-point number whose exponent bits are all 0 and whose mantissa is not 0. It is used to represent a value smaller than the minimum normal number, avoiding "underflow to 0". Version 3510 does not support subnormal by default, and subnormal floating-point numbers are treated as 0 during computation.

**Compatibility solution:** For basic APIs that support the **config** parameter, you can configure the subnormal computation mode by setting the **config** template parameter. For some basic APIs that do not accept the **config** parameter and for advanced APIs, you can configure the subnormal handling mode through the compilation option `--cce-ftz`, which defaults to `true`. When `--cce-ftz=false`, subnormal is preserved; when `--cce-ftz=true`, the FTZ (Flush-To-Zero) mode is used. Software simulation processes subnormal data through precision extension and other methods to prevent it from underflowing to 0.

**Table 2**  APIs and config parameters related to subnormal

| Ascend C Basic API | Compatibility Description |
|-----------------|---------|
| Exp, Ln, Reciprocal, Sqrt, Rsqrt, Div | The Ln API is used as an example.<br>Configure the subnormal computation mode through the algo parameter of the LnConfig structure. The values of algo are as follows:<br>- **LnAlgo::INTRINSIC**: the subnormal handling mode is controlled by the compilation option **--cce-ftz**, which defaults to **true**.<br>- **LnAlgo::PRECISION_1ULP_FTZ_TRUE**: the result is obtained through a single instruction, using the FTZ mode.<br>- **LnAlgo::PRECISION_1ULP_FTZ_FALSE**: subnormal data computation is supported.<br>The default value **DEFAULT_LN_CONFIG** of this parameter is as follows:<br>`constexpr LnConfig DEFAULT_LN_CONFIG = { LnAlgo::INTRINSIC };` |

**Migration example:**

Before migration (2201): The hardware supports subnormal by default, so you can call the API directly.

```cpp
// Reference sample code line: 45
AscendC::Ln<half>(dstLocal, srcLocal, count);
```

After migration (3510): Configure algo to **PRECISION_1ULP_FTZ_FALSE** through the **LnConfig** template parameter so that software simulation preserves subnormal. Note that **config** is a const reference template parameter, so the argument must have static storage duration. Use static constexpr.

```cpp
// Reference sample code lines: 49-50
static constexpr AscendC::LnConfig CONFIG = { AscendC::LnAlgo::PRECISION_1ULP_FTZ_FALSE };
AscendC::Ln<half, CONFIG>(dstLocal, srcLocal, count);
```

**Corresponding sample**: [subnormal compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/subnormal).

## Data Movement

### Deletion of the L1 Buffer-\>GM Hardware Path<a id="data-copy-l1-to-gm-migration"></a>

**Description**: The hardware removes the path from the L1 Buffer to GM, so data cannot be moved directly from the L1 Buffer to GM. Existing APIs do not support direct movement from the L1 Buffer to GM.

**Compatibility solution**: For cube-only computation scenarios, allocate an additional identity cube in GM, compute the output to the L0C Buffer through Mmad cube multiplication, and then move it from the L0C Buffer to GM through Fixpipe. For scenarios that combine vector and cube computation, data can be moved from the L1 Buffer to the Unified Buffer (UB), and then moved to GM. The following uses a cube-only computation scenario as an example to describe the core operator process.

**Migration example**:

Before migration (2201): GM→L1→GM, direct movement.

```cpp
// Reference sample code lines: 123-129
AscendC::LocalTensor<T> leftMatrix(AscendC::TPosition::A1, a1Addr, M * K);
AscendC::DataCopy(leftMatrix, aGlobal, M * K);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::DataCopy(cGlobal, leftMatrix, M * K);  // L1 -> GM
```

After migration (3510): GM→L1→L0A/L0B→L0C→GM, moved through cube multiplication + Fixpipe.

```cpp
// Reference sample code lines: 139-150
CopyGmToL1A(a1Local);  // GM -> L1A
CopyGmToL1B(b1Local);  // GM -> L1B (B is the identity cube)
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
Load2DL1AToL0A(a1Local, a2Local);  // L1A -> L0A
Load2DL1BToL0B(b1Local, b2Local);  // L1B -> L0B
AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID1);
AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID1);
Compute(co1Local, a2Local, b2Local);  // Mmad cube multiplication, A × identity cube = A
AscendC::SetFlag<AscendC::HardEvent::M_FIX>(EVENT_ID2);
AscendC::WaitFlag<AscendC::HardEvent::M_FIX>(EVENT_ID2);
CopyL0CToGm(co1Local);  // L0C -> GM via Fixpipe
```

**Corresponding sample**: [L1 to GM movement compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm).

### Setting the L1 Buffer Boundary Value Is Not Supported<a id="set-load-data-boundary-migration"></a>

**Note:** The 3510 architecture removes the registers related to setting the L1 Buffer boundary value, and the **SetLoadDataBoundary** API is no longer supported. This API is used to set the L1 Buffer boundary value for **LoadData** (convolution data movement). If the address of a source operand in the L1 Buffer exceeds the set boundary when the instruction processes the source operand, data is read from the start address of the L1 Buffer. Setting the value to **0** indicates no boundary, and the entire L1 Buffer can be used.

**Compatibility solution:**

-   When the **boundaryValue** parameter of the API in [NPU architecture version 2201](../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) is set to **0**, it is equivalent to the 3510 architecture version.
-   If operands need to be read cyclically in the L1 Buffer, manually split the corresponding **LoadData** (convolution data movement) API into multiple instructions and manually wrap around.

**Figure 1**  Manually splitting LoadData to implement L1 Buffer address wrap-around

![](../../../figures/fig_1_zn.png)

**Migration example:**

Before migration (2201): Set the boundary through **SetLoadDataBoundary**, and a single **LoadData** automatically wraps around.

```cpp
// Reference sample code lines: 95-101
AscendC::LoadDataRepeatParam repeatParams;
repeatParams.repeatStride = 0;  // Distance between the start addresses of adjacent iterations, in units of 16 elements
repeatParams.repeatTime = 1;    // Number of iterations in the height direction
repeatParams.repeatMode = 0;    // Iteration direction: 0 indicates the height direction
AscendC::SetLoadDataRepeat(repeatParams);
AscendC::SetLoadDataBoundary(1024);  // Set the L1 boundary to 1024B
AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
```

After migration (3510): Delete **SetLoadDataBoundary** and call **LoadData** multiple times to wrap around manually.

```cpp
// Reference sample code lines: 103-121
uint16_t dstStride = AscendC::DivCeil(M / 2, 16);
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.repeatStride = 0;  // Distance between the start addresses of adjacent iterations, in units of 16 elements
repeatParams.repeatTime = 1;    // Number of iterations in the height direction
repeatParams.repeatMode = 0;    // Iteration direction: 0 indicates the height direction
repeatParams.dstStride = dstStride;  // K-axis offset of the output cube, in units of 512B fractals
AscendC::SetLoadDataRepeatWithStride(repeatParams);
AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a3 = a2[256];   // Destination address offset by 256 elements
AscendC::LoadData(a3, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a4 = a2[512];   // Destination address offset by 512 elements
AscendC::LoadData(a4, leftMatrix, loadData3dParamsPro);
AscendC::LocalTensor<T> a5 = a2[768];   // Destination address offset by 768 elements
AscendC::LoadData(a5, leftMatrix, loadData3dParamsPro);
```

**Corresponding sample**: [SetLoadDataBoundary compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary).

## Cube Computation

### Cube Compute Unit No Longer Supports the int4b_t Data Type<a id="cube-int4-migration"></a>

**Note:** Compared with architecture version 2201, the Cube compute unit in architecture version 3510 does not support int4b_t. The involved basic APIs are **LoadData**, **Mmad**, and **LoadDataWithTranspose**, which no longer support int4b_t.

**Compatibility solution:** On the operator side, write a CV fusion operator to perform the Cast conversion from int4b_t to int8_t on the Vector Core, then move the data to L1 through UB before performing Mmad computation.

**Migration example:**

Before migration (2201): Directly use int4b_t for Matmul computation.

```cpp
// Reference sample code lines: 128-133
AscendC::Matmul<A_TYPE_S4, B_TYPE_S4, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
mm.SetTensorA(gmA, false);  // gmA is of the int4b_t type
mm.SetTensorB(gmB, false);  // gmB is of the int4b_t type
mm.IterateAll(gmC);
```

After migration (3510): First Cast int4b_t to int8_t on the Vector Core, then perform int8_t cube multiplication.

```cpp
// Reference sample code lines: conversion logic in lines 140-156, cube multiplication logic in lines 128-133
// Step1 - int4b_t -> int8_t conversion on Vector Core
AscendC::LocalTensor<AscendC::int4b_t> int4SrcLocalTensor =
    srcLocalTensor.ReinterpretCast<AscendC::int4b_t>();
AscendC::Cast<half, AscendC::int4b_t>(tmpTensor, int4SrcLocalTensor,
    AscendC::RoundMode::CAST_NONE, count * 2);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Cast<int8_t, half>(dstLocalTensor, tmpTensor,
    AscendC::RoundMode::CAST_CEIL, count * 2);
// Step2 - int8_t cube multiplication
AscendC::Matmul<A_TYPE_S8, B_TYPE_S8, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
mm.SetTensorA(gmA, false);  // gmA has been converted to int8_t
mm.SetTensorB(gmB, false);  // gmB has been converted to int8_t
mm.IterateAll(gmC);
```

**Corresponding sample**: [Matmul compatibility sample for int4 data type](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/matmul_s4).

### L0A Buffer Fractal Change: Conversion from Zz to Nz Format<a id="l0a-fractal-migration"></a>

**Description**: The involved APIs are **LoadData**, **Mmad**, and **LoadDataWithTranspose**.

-   In architecture version 2201, when participating in cube multiplication computation (A \* B = C), the data layout formats of the A, B, and C cubes are Zz, Zn, and Nz, respectively. The A, B, and C cubes are located in the L0A Buffer, L0B Buffer, and L0C Buffer, respectively.

    Cube A: Each fractal cube is row-major order internally, and the fractal cubes are row-major order between each other. The fractal Shape is `16 x (32B / sizeof(AType))`, with a size of 512 bytes.

    Cube B: Each fractal cube is column-major order internally, and the fractal cubes are row-major order between each other. The fractal shape is `(32B / sizeof(BType)) x 16`, with a size of 512 bytes.

    Cube C: Each fractal cube is row-major order internally, and the fractal cubes are column-major order between each other. The fractal shape is 16 x 16, with a size of 256 elements.

    **Figure 2**  Cube fractal layout in architecture 2201

    ![](../../../figures/fig_2_zn.png)

-   In architecture version 3510, when participating in cube multiplication computation (A \* B = C), the data layout formats of the A, B, and C cubes are Nz, Zn, and Nz, respectively.

    Cube A: Each fractal cube is row-major order internally, and the fractal cubes are column-major order between each other. Its shape is `16 x (32B / sizeof(AType))`, with a size of 512 bytes.

    Cube B: Each fractal cube is column-major in its interior and row-major between fractal cubes. Its shape is `(32B / sizeof(BType)) x 16`, with a size of 512 bytes.

    Cube C: Each fractal cube is row-major in its interior and column-major between fractal cubes. Its shape is 16 x 16, with a size of 256 elements.

    **Figure 3**  Fractal layout of cubes under the 3510 architecture

    ![](../../../figures/fig_3_zn.png)

**Compatibility solution**: Depending on whether the L0A Buffer is reused, adaptation is divided into two scenarios: L0A Buffer not reused and L0A Buffer reused (M-axis splitting).

**Figure 4**  Difference in fractal conversion from L1 Buffer to L0A Buffer

![](../../../figures/fig_4_zn.png)

#### L0A Buffer Non-Reuse Scenario

In this scenario, the complete A cube is moved into the L0A Buffer at once, and the cube computation is completed through a single Mmad.

-   2201 architecture: The A cube in the L1 Buffer is in Nz fractal format, while the L0A Buffer requires the Zz fractal format. Therefore, **LoadData** (2D cube movement) is required to convert the Nz fractal to the Zz fractal.
-   3510 architecture: The A cubes in both the L1 Buffer and the L0A Buffer are in Nz fractal format, so no fractal conversion is required. The parameters of **LoadData** (2D cube movement) need to be adjusted according to the data layout of the Nz fractal.

**Figure 5**  Fractal adaptation in the L0A Buffer non-reuse scenario

![](../../../figures/fig_5_zn.png)

**Migration example**:

Before migration (2201): The A cube in the L1 Buffer is in Nz fractal format. The M-axis direction is used as the outer loop, and each time **LoadData** moves the fractal along the K-axis direction into the L0A Buffer and converts it to the Zz fractal.

```cpp
// Reference sample code lines: 98-110
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < mBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = kBlocks;
    loadDataParams.srcStride = mBlocks;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK;
    dstOffset += K * CUBE_BLOCK;
}
```

After migration (3510): The A cubes in both the L1 Buffer and the L0A Buffer are in Nz fractal format. Use `LoadData2DParamsV2` to describe the movement ranges along the M-axis and K-axis, and complete the Nz fractal movement through a single **LoadData** without fractal conversion.

```cpp
// Reference sample code lines: 112-124
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;  // M-axis start position, unit is 16 elements
loadDataParams.kStartPosition = 0;  // K-axis start position, unit is 32 bytes
loadDataParams.mStep = mBlocks;     // M-axis movement range, unit is 16 elements
loadDataParams.kStep = kBlocks;     // K-axis movement range, unit is 32 bytes
loadDataParams.srcStride = mBlocks; // Source adjacent K-axis fractal interval, unit is 512 bytes
loadDataParams.dstStride = mBlocks; // Destination adjacent K-axis fractal interval, unit is 512 bytes
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2, a1, loadDataParams);
```

#### L0A Buffer Reuse Scenario

In this scenario, cube A is split into A1 and A2 along the M axis, and Mmad computation is performed with cube B respectively to obtain C1 and C2.

-   2201 architecture: Cube A in the L0A Buffer is in Zz fractal format. After splitting along the M axis, A1 and A2 remain contiguous in the L0A Buffer. The movement from the L1 Buffer to the L0A Buffer is the same as in the non-reuse scenario. For the second Mmad computation, only the address offset of cube A needs to be adjusted.
-   3510 architecture: Cube A in the L0A Buffer is in Nz fractal format. After splitting along the M axis, A1 and A2 cannot be used separately as contiguous Nz subcubes. Two **LoadData** operations (2D cube movement) are required to move the upper and lower halves of cube A respectively, rearranging them into two contiguous Nz subcubes. The Mmad computation logic and pipeline scheduling do not need to be modified.

**Figure 6**  Fractal adaptation in the L0A Buffer reuse scenario

![](../../../figures/fig_6_zn.png)

**Migration example**:

Before migration (2201): The movement from the L1 Buffer to the L0A Buffer is the same as in the non-reuse scenario. A1 and A2 are contiguous in the Zz fractal. When executing the second Mmad, offset the addresses of cube A and cube C to the lower half.

```cpp
// Reference sample code lines: cube A movement at 98-110, two Mmad operations at 180-187
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < mBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = kBlocks;
    loadDataParams.srcStride = mBlocks;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK;
    dstOffset += K * CUBE_BLOCK;
}

AscendC::MmadParams mmadParams;
mmadParams.m = M / 2;  // Split cube A into upper and lower halves along the M axis
mmadParams.n = N;
mmadParams.k = K;
mmadParams.cmatrixInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
// Offset both the A cube and the C cube to the lower half to compute C2 = A2 * B.
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);
```

After migration (3510): After axis splitting along the M axis, A1 and A2 are non-contiguous in the original Nz fractal. Move the upper half and the lower half separately, rearrange them into two contiguous Nz subcubes, and then reuse the same Mmad computation logic as in 2201.

```cpp
// Reference sample code lines: A cube movement is in lines 112-132, and the two Mmad operations are in lines 180-187.
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;      // Start from the upper half of the A cube, in units of 16 elements.
loadDataParams.kStartPosition = 0;      // Start position of the K axis, in units of 32 bytes.
loadDataParams.mStep = mBlocks / 2;     // Move half of the M axis each time, in units of 16 elements.
loadDataParams.kStep = kBlocks;         // Movement range of the K axis, in units of 32 bytes.
loadDataParams.srcStride = mBlocks;     // Interval between adjacent K-axis fractals in the source, in units of 512 bytes.
loadDataParams.dstStride = mBlocks / 2; // Interval between adjacent K-axis fractals in the subcube, in units of 512 bytes.
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
// Move the upper half of the A cube.
AscendC::LoadData(a2, a1, loadDataParams);
// Move the lower half of cube A
loadDataParams.mStartPosition = mBlocks / 2;
AscendC::LoadData(a2[M * K / 2], a1, loadDataParams);

AscendC::MmadParams mmadParams;
mmadParams.m = M / 2;  // Split cube A into upper and lower halves along the M axis
mmadParams.n = N;
mmadParams.k = K;
mmadParams.ccubeInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
// Offset both cube A and cube C to the lower half, and compute C2 = A2 * B
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);
```

**Corresponding sample**: [pattern_transformation compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/pattern_transformation).

### Deletion of the 4:2 Structured Sparsity Feature<a id="structured-sparsity-migration"></a>

**Note**: The 2201 architecture supports 4:2 structured sparsity. Through `LoadDataWithSparse`, the 512B dense weight cube in the L1 Buffer is moved to the L0B Buffer while the 128B index cube is read to implement sparsification, and then `MmadWithSparse` is used to perform sparse cube multiply-accumulate. The 3510 architecture does not support the structured sparsity feature, and both `LoadDataWithSparse` and `MmadWithSparse` are unavailable.

**Compatibility solution**: Do not call `LoadDataWithSparse` to perform dense-to-sparse cube conversion. Instead, directly use `Mmad` for normal dense cube computation. For sparse cube related algorithms, see the introduction in **MmadWithSparse**.

**Migration example**:

Before migration (2201): Use **LoadDataWithSparse** + **MmadWithSparse** to perform sparse cube multiplication.

```cpp
AscendC::LoadDataWithSparse(b2Local, b1Local, loadDataSparseParams);
AscendC::MmadWithSparse(co1Local, a2Local, b2Local, mmadSparseParams);
```

After migration (3510): Use **LoadData** + **Mmad** to perform dense cube multiplication.

```cpp
AscendC::LoadData(b2Local, b1Local, loadDataParams);
AscendC::Mmad(co1Local, a2Local, b2Local, mmadParams);
```

### Deletion of the GM-\>L0A Buffer/L0B Buffer Path<a id="gm-to-l0a-l0b-migration"></a>

**Description**: The hardware removal GM-\>L0A of the Buffer/L0B Buffer path means that these paths are no longer supported when **LoadData** is called.

**Compatibility solution**: To implement GM→L0A Buffer/L0B Buffer movement, split it into two steps: first move from GM to L1 Buffer, then move from L1 Buffer to L0A Buffer and L0B Buffer.

**Migration example**:

Before migration (2201): GM→L0A, single-step **LoadData** direct movement.

```cpp
// Reference sample code line: no corresponding sample is available; this section is illustrative migration code for 2201
AscendC::LoadData(a2Local, aGlobal, loadDataParams);  // Source is GM
```

After migration (3510): GM→L1→L0A, two-step movement.

```cpp
// Reference sample code line: see lines 40-42 for GM→L1 and lines 54-64 for L1→L0A
// Step1 - GM -> L1
AscendC::Nd2NzParams intriParams{1, M, K, 0, K, M, 1, 0};
AscendC::DataCopy(a1Local, aGlobal, intriParams);
// Step2 - L1 -> L0A
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**Corresponding sample**: [L1 to GM movement compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm) (the 3510 branch implements the GM→L1→L0A/L0B path split).

### Deletion of Hardware Instructions Related to L0A Buffer/L0B Buffer Initialization<a id="fill-l0a-l0b-migration"></a>

**Note:** The 2201 architecture supports directly initializing the L0A Buffer and L0B Buffer through the `Fill` API. The 3510 architecture deletes the hardware instructions related to L0A Buffer/L0B Buffer initialization, and the `Fill` API no longer supports directly initializing L0A/L0B.

**Compatibility solution:** First initialize the L1 Buffer through the `Fill` API, and then move the data on the L1 Buffer to the L0A Buffer and L0B Buffer through the `LoadData` API.

**Migration example:**

Before migration (2201): Call **Fill** to initialize the L0A Buffer.

```cpp
// Reference sample code lines: 32-37
AscendC::Fill(a2Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 512), 0, 1});
```

After migration (3510): Call **Fill** to initialize L1, and then call **LoadData** to move the data to the L0A Buffer.

```cpp
// Reference sample code lines: L1 initialization at 48-51, L1 to L0A movement at 59-71
// Step1 - Initialize L1 through Fill
AscendC::Fill(a1Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 32), 0, 1});
AscendC::PipeBarrier<PIPE_MTE1>();
// Step2 - Move data from L1 to L0A through LoadData
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**Corresponding sample**: [Fill compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fill).

### Fixpipe API Parameter Structure Switch<a id="fixpipe-params-migration"></a>

**Note:** The Fixpipe API under the 2201 architecture uses the `FixpipeParamsV220` parameter structure. The 3510 architecture introduces the native `FixpipeParamsArch3510` parameter structure while remaining compatible with `FixpipeParamsV220`. However, the native structure is recommended to obtain full capabilities.

**Precautions on migrating from FixpipeParamsV220 to FixpipeParamsArch3510:**

- When NZ2ND/NZ2DN is not enabled, the unit of the `dstStride` parameter changes. This parameter indicates the start address offset between adjacent Z layouts in the destination NZ cube.
  - Under the 2201 architecture, the unit is 32B.
  - Under the 3510 architecture, the unit is element.
- Under the 3510 architecture, parameters such as `ndNum`, `srcNdStride`, and `dstNdStride` are consolidated under the `params` structure parameter.

**Compatibility solution:**
The 3510 architecture is compatible with `FixpipeParamsV220`, but migration to `FixpipeParamsArch3510` is recommended. In the NZ2ND scenario, parameters such as `ndNum`, `srcNdStride`, and `dstNdStride` are wrapped in `params` and passed. In non-NZ2ND scenarios, `dstStride` must be recalculated in element units.

**Migration example (NZ2ND scenario):**

Before migration (2201): Use **FixpipeParamsV220**.

```cpp
// Reference sample code lines: 155-174
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.ndNum = 1;
fixpipeParams.srcNdStride = 0;
fixpipeParams.dstNdStride = 0;
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
fixpipeParams.nSize = baseN;
fixpipeParams.dstStride = baseN;
fixpipeParams.quantPre = QuantMode_t::NoQuant;
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, cL0C, fixpipeParams);
```

After migration (3510): Use the native **FixpipeParamsArch3510** structure. The top-level fields `ndNum`, `srcNdStride`, and `dstNdStride` in 2201 must be migrated to the corresponding fields of the `params` substructure respectively. The common parameter field names remain the same.

```cpp
// Reference sample code lines: 155-174
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.params.ndNum = 1;        // Number of NZ cubes in NZ2ND movement
fixpipeParams.params.srcNdStride = 0;  // No need to configure the source cube stride for a single NZ cube
fixpipeParams.params.dstNdStride = 0;  // No need to configure the destination cube stride for a single ND cube
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
fixpipeParams.nSize = baseN;
fixpipeParams.dstStride = baseN;
fixpipeParams.quantPre = QuantMode_t::NoQuant;
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, cL0C, fixpipeParams);
```

**Migration example (NZ2NZ scenario, different dstStride units)**:

Before migration (2201): The **dstStride** unit is a datablock (32 bytes).

```cpp
// Reference sample code lines: 175-189
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
fixpipeParams.nSize = CeilAlign(baseN, c0Size);
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(cGM, cL0C, fixpipeParams);
```

After migration (3510): The **dstStride** unit is the number of elements.

```cpp
// Reference sample code lines: 175-189
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::NZ> fixpipeParams;
fixpipeParams.dstStride = baseM * c0Size;
fixpipeParams.nSize = CeilAlign(baseN, c0Size);
fixpipeParams.mSize = baseM;
fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(cGM, cL0C, fixpipeParams);
```

**Corresponding sample**: [Fixpipe parameter structure switch compatibility sample](../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fixpipe_params_switch).

## System Variable Access

### CheckLocalMemoryIA No Longer Supported, Related Registers Removed by Hardware<a id="check-local-memory-ia-migration"></a>

**Note:** **CheckLocalMemoryIA** monitors UB read/write behavior within a specified range. If read/write behavior within the specified range is detected, an EXCEPTION error is reported; if no read/write behavior within the specified range is detected, no error is reported.

**Compatibility solution:** This API is a debugging API and has no impact on functionality. Simply delete the call.

**Migration example:**

Before migration (2201): Call **CheckLocalMemoryIA** to monitor UB read/write.

```cpp
// Reference sample code line: no corresponding sample available; this is migration illustration code
AscendC::CheckLocalMemoryIA(...);
```

After migration (3510): The API is deleted. Directly remove the call.

```cpp
// Reference sample code line: no corresponding sample available; this is migration illustration code
// The API has been deleted; no call is required
```
