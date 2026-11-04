# Cube Computation Programming<a name="ZH-CN_TOPIC_0000002600000003"></a>

<!-- md-trans-meta sourceCommit=efbb24c9bdac3f8940cf150f90a686c06f5dd4f1 translatedAt=2026-10-28T08:54:13.448Z pushedAt=2026-11-03T07:08:03.560Z nl=2 -->

This document describes how to use the C++ Tensor programming API in Ascend C to write **cube computation code** based on on-chip storage such as the L1 Buffer and L0A Buffer.

The AI Core contains the Vector compute unit (deployed on the AIV core) and the Cube compute unit (deployed on the AIC core). The Cube compute unit is dedicated to executing cube operations and directly accesses the following dedicated caches: the L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the initial accumulation values and the cube computation results.

## Basic Steps of Cube Programming

The basic steps of cube computation are: data move-in → data load → computation → data move-out. To describe a common cube computation, perform the following four steps:

1. Move data from GM into `Local Memory (L1 Buffer)` through DMA.
2. Move data from L1 Buffer into `L0A Buffer & L0B Buffer` through data load.
3. Perform cube computation, and store the result in `L0C Buffer`.
4. Move data from `L0C Buffer` out to GM through DMA.

## Cube Computation Memory Management

### Cube Computation Memory Allocation

Cube computation programming mainly performs cube computation through the L0A/L0B/L0C Buffer. Developers need to move input data from Global Memory (hereinafter referred to as GM) into the L1 Buffer, then move it from the L1 Buffer into the L0A/L0B Buffer, and finally complete the computation through Cube computation instructions. Both the L1 Buffer and the L0A/L0B/L0C Buffer are managed through LocalTensor objects. Developers can use the LocalMemAllocator allocator to allocate the memory space required for cube computation.

```cpp
__global__ __cube__ void matmul_custom(...)
{
    constexpr uint32_t m_size = 128;
    constexpr uint32_t n_size = 128;
    constexpr uint32_t k_size = 256;

    AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;

    // Allocate L1 Buffer for cube A and B
    AscendC::LocalTensor<half> l1a_buffer = l1Allocator.Alloc<half, m_size * k_size>();
    AscendC::LocalTensor<half> l1b_buffer = l1Allocator.Alloc<half, k_size * n_size>();
    // Allocate L0A/L0B/L0C Buffer for cube computation
    AscendC::LocalTensor<half> l0a_buffer = l0aAllocator.Alloc<half, m_size * k_size>();
    AscendC::LocalTensor<half> l0b_buffer = l0bAllocator.Alloc<half, k_size * n_size>();
    AscendC::LocalTensor<float> l0c_buffer = l0cAllocator.Alloc<float, m_size * n_size>();

    // ... Use Buffer for cube computation
}
```

### Cube Computation Memory Layout

The cube compute unit adopts a block-based computation logic, where 16×16 is the basic fractalization granularity of the cube compute unit (for the half data type; for int8, it is 16×32 or 32×16; for fp32, it is fractalized into 16×8). The hardware can process multiple cube compute units in parallel. In a traditional linear storage layout (such as row-major or column-major), data is stored contiguously by row or by column. When a 16×16 rectangular data block needs to be read, if it is obtained directly from memory, multiple non-contiguous memory addresses must be accessed to "collect" the data row by row. Such non-contiguous access significantly degrades memory efficiency.
To solve the preceding problem, the cube fractal storage format is introduced. This format rearranges data at the software layer so that each 16×16 computation block is stored contiguously in physical memory. The hardware only needs to initiate a single contiguous read to load the entire data block into on-chip registers, avoiding the overhead of multi-address addressing and thereby greatly improving the data throughput of the cube compute unit. For the half data type, 16×16 is the basic fractalization granularity of the cube compute unit. The product models that currently support cube computation include [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) and [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).
To pursue extremely high data throughput and computation density, the cube compute unit of the Ascend AI processor adopts a proprietary hardware architecture design. Unlike the contiguous linear storage commonly used by general-purpose CPUs, this unit requires that the input and output data conform to a specific fractal storage format. When developing operators using the Mmad-class APIs of Ascend C, a deep understanding of these data formats is crucial for implementing correct data movement, blocking, and performance optimization.
For ease of understanding and memorization, this document adopts the intuitive "large Y small x" naming convention:
 - Large Y (Z/N): indicates the arrangement order among fractal cubes (Z is row-major, N is column-major).
 - Small x (z/n): indicates the arrangement order of elements within a fractal cube (z is row-major, n is column-major).

### Key Fractal Formats in Detail

A cube fractal storage format describes the data layout of a multidimensional Tensor in memory. To adapt to the physical read path of the cube compute unit, different product forms have different format requirements: For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the cube multiplication C = A × B requires that the left cube A use the Zz format, the right cube B use the Zn format, and the result cube C use the Nz format, as shown in the following figure:

![Data formats involved in the cube multiplication scenario](../../../../figures/matmul_data_2.png)

For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the cube multiplication C = A × B requires that the left cube A use the Nz format, the right cube B use the Zn format, and the result cube C use the Nz format, as shown in the following figure:

![Data formats involved in the cube multiplication scenario](../../../../figures/matmul_data_1.png)

#### ND Format (N-Dimension)

The ND format is a general N-dimensional tensor format in which data is stored contiguously and linearly in memory, typically in the GM. Before cube computation, it usually needs to be converted to the Nz format to adapt to the Cube compute unit.

#### Nz Format

The Nz format is mainly used to store data in the L1 Buffer and L0C Buffer. For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the L0A Buffer also uses the Nz fractal format. This format uses large N (external column-major) + small z (internal row-major) for data layout.

![Nz data layout format](../../../../figures/nz_format.png)

Nz is an intermediate format of the Ascend architecture, between linear data (ND) and computation-specific data (Zz/Zn), which facilitates format conversion. The large N layout (column-major block) is designed to facilitate multi-core parallelism or the convenience of subsequent operators (such as Vector computation) when processing data in the Channel dimension.

> 📌 Application scenarios of the Nz format in different buffers:
> - **L0C Buffer**: Stores the cube multiplication result. The fractal shape is 16×16 (256 elements for the half data type), suitable for efficient computation by the cube compute unit.
> - **L1 Buffer**: Stores intermediate data. The fractal shape is 16×(32B/sizeof(DataType)), with a size of 512 bytes, facilitating conversion to the Zz and Zn formats.
> - **L0A Buffer (only for [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md))**: Stores the left cube.

#### Zz Format

For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the L0A Buffer uses this format ([NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) uses the Nz format). This format uses large Z (external row-major order) + small z (internal row-major order).

![Zz format (using the half type as an example)](../../../../figures/zz_half.png)

In cube multiplication, cube A reads data row by row. The row-major order characteristic of the Zz format makes the physical addresses of elements in the same row contiguous, which matches the computation access pattern. This facilitates the left channel of the Cube unit to read data continuously through DMA, avoiding strided access and improving data movement efficiency.

#### Zn Format

The Zn format is mainly used to store data in the L0B buffer. If the offline-processed right cube data is directly in the Zn format, this format is stored in the GM and L1 buffer. This format uses large Z (external row-major order) + small n (internal column-major order).

![Zn format (using the half type as an example)](../../../../figures/zn_half.png)

Cube multiplication requires reading the column vector of the B cube. Ordinary row-major storage causes column data to be non-contiguous in memory. The Zn format stores data in column-major order within the fractal, so that column data is compactly arranged in L0B, facilitating the linear reading of complete column vectors by the right channel of the cube unit and avoiding strided access.

### Format Conversion

Based on the preceding information, Global Memory uses the ND format and L1 Buffer uses the Nz format. Therefore, ND2NZ format conversion is required.

![ND2NZ format conversion](../../../../figures/nd2nz_conv.png)

Using the preceding figure as an example, the figure shows the memory layout changes of a specific cube with M=40 and K=56 (data type: half) when it is moved from GM to L1 Buffer and converted to the Nz format:
- Original data (ND format): The left GM stores the original cube with a shape of 40×56. The data is laid out linearly and contiguously (as indicated by the long arrow in the figure), following the row-major rule.
- Alignment and padding: Due to the fractalization requirement of the Cube unit, the target cube must be divisible by a 16×16 Block (assuming the data type is half, that is, 32/sizeof(half)=16). M-axis padding: The original M=40 is not divisible by 16 and must be padded upward to 48 (16×3). K-axis padding: The original K=56 is not divisible by 16 and must be padded upward to 64 (16×4). In the figure, the gray area is the invalid data generated by padding, and the white area is the original valid data.
- Relayout (Nz format): In the right L1 Buffer, the data is reorganized into 3×4 fractal blocks. Z-shaped flow: Within each 16×16 small block and between blocks, the data is stored in a specific Z-shaped order (as indicated by the polyline arrows in the figure), completing the physical mapping from "linear" to "fractal" and preparing the data for cube computation.

## Basic Tensor Programming Paradigm

A basic Tensor only encapsulates the data pointer and size information. Developers need to manually configure the move parameters, compute the fractalization stride, and manage the memory layout. It is suitable for scenarios that require fine-grained control over hardware behavior.

### Cube Move-in

The cube move-in interface implements data transfer from Global Memory through the L1 Buffer to the L0A/L0B Buffer, and is usually used in coordination with the Mmad interface. This interface supports multiple move modes: 2D move, 3D format conversion, compressed data decompression, cube transposition, sparse data processing, and so on.

Developers control the move path, start position, length, and other information by configuring parameters, fully leveraging hardware capabilities to maximize operator performance. Each data path corresponds to specific storage levels and access characteristics.

Based on the preceding data flow and fractalization knowledge, C++ Tensor programming provides the following cube move-in capabilities (for the complete interfaces, see the API Reference):

| Data Path | Function    | C++ Tensor Programming API  | Fractalization Support |
|---------|---------|---------------|-------------|
| Global Memory → L1 Buffer   | Move with on-the-fly ND2NZ conversion  | `DataCopy(dst, src, nd2nzParams)` | ![ND->Nz](../../../../figures/ND_Nz.png) |
| L1 Buffer → L0A Buffer  | Move data from the L1 Buffer to the L0A Buffer  | `LoadData(dst, src, l12l0Params)` | ![Nz->Zz](../../../../figures/Nz_Zz.png)<br>On [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), L0A is fractalized into Nz, and the move is Nz-Nz |
| L1 Buffer → L0B Buffer  | Move data from the L1 Buffer to the L0B Buffer  | `LoadData(dst, src, l12l0Params)` | ![Nz->Zn](../../../../figures/Nz_Zn.png) |

#### GM2L1 On-the-Fly ND2NZ Move

This capability mainly moves a cube from GM to the L1 Buffer and supports ND-to-Nz format conversion during data movement.

This capability is implemented through the `DataCopy` API in coordination with the `Nd2NzParams` parameter. Parameters such as `ndNum` and `srcDValue` are configured to complete the GM2L1 move and ND2NZ format conversion.

![Example of ND2NZ move from GM to L1](../../../../figures/gm_l1_nd2nz.png)

Taking the preceding figure as an example, to move cube A (Shape(16, 23), data type half) from GM to L1, for [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), call the API as follows:

```cpp
// Allocate L1 Buffer space
constexpr uint64_t m = 16;
constexpr uint64_t k = 32;
AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
AscendC::LocalTensor<half> dst = l1Allocator.Alloc<half, m * k>();

AscendC::GlobalTensor<half> srcGlobal;
srcGlobal.SetGlobalBuffer((__gm__ half*)src);

// Set parameters for Nd2NzParams
AscendC::Nd2NzParams intriParams{/*ndNum*/1, /*nValue*/16, /*dValue*/23, /*srcNdMatrixStride*/0, /*srcDValue*/32, /*dstNzC0Stride*/16, /*dstNzNStride*/1, /*dstNzMatrixStride*/0};

// Global Memory -> Local Memory with ND2NZ conversion
AscendC::DataCopy(dst, srcGlobal, intriParams);
```

#### L12L0 Data Movement

This capability mainly moves a cube from the L1 Buffer to the L0A/L0B Buffer, and supports conversion from the Nz fractalization to the Zz or Zn format. For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), C++ Tensor programming provides the `LoadData` API coordinated with the `LoadData2DParamsV2` parameter to complete 2D movement from the L1 Buffer to the L0A/L0B Buffer using 512B data fractalization as the basic unit, and fractalization transpose can be configured through `ifTranspose`. For 3D movement scenarios, use the `LoadData` API coordinated with the `LoadData3DParamsV1` or `LoadData3DParamsV2` parameter; among them, NPU architecture version 3510 does not support `LoadData3DParamsV1`, so `LoadData3DParamsV2` must be used.

![L12L0A non-transpose movement](../../../../figures/l1_l0a_copy.png)

The move-in operation from the L1 Buffer to the L0A Buffer described above can be completed by calling the API as follows.

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
AscendC::LocalTensor<half> l1_buffer = l1Allocator.Alloc<half, 6400>();
AscendC::LocalTensor<half> l0a_buffer = l0aAllocator.Alloc<half, 2304>();

AscendC::LoadData2DParamsV2 l12l0Params;
// mStep is 2, kStep is 3, indicating transfer of 6 data blocks
l12l0Params.mStep = 2;
l12l0Params.kStep = 3;
// mStartPosition is 2, indicates M direction transfer starting position
l12l0Params.mStartPosition = 2;
// kStartPosition is 2, indicates K direction transfer starting position
l12l0Params.kStartPosition = 2;
// srcStride is 5, indicates interval between fractal starting addresses in K direction
l12l0Params.srcStride = 5;
// dstStride is 3: indicates interval between fractal starting addresses in M direction
l12l0Params.dstStride = 3;
// Do not transpose the fractal in this example
l12l0Params.ifTranspose = false;

AscendC::LoadData(l0a_buffer, l1_buffer, l12l0Params);
```

Movement to the L0B Buffer uses the same `LoadData` API implementation. The API parameters and implemented functions are the same as those of L12L0A. For more details, see [cube computation move-in](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md).

When the input A cube (GM) is in transposed layout (K×M), it becomes an Nz fractalization in (K×M) layout after being moved into the L1 Buffer. To make the L0A Buffer store a (M×K) cube, transpose must be performed during the movement from the L1 Buffer to the L0A Buffer. For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the `LoadDataWithTranspose` API can be used. The call example is as follows.

![Scenario requiring transpose](../../../../figures/need_transpose.png)

```cpp
// A cube LoadDataWithTranspose: Nz -> Zz
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
  AscendC::LoadDataWithTranspose(l0aTensor[i * dstOffset], l1aTensor[i * srcOffset], loadDataParams);
}
```

For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), `LoadDataWithTranspose` supports only the L1 Buffer to L0B Buffer data path. It is recommended to use `LoadData2dTransposeParamsV2` as the movement parameter, and the L1 Buffer to L0A Buffer transpose scenario described in this section is not supported. In this scenario, use the `LoadData` API coordinated with the `LoadData2DParamsV2` parameter instead, and set `ifTranspose` to `true`.

Taking the `half` data type as an example, the A cube is an Nz fractalization in (K×M) layout on the L1 Buffer and needs to be converted to an Nz fractalization in (M×K) layout on the L0A Buffer. It can be configured as follows:

```cpp
constexpr uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t kAlignL1 = CeilAlign(k, fractalShape[0]);
uint32_t mAlignL1 = CeilAlign(m, fractalShape[1]);
uint32_t mAlignL0 = CeilAlign(m, fractalShape[0]);

AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(kAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(l0aTensor, l1aTensor, loadDataParams);
```

### Cube Move-out

The cube move-out interface implements data transfer from the L0C Buffer to Global Memory or the L1 Buffer, supporting operations such as in-path quantization, ReLU, NZ2ND format conversion, and channel split/merge. Developers control the move path, position, and length through configuration parameters, and the interface is typically used after the `Mmad` interface. For [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md), it also supports moving from the L0C Buffer to the Unified Buffer (UB) and the UB dual-destination mode. The NZ2ND format conversion on the L0C Buffer-to-Global Memory path supports both NPU Architecture Version 2201 and 3510. The cube move-out capability is shown in the following figure:

The cube move-out process under [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md) is as follows:

![Cube move-out process](../../../../figures/mat_copy_out_a2a3.png)

The cube move-out process under [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) is as follows:

![Cube move-out process](../../../../figures/mat_copy_out_950.png)

The cube move-out interface supports flexible combinations of multiple in-path capabilities. Taking the L0C Buffer-to-Global Memory move scenario as an example: after the cube computation result is temporarily stored in the L0C Buffer, it needs to be moved to GM. During this process, operations such as in-path quantization, ReLU activation, format conversion, and channel split/merge can be executed in coordination. The following figures show the effective combinations of these in-path capabilities, the intermediate data types, and the complete data paths under different NPU architecture versions. For example, F32 data in the L0C Buffer can be quantized in-path through QF322B8_PRE to obtain B8 data, optionally enable ReLU, and then be output to GM through NZ2NZ/NZ2ND format conversion. In the figures, F32->F16 and F32->BF16 are data type conversions, and the remaining paths are in-path Scalar/Vector quantization modes.

The cube move-out capability under [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md) is as follows:

![L0C2GM move](../../../../figures/l0c_gm_a2a3.png)

The cube move-out capability under [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) is as follows:

![L0C2GM move](../../../../figures/l0c_gm_950.png)

C++ Tensor programming provides the `DataCopy` API, coordinated with the `DataCopyCO12DstParams` parameter, to enable the various on-the-fly capabilities of the chip. By directly passing configuration parameters, you can complete the on-the-fly move operation.
Example: Mmad contains a cube multiplication bias. The data type of the left cube and right cube is int8_t, and the data type of the result cube is int32_t. The quantization mode is DEQF16, and the Scalar quantization parameter is 2.0. The result computed by Mmad is quantized from int32_t to half and moved out.

```cpp
// Scalar quantization, quantization parameter is 2.0
float quantScalar = 2.0f;
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScalar));
// Write scalar quantization parameter to register for subsequent DataCopy instructions
AscendC::SetFixpipePreQuantFlag(deqScalar);
// Create parameters for DataCopy
AscendC::DataCopyCO12DstParams intriParams;
intriParams.nSize = n;
intriParams.mSize = m;
intriParams.srcStride = CeilAlign(m, CUBE_BLOCK);
intriParams.dstStride = n;
intriParams.quantPre = QuantMode_t::DEQF16;
intriParams.reluPre = 1; // Enable ReLU
intriParams.nz2ndEn = true; // Enable NZ2ND format conversion
// Set NZ2ND parameters before enabling NZ2ND in DataCopy
AscendC::SetFixpipeNz2ndFlag(/*ndNum*/1, /*srcNdStride*/0, /*dstNdStride*/1);
// Execute data transfer based on configurations in intriParams
AscendC::DataCopy(cGM, cLocal, intriParams);
```

In addition, developers can set Tensor quantization parameters or clear the unitFlag through `SetFixPipeConfig`, and set NZ2ND format conversion information through `SetFixpipeNz2ndFlag`. The `reluPreTensor` and `quantPreTensor` of `SetFixPipeConfig` need to be `LocalTensor` of data type `uint64_t` on the Fixpipe Buffer. Currently, `reluPreTensor` can be passed an empty `LocalTensor`. The corresponding APIs and processing capabilities are as follows:

```cpp
// Set vector quantization parameters for on-the-fly quantization during data transfer
AscendC::SetFixPipeConfig(reluPreTensor, quantPreTensor, isUnitFlag);

// Configure parameters for NZ to ND format conversion during data transfer
// Number of ND matrices
uint16_t ndNum = 1;
// Source stride in fractal unit, offset between adjacent source NZ cube headers
uint16_t srcNdStride = 0;
// Destination stride, offset between adjacent destination ND cube headers
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride);

// Set scalar quantization parameter for on-the-fly quantization in DataCopy (L0C->GM, L0C->L1)
float tmp = 0.5f;
// Convert float value tmp to uint64_t deqScalar
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
AscendC::SetFixpipePreQuantFlag(deqScalar);
```

For details, see the API reference document [Cube API Reference](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md).

### Cube Computation

[Mmad](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) is the core cube multiply-accumulate API through which Ascend C encapsulates the NPU hardware computation capability. It is used for developing operators such as fully connected layers and convolution layers, implementing the cube multiply-accumulate computation C = A x B + C (with L0A Buffer and L0B Buffer as inputs and L0C Buffer as output). For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the L0A Buffer is fractalized into Zz, the L0B Buffer is fractalized into Zn, and the L0C Buffer is fractalized into Nz, while for [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) products, the L0A Buffer is fractalized into Nz.

![NPU architecture version 3510 `Mmad` computation](../../../../figures/mmad_950.png)

**Table Description of matrices A, B, and C in cube multiplication computation ([NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md))**

| Cube | Storage Location | Dimension | Data Format | Data Type |
|---|---|---|---|---|
| A | L0A Buffer | M x K | Nz | half |
| B | L0B Buffer | K x N | Zn | half |
| C | L0C Buffer | M x N (Bias initialization supported, dimension 1 x N) | Nz | float |

C++ Tensor programming provides a series of `Mmad` overloaded APIs that support cube computation of different data types. The following code implements the cube multiplication computation shown in the preceding figure.

```cpp
// Define L0A/L0B cube sizes
constexpr uint32_t mSize = 64;
constexpr uint32_t kSize = 64;
constexpr uint32_t nSize = 64;

AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;

AscendC::LocalTensor<half> a_matrix = l0aAllocator.Alloc<half, mSize * kSize>();
AscendC::LocalTensor<half> b_matrix = l0bAllocator.Alloc<half, kSize * nSize>();
AscendC::LocalTensor<float> c_matrix = l0cAllocator.Alloc<float, mSize * nSize>();

AscendC::MmadParams mmadParams;
// Configure Mmad parameters
// m direction: 3 fractals, actual value = 3 × 16 = 48
mmadParams.m = 48;
// n direction: 3 fractals, actual value = 3 × 16 = 48
mmadParams.n = 48;
// k direction: 4 fractals, actual value = 4 × 16 = 64
mmadParams.k = 64;

// Feature parameters
// unitFlag: Controls fine-grained parallelism of Mmad and Fixpipe instructions
mmadParams.unitFlag = 0;
// disableGemv: Configures whether to enable GEMV mode when M=1
mmadParams.disableGemv = false;
// cmatrixInitVal: Initializes C cube to zero before computation
mmadParams.cmatrixInitVal = true;
// cmatrixSource: Does not initialize C cube from BiasTable Buffer
mmadParams.cmatrixSource = false;

AscendC::Mmad(c_matrix, a_matrix, b_matrix, mmadParams);
```

### Synchronization Mechanism

The internal execution units of the AI Core (such as the MTE2 move unit and the Vector compute unit) run in an asynchronous parallel manner, and data dependencies may exist when reading and writing the same storage resource. Therefore, cube computation must use pipeline synchronization APIs to ensure correct execution.  Cube computation mainly includes four steps: data move-in (Global Memory to L1 Buffer), data loading (L1 Buffer to L0A/B Buffer), cube computation (L0A/B Buffer to L0C Buffer), and data move-out (L0C Buffer to Global Memory). The four steps correspond to the PIPE_MTE2, PIPE_MTE1, PIPE_M, and PIPE_FIX pipelines respectively, and their execution order must be coordinated through intra-core synchronization APIs to ensure that each step is completed in order.

When a developer performs a cube multiplication (on products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md)), the four steps can be synchronized through `Mutex::Lock` and `Mutex::Unlock`. On products with [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), `SetFlag` and `WaitFlag` can be used to achieve the same synchronization effect. For details, see [Synchronization Mechanism](../cpp_tensor_programming/cpp_tensor_programming_overview.md#synchronization-mechanism).

In the following example, the pointer types of the kernel function input parameters `x`, `y`, and `z` are consistent with the template types of the corresponding `GlobalTensor`, so the explicit type conversion can be omitted when calling `SetGlobalBuffer`. If the actual argument pointer type is inconsistent with the template type of `GlobalTensor`, it must first be explicitly converted to the corresponding `__gm__ PrimType*` type.

```cpp
__global__ __cube__ void matmul_kernel(__gm__ half* x, __gm__ half* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    
    AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;
    
    AscendC::LocalTensor<half> al1ATensor = l1Allocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> bl1BTensor = l1Allocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<half> l0aTensor = l0aAllocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> l0bTensor = l0bAllocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<float> l0cTensor = l0cAllocator.Alloc<float, BLOCK_M * BLOCK_N>();
    
    AscendC::GlobalTensor<half> aGm, bGm;
    AscendC::GlobalTensor<float> cGm;
    aGm.SetGlobalBuffer(x);
    bGm.SetGlobalBuffer(y);
    cGm.SetGlobalBuffer(z);
    
    // Step 1: Data transfer in (Global Memory → L1 Buffer), pipeline is PIPE_MTE2
    AscendC::Mutex::Lock<PIPE_MTE2>(mutex_id);
    AscendC::Nd2NzParams nd2nzParamsA{
        1, BLOCK_M, BLOCK_K, 0, BLOCK_K, (BLOCK_M + 15) / 16 * 16, 1, 0};
    AscendC::DataCopy(al1ATensor, aGm, nd2nzParamsA);
    AscendC::Nd2NzParams nd2nzParamsB{
        1, BLOCK_K, BLOCK_N, 0, BLOCK_N, (BLOCK_K + 15) / 16 * 16, 1, 0};
    AscendC::DataCopy(bl1BTensor, bGm, nd2nzParamsB);
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutex_id);
    
    // Step 2: Data transfer in (L1 Buffer → L0A/B Buffer), pipeline is PIPE_MTE1
    AscendC::Mutex::Lock<PIPE_MTE1>(mutex_id);
    ...
    AscendC::LoadData(l0aTensor, al1ATensor, loadParams);
    AscendC::LoadData(l0bTensor, bl1BTensor, loadParams);
    AscendC::Mutex::Unlock<PIPE_MTE1>(mutex_id);
    
    // Step 3: Cube computation (L0A/B Buffer → L0C Buffer), pipeline is PIPE_M
    AscendC::Mutex::Lock<PIPE_M>(mutex_id);
    AscendC::MmadParams mmadParams;
    mmadParams.m = BLOCK_M;
    mmadParams.n = BLOCK_N;
    mmadParams.k = BLOCK_K;
    mmadParams.unitFlag = 0;
    mmadParams.cmatrixSource = false;
    mmadParams.cmatrixInitVal = true;
    AscendC::Mmad(l0cTensor, l0aTensor, l0bTensor, mmadParams);
    AscendC::Mutex::Unlock<PIPE_M>(mutex_id);
    
    // Step 4: Data transfer out (L0C Buffer → Global Memory), pipeline is PIPE_FIX
    AscendC::Mutex::Lock<PIPE_FIX>(mutex_id);
    ...
    AscendC::DataCopy(cGm, l0cTensor, l0c2gmParams);
    AscendC::Mutex::Unlock<PIPE_FIX>(mutex_id);
}
```

### Cube Computation Programming Example Based on Basic Tensor

By combining the move and computation capabilities of cube computation described above, developers can implement cube-type operators based on C++ Tensor programming. For specific development practices, see [matmul implementation sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_basic_api/README.md).

## Extended Tensor Programming Paradigm

[NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) introduces the Layout concept on top of Tensor, moving Shape and Stride to compile time to achieve type safety and automatic fractal layout generation. Developers only need to define the Layout of a Tensor, and the move and computation APIs automatically infer the underlying parameters, greatly reducing the difficulty of cube operator development.

> 📌 The extended Tensor (with Layout) capability is supported starting from [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).

> [!NOTE] Note
> The extended Tensor examples in this section use the Tensor API. For the Nz layout used in this section, the type template parameter of `asc::te::make_frame_layout` can be omitted as needed; the examples explicitly pass `half` to ensure that the Layout type and C0 deduction are consistent with the Tensor data type, not because the API adds a mandatory parameter. When using `asc::te::make_tensor` to wrap a raw pointer and when calling move APIs, follow these rules: the raw memory pointer must be wrapped through `asc::te::make_mem_ptr`, and passing a raw pointer directly to `make_tensor` is prohibited; when performing data movement, first construct a `copy_atom` object matching the movement path through `asc::te::make_copy`, and then complete the move in the form of `asc::te::copy(atom, dst, src)`.

### Hierarchical Expression of Layout

We use Shape and Stride to express the Layout format. Shape expresses the Layout shape, and Stride distinguishes different layouts. For example, the row-major and column-major layouts in the following figure:

- Row-major: Shape (2, 4) Stride (4, 1)
- Column-major: Shape (2, 4) Stride (1, 2)

The number in each cell in the figure indicates the subscript of the element at that position when elements are arranged sequentially in memory. For the same cube position, when the layout differs, the order in memory may differ. For example, for cube coordinate (1, 0), the corresponding elements are at positions 4 and 1 in memory under row-major and column-major layouts, respectively.

![Row-major layout](../../../../figures/row_major.png)

![Column-major layout](../../../../figures/col_major.png)

Usually, an element in Shape or Stride is a single positive integer (as in the preceding example). However, some complex memory layouts cannot be expressed this way. Therefore, a hierarchical expression is adopted, in which an element can also be a tuple. The first element of the tuple describes the number of rows of the cube at different levels, and the second element describes the number of columns of the cube at different levels. For example, in the following figure, the Shape and Stride of the Layout are:

- Shape((2, 3), (2, 4))
- Stride((1, 4), (2, 12))

![Hierarchical layout expression](../../../../figures/hier_layout.png)

The figure shows two levels of cubes: the inner cube is the cube enclosed by gray lines, and the outer cube is the cube enclosed by black lines when the inner cube is treated as a single element. The first element of Shape describes the shape in the row direction, and (2, 3) indicates that the numbers of rows of the inner cube and the outer cube are 2 and 3, respectively. The second element of Shape describes the shape in the column direction, and (2, 4) indicates that the numbers of columns of the inner cube and the outer cube are 2 and 4, respectively. Each element in Stride corresponds to an element in Shape, indicating the interval between the start addresses of adjacent elements in the memory address for the corresponding dimension. In the figure, arrows indicate the start address interval between adjacent elements in each dimension.

### Fractal Expression in Cube Computation

In cube programming scenarios based on Ascend C, the following common Layouts are used. These formats are all expressed using the hierarchical expression method described above, which requires two layers of cubes: an inner cube and an outer cube. Among them, the row layout can be regarded as a Z-shaped layout, and the column layout can be regarded as an N-shaped layout. The fractal name of a Layout describes the layout order from the outer layer to the inner layer from front to back. For example, the NZLayout format indicates that the outer layer uses an N-shaped layout and the inner layer uses a Z-shaped layout. The specific expression of a four-dimensional Layout is as follows:

```cpp
Layout = ((Shape): (Stride))
Shape = ((ShapeRow0, ShapeRow1), (ShapeColumn0, ShapeColumn1))
Stride = ((StrideRow0, StrideRow1), (StrideColumn0, StrideColumn1))
```

Take the Nz fractal as an example. In the inner fractal Shape of the Nz fractal format, ShapeRow0 is fixed to 16 rows, and ShapeColumn0 is fixed to 32Byte / sizeof(T). In the inner fractal Stride, StrideRow0 is fixed to 32Byte / sizeof(T), and StrideColumn0 is also fixed to 1. That is, the inner fractal is organized in a Z shape, and the outer fractal is organized in an N shape. ShapeRow1 must be 16-aligned in the row direction, and ShapeColumn1 must be 32B-aligned in the column direction.

![Nz data layout format](../../../../figures/nz_format.png)

The following is an example of a continuous Nz Layout, where C0_ELEMENT = Std::Int<32>{} / sizeof(T).

```cpp
Layout = ((Shape) : (Stride))
Shape = ((Std::Int<16>{}, ceil_div(row, Std::Int<16>{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
Stride = ((C0_ELEMENT, C0_ELEMENT * Std::Int<16>{}), (Std::Int<1>{}, C0_ELEMENT * ceil_align(row, Std::Int<16>{})))
```

### Layout Construction

Combined with the description of fractal expression above, and based on the capabilities of Layout, C++ Tensor programming provides the primitive Layout construction API. For example, when a multi-dimensional data layout with a shape of (128, 256, 128) needs to be created, it can be directly constructed through the `make_layout` API.

```cpp
auto three_dim_layout = asc::te::make_layout(asc::te::make_shape(128, 256, 128));
```

For an Nz fractal layout, a hierarchical Layout expression can also be constructed through `make_layout`. For example, for Nz data with an outer size of (128, 256) and a data type of `half`, a four-dimensional Layout can be constructed through `make_layout`:

```cpp
// Define Nz fractal with shape (128, 256), its 4D representation is: ((16, 128 / 16), (16, 256 / 16))
auto nz_layout = asc::te::make_layout(
    asc::te::make_shape(asc::te::make_shape(16, 8), asc::te::make_shape(16, 16)));
```

In cube computation, different internal storages each have their own unique fractal structure. To simplify user coding, C++ Tensor programming provides `make_frame_layout` to simplify the user-defined four-dimensional Layout definition shown above:

```cpp
// Use make_frame_layout to simplify Nz fractal definition, user only needs to perceive the fractal structure of corresponding storage, no need to directly express 4D structure
auto nz_layout = asc::te::make_frame_layout<asc::te::nz_layout_ptn, half>(128, 256);
```

### Tensor Expression Based on Layout

The extended Tensor embeds the Layout information described above into the Tensor structure, which consists of a data pointer and a data layout. A Tensor object describes both "where the data is placed" and "what the data looks like".
C++ Tensor provides the `make_tensor` function for constructing an extended Tensor structure with a Layout. For example, a Global Memory data block with a shape of (128, 256) and a data type of `half`, and an L1 Buffer data block of the same size (Nz fractal layout), can be expressed as:

```cpp
// Construct Layout for Global Memory & L1 Buffer 
auto global_layout = asc::te::make_layout(asc::te::make_shape(128, 256));
auto l1_layout = asc::te::make_frame_layout<asc::te::nz_layout_ptn, half>(128, 256);

// Construct global_tensor & local_tensor through memory pointer + Layout
auto global_tensor = asc::te::make_tensor(asc::te::make_mem_ptr(gm_ptr), global_layout);
auto l1_tensor = asc::te::make_tensor(asc::te::make_mem_ptr(l1_ptr), l1_layout);
```

The extended Tensor provides layout adjustment APIs such as `operator[]` element access, `operator()` sub-Tensor extraction, and `slice` slicing extraction, greatly simplifying the operation process of multidimensional data. Compared with the basic Tensor, which requires manually calculating offset addresses, stride parameters, and move lengths, the extended Tensor APIs can automatically derive these underlying parameters, so developers only need to focus on the data logic. This abstraction and encapsulation approach not only reduces programming complexity but also lowers the risk of parameter configuration errors, allowing developers to concentrate on operator performance optimization and algorithm implementation, thereby significantly improving development efficiency.

```cpp
// Construct tensor for Global Memory
auto global_layout = asc::te::make_layout(asc::te::make_shape(128, 256));
auto global_tensor = asc::te::make_tensor(asc::te::make_mem_ptr(gm_ptr), global_layout);

auto sub_tensor = global_tensor.slice(asc::te::make_coord(32, 64), asc::te::make_shape(32, 32));
```

### Cube Movement and Computation

The basic movement APIs require manually passing the `count` or `DataCopyParams` parameter to specify the movement length and movement mode. The extended Tensor carries Shape, Stride, and storage location information, allowing the compiler to automatically infer some movement parameters and physical paths, which simplifies API calls. It also provides the **Atom customization API**, with which developers can customize movement behavior (such as custom movement modes and in-transit processing) based on `copy_traits` to meet customization requirements in special scenarios.

- **GM2L1 movement**: `copy(atom, dst, src)` moves cube data to the L1 Buffer and supports in-transit format conversion (ND2NZ, DN2NZ, NZ2NZ, and so on). Developers define the Layout of the source Tensor and the destination Tensor, and use `make_copy` to construct the movement atom object from GM to the L1 Buffer. The API automatically infers the underlying movement parameters and format conversion method.

```cpp
// Construct GM-to-L1 copy_atom with the default trait
auto atom = asc::te::make_copy(asc::te::copy_gm_to_l1{}, asc::te::gm_to_l1_trait_default{});

// Copy equal-size tensors, and infer copy parameters from Layout
asc::te::copy(atom, l1_tensor, global_tensor);

// Specify Global Tensor offset to copy a slice from Global Tensor
asc::te::copy(atom, l1_bias_tensor,
    global_bias.slice(asc::te::make_coord(0, j * base_n), asc::te::make_shape(1, cur_n)));
```

- **L0C2GM move-out**: Based on `copy_traits`, in-transit processing (such as Fixpipe quantization) is customized to move the computation result in the L0C Buffer out to GM.

```cpp
// Fixpipe inline quantization example
float quant_value = 0.5f;
uint64_t quant_scalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quant_value));
auto atom = asc::te::make_copy(asc::te::copy_l0c_to_gm{}, asc::te::l0c_to_gm_trait_default{})
                .with(asc::te::l0c_to_gm_params{});
asc::te::copy(atom, global_c, l0c_tensor, quant_scalar);
```

Similarly, moving data from the L1 Buffer to the L0A/L0B Buffer requires constructing the `copy_atom` of the corresponding path, and cube computation requires constructing the `mmad_atom`. After developers pass in the movement or computation atom object and the operation Tensor, the API can infer the underlying parameters based on the Layout.

### Cube Computation Example Based on Extended Tensor

By combining the data movement and computation capabilities of cube computation described above, developers can implement cube operators based on C++ Tensor programming. For specific development practices, see the [matmul implementation sample](../../../../../../../examples/01_simd_cpp_api/07_tensor_api/matmul_tensor_api/README.md).
