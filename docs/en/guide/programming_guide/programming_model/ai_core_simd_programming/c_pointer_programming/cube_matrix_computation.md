# Cube Computation Programming<a name="ZH-CN_TOPIC_0000002600000002"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T11:06:05.100Z pushedAt=2026-11-19T06:32:19.964Z nl=2 -->

This document describes how to implement cube computation based on the Ascend C language extension APIs, relying on the L1 Buffer and L0 series on-chip caches. This approach is the most basic and general-purpose cube programming solution in Ascend C. The Cube unit in the AI Core is deployed on the AIC core and is responsible for cube computation. It has dedicated on-chip caches: the L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the initial accumulation values and cube computation results.

##  Core Steps for Cube Computation

Cube computation is divided into four stages: data move-in, data loading, cube computation, and data move-out. The complete process is as follows:
1. Use DMA to move data from Global Memory to the L1 Buffer.
2. Load the data in the L1 Buffer into the L0A Buffer and L0B Buffer.
3. Perform cube computation, and store the result in the L0C Buffer.
4. Use DMA to move the computation result from the L0C Buffer back to Global Memory.

## Cube Computation Memory Management

### Cube Computation Memory Allocation

Cube computation mainly relies on the L0 Buffer to complete operations. Developers need to first move the input data from Global Memory to the L1 Buffer, then load it into the L0 Buffer, and finally call the Cube computation instruction to complete the operation. Both the L1 Buffer and the L0 Buffer are declared in array form. This guide mainly introduces the static memory allocation method.

- Static allocation: Allocates a memory space of a specified size. The space size is determined at compile time and cannot be modified dynamically.

  ```c
  __global__ __cube__ void add_custom(...)
  {
      constexpr uint32_t m_size = 128;
      constexpr uint32_t n_size = 128;
      constexpr uint32_t k_size = 256;
      // Allocate L1A/B Buffer
      __cbuf__ half static_l1a_buffer[m_size * k_size];
      __cbuf__ half static_l1b_buffer[n_size * k_size];
      // Allocate L0A/B/C Buffer
      __ca__ half static_l0a_buffer[m_size * k_size];
      __cb__ half static_l0b_buffer[n_size * k_size];
      __cc__ half static_l0c_buffer[m_size * n_size];

      // ... Use Buffer for cube computation
  }
  ```

### Cube Computation Memory Layout

The Cube unit adopts a block-based computation logic. The minimum hardware computation granularity is the fractal block, and multiple fractals can be processed in parallel. The fractal block size depends on the data type: for the half type, the fractal shape is 16×16 (32B/sizeof(half)=16); for the int8 type, the fractal shape is 16×32 or 32×16 (32B/sizeof(int8)=32). In traditional linear storage modes such as row-major and column-major, reading a fractal block generates a large number of non-contiguous memory accesses, greatly reducing memory access efficiency.
To solve this problem, Ascend introduces the cube fractal storage format. The software layer rearranges the data so that each fractal block is stored contiguously in physical memory. A single hardware read can load the entire block, eliminating the overhead caused by scattered addressing and effectively improving data throughput.
The fractal granularity is the basic computation specification of the Cube unit. All products of [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) and [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) follow this specification, but the specific fractal shape varies with the data type. To maximize computing power, the Ascend Cube unit adopts storage rules different from those of general-purpose CPUs, and both input and output data must match the specified fractal format. When developing operators using the Ascend C Mmad API, mastering the various fractal formats is key to ensuring correct data movement and block-based logic as well as performance optimization.

For ease of understanding and memorization, this document adopts the intuitive "big Y small x" naming convention:
- Large Y (Z/N): indicates the arrangement order among fractal cubes (Z is row-major, N is column-major).
- Small x (z/n): indicates the arrangement order of elements within a fractal cube (z is row-major, n is column-major).

### Key Fractal Formats in Detail

The cube fractal format defines the layout rules of multidimensional tensors in memory. Affected by the hardware read logic, different product models have different format requirements for the cube multiplication C = A × B:

- For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the cube multiplication C = A × B requires that the left cube A use the Zz format, the right cube B use the Zn format, and the result cube C use the Nz format, as shown in Figure 1:

<img src="../../../../figures/matmul_data_2.png" alt="Data format designed for the cube multiplication scenario" width="920px" height="334px"/>

- For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the cube multiplication C = A × B requires that the left cube A use the Nz format, the right cube B use the Zn format, and the result cube C use the Nz format, as shown in Figure 2:

<img src="../../../../figures/matmul_data_1.png" alt="Data formats involved in the cube multiplication scenario" width="920px" height="334px"/>

#### ND Format (N-Dimension)

The ND format is a general N-dimensional tensor format in which data is stored contiguously and linearly in memory, typically in the `Global Memory (GM)`. Before cube computation, it usually needs to be converted to the Nz format to adapt to the Cube compute unit.

#### Nz Format

The Nz format is mainly used to store data in the L1 Buffer and L0C Buffer. For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the L0A Buffer also uses the Nz fractal format. This format uses large N (external column-major) + small z (internal row-major) for data layout.

<img src="../../../../figures/nz_format.png" alt="Nz data layout format" width="400"/>


Nz is an intermediate format of the Ascend architecture, between linear data (ND) and computation-specific data (Zz/Zn), which facilitates format conversion. The large N layout (column-major block) is designed to facilitate multi-core parallelism or the convenience of subsequent operators (such as Vector computation) when processing data in the Channel dimension.

> 📌 Application scenarios of the Nz format in different buffers:
> - **L0C Buffer**: Stores cube multiplication results. The fractal shape depends on the data type: 16×16 (256 elements) for half, and 16×32 or 32×16 for int8. It is suitable for efficient computation by the Cube compute unit.
> - **L1 Buffer**: Stores intermediate data. The fractal shape is 16×(32B/sizeof(DataType)), which facilitates conversion to the Zz and Zn formats. For the half type, the size is 16×16 = 256 elements (512 bytes); for the int8 type, the size is 16×32 or 32×16 = 512 elements (512 bytes).
> - **L0A Buffer ([NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) only)**: Stores the left cube.

#### Zz Format

For [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the L0A Buffer uses this format ([NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) uses the Nz format). This format uses large Z (external row-major order) + small z (internal row-major order).

<img src="../../../../figures/zz_half.png" alt="Zz format (using half as an example)" width="400"/>


In cube multiplication, cube A reads data row by row. The row-major order characteristic of the Zz format makes the physical addresses of elements in the same row contiguous, which matches the computation access pattern. This facilitates the left channel of the Cube unit to read data continuously through DMA, avoiding strided access and improving data movement efficiency.

#### Zn Format

The Zn format is mainly used to store data in the L0B buffer. If the offline-processed right cube data is directly in the Zn format, this format is stored in `Global Memory` and `L1 Buffer`. This format uses large Z (external row-major order) + small n (internal column-major order).

<img src="../../../../figures/zn_half.png" alt="Zn format (using half as an example)" width="300px" height="312px"/>


Matrix multiplication requires reading the column vector of the B cube. Ordinary row-major storage causes column data addresses to be non-contiguous. The Zn format stores data in column-major order within the fractal, so that column data is compactly arranged in L0B, facilitating the linear reading of complete column vectors by the right channel of the cube unit and avoiding strided access.

### Format Conversion

The original data in Global Memory is in the ND general linear format, while L1 Buffer requires the Nz format. Therefore, format conversion from ND to Nz must be completed during data movement.

<img src="../../../../figures/nd2nz_conv.png" alt="ND-to-NZ format conversion" width="920px" height="427px"/>

Using the preceding figure as an example, the figure shows the memory layout changes of a specific cube with M=40 and K=56 (data type: half) when it is moved from Global Memory (GM) to L1 Buffer and converted to the Nz format:
- Original data (ND format):
  The left GM stores the original cube with a shape of 40×56.
  The data is arranged linearly and contiguously (as shown by the long arrow in the figure), following the row-major rule.
- Alignment and padding:
  Due to the fractal requirement of the Cube unit, the target cube must be divisible by a 16×16 Block (assuming the data type is half, that is, 32/sizeof(half)=16).
  M-axis padding: The original M=40 is not divisible by 16 and must be padded upward to 48 (16×3).
  K-axis padding: The original K=56 is not divisible by 16 and must be padded upward to 64 (16×4).
  In the figure, the gray area is the invalid data generated by padding, and the white area is the original valid data.
- Relayout (Nz format):
  In the right L1 Buffer, the data is reorganized into 3×4 fractal blocks.
  Z-shaped flow: Within each 16×16 small block and between blocks, the data is stored in a specific Z-shaped order (as indicated by the polyline arrows in the figure), completing the physical mapping from "linear" to "fractal" and preparing the data for cube computation.

Ascend C programming directly provides C language programming APIs from `Global Memory` to `L1 Buffer`, through which data in `Global Memory` can be directly moved into `L1 Buffer` and the ND2NZ format conversion can be completed.
C language programming provides the `asc_set_gm2l1_nz_para` and `asc_copy_gm2l1_nd2nz` APIs to implement GM2L1 movement and complete the ND2NZ format conversion along the way. In addition, on [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the `asc_copy_gm2l1_dn2nz` capability is added to support format conversion movement from DN to Nz.

## Cube Movement Capabilities

### Cube Data Move-In

The cube data move-in API implements data transfer from Global Memory to the L0A/L0B Buffer, and is usually used in coordination with the Mmad API. This API supports multiple move modes: 2D move, 3D format conversion, compressed data decompression, cube transposition, sparse data processing, and so on.

Developers control the move path, start position, length, and other information by configuring parameters, fully leveraging hardware capabilities to maximize operator performance. Each data path corresponds to specific storage levels and access characteristics.

Based on the preceding data flow and fractalization knowledge, C language programming provides the following cube move-in capabilities (for the complete APIs, see the API Reference):

| Data Path | Function | C Language API | Fractalization Support |
|---------|---------|---------------|-------------|
| Global Memory → L1 Buffer | Move with on-the-fly ND2NZ conversion | `asc_copy_gm2l1_nd2nz` | <img src="../../../../figures/ND_Nz.png" alt="ND-&gt;Nz" width="300px" height="180px"> |
| L1 Buffer → L0A Buffer | Move data from the L1 Buffer to the L0A Buffer | `asc_copy_l12l0a` | <img src="../../../../figures/Nz_Zz.png" alt="Nz-&gt;Zz" width="300px" height="173px"><br>On [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the L0A fractal is Nz, and the move is Nz-Nz |
| L1 Buffer → L0B Buffer | Move data from the L1 Buffer to the L0B Buffer | `asc_copy_l12l0b` | <img src="../../../../figures/Nz_Zn.png" alt="Nz-&gt;Zn" width="300px" height="156px">|

[NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md)

#### GM2L1 On-the-Fly ND2NZ Movement

This capability is used to move a cube from Global Memory to the L1 Buffer and complete the ND-to-Nz format conversion synchronously during the move. It is mainly implemented through the `asc_copy_gm2l1_nd2nz` API. By configuring the corresponding parameters, data movement and format conversion can be completed.

<img src="../../../../figures/gm_l1_nd2nz.png" alt="ND2NZ move example from GM to L1" width="600px" height="176px"/>

Taking the preceding figure as an example, to move cube A (Shape(16, 23), data type half) from GM to L1, for [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), call the API as follows:

```c
// Allocate L1 Buffer space, m direction is 16, k direction 23 aligned up to 32
constexpr uint64_t m = 16;
constexpr uint64_t k = 32;
__cbuf__ half dst[m * k];

// Set parameters for dst cube (L1 Buffer) via asc_set_gm2l1_nz_para API
constexpr uint64_t dst_nz_matrix_stride = 0, dst_nz_c0_stride = 16, dst_nz_n_stride = 1, dst_nd_num = 1;
// Offset between starting addresses of adjacent Nz cubes in destination cube
constexpr uint64_t nz_config = static_cast<uint64_t>(dst_nz_matrix_stride) << 48;
// After ND conversion to Nz format, offset between starting addresses of adjacent Z cubes in destination Nz cube
nz_config |= static_cast<uint64_t>(dst_nz_c0_stride) << 32;
nz_config |= static_cast<uint64_t>(dst_nz_n_stride) << 16;
nz_config |= static_cast<uint64_t>(dst_nd_num);
asc_set_gm2l1_nz_para(nz_config);

// Number of elements contained in one row of source cube
constexpr uint64_t src_d_value = 32;
// Offset between starting addresses of adjacent ND cubes in source cube
constexpr uint64_t src_nd_matrix_stride = 0;
// L2 Cache uses NORMAL mode
constexpr asc_load_l2_cache_mode l2_cache_ctrl = asc_load_l2_cache_mode::NORMAL_LAST_VICTIM;
// Number of rows in ND cube of source cube
constexpr uint16_t n_value = 16;
// Number of columns in ND cube of source cube
constexpr uint32_t d_value = 23;
// When dValue is less than or equal to 4, can enable SmallC0 mode, C0_SIZE will pad to 4 * sizeof(T) Bytes. In this mode, dst_n_stride parameter and dst_c0_stride parameter are invalid, no need to configure.
constexpr bool enable_small_c0 = false;
asc_copy_gm2l1_nd2nz(dst, src, src_d_value, l2_cache_ctrl, n_value, d_value, src_nd_matrix_stride, enable_small_c0);
```

#### L12L0 Data Movement

This capability mainly implements moving a cube from the L1 Buffer to the L0A/L0B Buffer and supports conversion from the Nz fractal format to the Zz or Zn format. C language programming provides the `asc_copy_l12l0a` and `asc_copy_l12l0b` APIs. For [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the `asc_copy_l12l0a` API can move a 512B cube from the L1 Buffer to the L0A Buffer, supporting 2D move, transposed move, and 3D move modes.

<img src="../../../../figures/l1_l0a_copy.png" alt="L12L0A non-transposed move" width="920px" height="433px"/>

The move-in operation from the `L1 Buffer` to the `L0A Buffer` described above can be completed by calling the API as follows.

```c
__cbuf__ half l1_buffer[6400];
__ca__ half l0a_buffer[2304];
// m_step is 2, k_step is 3, indicating transfer of 6 data blocks
uint8_t m_step = 2;
uint8_t k_step = 3;
// mStartPosition is 2, indicates M direction transfer starting position is at M axis direction 16*2=64 elements away from source operand starting position
uint16_t m_start_position = 2;
// kStartPosition is 2, indicates K direction transfer starting position is at K axis direction 32B*2=64B position away from source operand starting position
uint16_t k_start_position = 2;
// srcStride is 5, indicates in source operand, interval between starting addresses of previous fractal and next fractal in K direction is 5
int16_t src_stride = 5;
// dstStride is 3: indicates in destination operand, interval between starting addresses of previous fractal and next fractal in K direction is 3
int16_t dst_stride = 3;
asc_copy_l12l0a(l0a_buffer, l1_buffer, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);
```

Moving data to `L0B Buffer` is implemented by `asc_copy_l12l0b`. The API parameters and implemented functions are the same as those of L12L0A. For more information, see [Cube Computation Data Movement](../../../../../api/SIMD-API/c_api/cube_datamove/cube_datamove.md).

When the input A cube (Global Memory) is in transposed layout (K × M), it is moved into `L1 Buffer` as Nz fractal in (K × N) layout. To make `L0A Buffer` store the Zz or Nz format of (M × K) ([NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md)), transposition is required during L12L0A, by calling the `asc_copy_l12l0a_transpose` API.

<img src="../../../../figures/need_transpose.png" alt="Processing scenario requiring transposition" width="800"/>

```c
// Use transpose API to complete processing
asc_copy_l12l0a_transpose(l0a_buffer, l1_buffer, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);
```

### Cube Data Move-Out

The cube move-out API implements data transfer from L0C to Global Memory or L1 Buffer, and supports operations such as in-path quantization, ReLU, format conversion (NZ2ND), and channel split/merge. Developers control the movement path, position, and length through configuration parameters to fully leverage the hardware capability. It is usually used after the Mmad API. For [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md), it also supports transfer from L0C to Unified Buffer (UB), NZ2DN format conversion, and UB dual-destination mode. The cube move-out capability is shown in the following figure:

For [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md):

<img src="../../../../figures/mat_copy_out_a2a3.png" alt="Matrix move-out process" width="800"/>

For [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md):

<img src="../../../../figures/mat_copy_out_950.png" alt="Matrix move-out process" width="920px" height="313px"/>

The cube move-out API supports flexible combinations of multiple in-path capabilities. Take the movement scenario from L0C to Global Memory as an example: after the cube computation result is temporarily stored in L0C Buffer, it needs to be moved to GM. During this process, operations such as in-path quantization, ReLU activation, format conversion, and channel split/merge can be executed collaboratively. The following figure shows the valid combinations of these in-path capabilities, the intermediate data types, and the complete data path. For example, F32 data in L0C can be quantized in-path to B8 data through QF322B8_PRE, and after optionally enabling ReLU, it is output to GM through NZ2NZ/NZ2ND format conversion. In the figure, F32→F16 and F32→BF16 are data type conversions, and the remaining paths are in-path scalar/vector quantization modes. For [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md), NZ2DN format conversion is additionally supported.

For [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md):

<img src="../../../../figures/l0c_gm_a2a3.png" alt="L0C2GM move-out" width="800"/>

For [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md):

<img src="../../../../figures/l0c_gm_950.png" alt="L0C2GM move-out" width="920px" height="676px"/>

The C language programming provides `asc_copy_l0c2gm` to enable the various in-line capabilities of the chip. By directly passing configuration parameters, you can complete the in-line movement operation.

```c
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre);

```

In addition, for the `unit_flag_mode` and `enable_nz2nd` parameters, you need to configure the corresponding register parameters to enable different functions. The corresponding APIs and processing capabilities are as follows:

```c
// 1.0 When performing inline quantization during data movement, call this API to set vector quantization parameters in the quantization flow.
__aicore__ inline void asc_set_l0c_copy_config(
    uint64_t relu_pre_addr, uint64_t quant_pre_addr, bool is_clean_unit_flag);

// 2.0 When performing inline format conversion (NZ to ND) during data movement, call this API to set format conversion configuration.
__aicore__ inline void asc_set_l0c_copy_nz_para(
    uint16_t matrix_num, uint16_t src_nz_matrix_stride, uint32_t dst_matrix_stride);

// 3.0 Set the value in RELU_ALPHA register. This is a 64-bit register storing the alpha value used in Scalar ReLU during fixpipe or cube instructions.
__aicore__ inline void asc_set_l0c_copy_lrelu_alpha(float scalar_relu_pre_alpha);
```

For details, see the API description document [Cube API Description](../../../../../api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm.md).


## Cube Computation Capabilities

`asc_mmad` is the core cube multiply-accumulate API through which Ascend C encapsulates the NPU hardware capability. It is widely used in operators such as fully connected layers and convolution to implement the C = A × B + C operation. The computation inputs are read from the L0A Buffer and L0B Buffer, and the result is written to the L0C Buffer. The cache fractal formats corresponding to different products have been described earlier.

![Schematic diagram of the 950 cube computation fractal format](../../../../figures/mmad_950.png)

The following table describes cubes A, B, and C in cube computation ([NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md)).

| Cube Computation Logic | Physical Location of Cube Computation | Dimension | Input/Output Data Format | Data Type |
|-------------|-----------------|------|------------------|---------|
| A | L0A Buffer | M × K | Nz | Data type |
| B | L0B Buffer | K × N | Zn | Data type |
| C | L0C Buffer | M × N (initialization with the bias cube is supported, with a dimension of 1 × N) | Nz | Data type |

C language programming provides a series of overloaded `asc_mmad` APIs that support cube computation of different data types. The following code implements the cube multiplication computation shown in the preceding figure.

```c
// 1. Define L0A/L0B cube sizes
constexpr uint32_t l0_m_size = 64;
constexpr uint32_t l0_k_size = 64;
constexpr uint32_t l0_n_size = 64;
__ca__ a_matrix[l0_m_size * l0_k_size];
__cb__ b_matrix[l0_k_size * l0_n_size];
__cc__ c_matrix[l0_m_size * l0_n_size];

// 2. Configure asc_mmad parameters
// 2.1. Actual cube sizes participating in computation (m, n, k)
// m direction: 3 fractals, actual value = 3 × 16 = 48
constexpr uint16_t m = 48;
// n direction: 3 fractals, actual value = 3 × 16 = 48
constexpr uint16_t n = 48;
// k direction: 4 fractals, actual value = 4 × 16 = 64
constexpr uint16_t k = 64;

// 2.2 Feature parameters
// unit_flag: Controls fine-grained parallelism of Mmad and Fixpipe instructions. When enabled, results are transferred out immediately after computing each fractal.
asc_unit_flag_mode unit_flag = asc_unit_flag_mode::DISABLE;
// disable_gemv: Configures whether to enable GEMV mode when M=1
bool disable_gemv = false;
// enable_cube_init: Enables C cube initial value loading. false: do not initialize L0C; true: load initial value from BT Buffer.
bool enable_cube_init = false;
// enable_cube_zero_init: Enables C cube default zero initialization. true: initialize to 0; false: control initialization via enable_cube_init parameter.
bool enable_cube_zero_init = true;

asc_mmad(c_matrix, a_matrix, b_matrix, m, n, k, unit_flag, disable_gemv, enable_cube_init, enable_cube_zero_init);
```

## Synchronization of Cube Computation

The data movement and compute units inside the AI Core work asynchronously and in parallel. When multiple units access the same storage resource, data dependencies arise. Therefore, cube computation must use pipeline synchronization APIs to control the execution sequence. The four stages of cube computation correspond to four pipelines: PIPE_MTE2, PIPE_MTE1, PIPE_M, and PIPE_MTE3. On-core synchronization APIs are required to ensure serial execution of the process. On products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the `asc_lock` and `asc_unlock` APIs can be used to implement full-process synchronization control.

```c
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    ...
    // 1. Step 1: Data transfer in (Global Memory → L1 Buffer), execution pipeline is PIPE_MTE2
    asc_lock(PIPE_MTE2, mutex_id);
    asc_copy_gm2l1_nd2nz(a_l1_buf, a_gm, 128, asc_load_l2_cache_mode::NORMAL_LAST_VICTIM, BLOCK_M, BLOCK_K, 0, false);
    asc_copy_gm2l1_nd2nz(b_l1_buf, b_gm, 64, asc_load_l2_cache_mode::NORMAL_LAST_VICTIM, BLOCK_K, BLOCK_N, 0, false);
    asc_unlock(PIPE_MTE2, mutex_id);
    ...
    // 2. Step 2: Data transfer in (L1 Buffer → L0A/B Buffer), execution pipeline is PIPE_MTE1
    asc_lock(PIPE_MTE1, mutex_id);
    asc_copy_l12l0a(l0a_buf, a_l1_buf, 0, 0, 4, 8, 1, 1);
    asc_copy_l12l0b_transpose(l0b_buf, b_l1_buf, 0, 0, 4, 8, 1, 1);
    asc_unlock(PIPE_MTE1, mutex_id);
    ...
    // 3. Step 3: Cube computation (L0A/B Buffer → L0C Buffer), execution pipeline is PIPE_M
    asc_lock(PIPE_M, mutex_id);
    asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, asc_unit_flag_mode::DISABLE, false, false, false);
    asc_unlock(PIPE_M, mutex_id);
    ...
    // 4. Step 4: Data transfer out (L0C Buffer → Global Memory), execution pipeline is PIPE_MTE3
    asc_lock(PIPE_MTE3, mutex_id);
    asc_copy_l0c2gm(c_gm, l0c_buf, BLOCK_N, BLOCK_M, BLOCK_N, 16,
        asc_store_l2_cache_mode::NORMAL_LAST_VICTIM, asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant,
        asc_relu_pre_mode::NONE, false, true, false, false);
    asc_unlock(PIPE_MTE3, mutex_id);
}
```

## Cube Computation Example

A complete cube computation engineering sample is still under development and will be supplemented in later versions. Developers can refer to the API documentation for [detailed cube API descriptions](../../../../../api/SIMD-API/c_api/c_api.md).
