# Multi-Core Matmul Computation Based on the SIMD C API

## Overview

This sample implements multi-core matrix multiplication computation based on the SIMD C API programming paradigm.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|-----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── matmul_multi_core
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and golden data
│   │   └── verify_result.py    // Golden value comparison file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read and write functions
│   ├── matmul_multi_core.asc   // SIMD C API sample implementation & invocation sample
│   └── README.md               // Sample documentation
```

## Sample Description

- Sample Functionality:  
  This sample uses the SIMD C API to implement a basic matrix multiplication (Matmul) [kernel function](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md). The matrix multiplication formula is as follows:
  $$
  C = A * B
  $$
  Where matrix A has shape `[M, K]`, matrix B has shape `[K, N]`, and output matrix C has shape `[M, N]`. For each element `C[m, n]` in the output matrix C, the product of row `m` of matrix A and column `n` of matrix B along the K axis accumulates. In matrix multiplication, **M direction** refers to the row direction of matrix C, **N direction** refers to the column direction of matrix C, and **K direction** refers to the inner dimension (accumulation dimension) of matrix C multiplication.

- Sample Specifications:  
  This sample uses parameters `M = 256, N = 256, K = 64`, with both input and output in `half` type and [`ND`](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) format. The sample launches 2 cores to complete the computation, with each core responsible for 128 rows in the M axis direction and all 256 columns in the N axis direction of the output matrix C:
  - Core 0 computes rows `0~127` of matrix C.
  - Core 1 computes rows `128~255` of matrix C.

  The input and output specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_multi_core_custom</td></tr>
  </table>

- Sample Implementation:
  - Kernel-side Overall Approach
    - `matmul_multi_core_custom` is a [`__global__`](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md) [`__cube__`](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md) kernel function, which indicates that this function runs on the [Cube](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md) computation unit of [AI Core](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md), primarily used for matrix computation.
    - The sample uses the SIMD C API programming method and declares data in GM, L1 Buffer, L0A Buffer, L0B Buffer, and L0C Buffer through the `__gm__`, `__cbuf__`, `__ca__`, `__cb__`, and `__cc__` address-space qualifiers.
    - `CUBE_BLOCK = 16` indicates that the half data type fractal is `16 x 16`, and the code calls `asc_copy_l12l0a` and `asc_copy_l12l0b_transpose` to transfer data in units of `16 x 16` fractals.

  - Kernel-side Detailed Process
    - The kernel arguments `a`, `b`, and `c` are addresses of matrices A, B, and C in [GM (Global Memory)](../../../../../docs/zh/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md). On the kernel side, they are converted to `__gm__ half*` variables `aGM`, `bGM`, and `cGM`, respectively.
    - Obtain the current core ID through the built-in variable `block_idx` and calculate `mIterIdx`. This sample only splits tasks along the M axis, so each core only needs to process its own M-axis slice of matrix A and matrix C.
    - Set GM address offsets:
      - Offset `aGM` by `mIterIdx * singleCoreM * K`, enabling the current core to read its assigned row block of matrix A.
      - Do not offset `bGM`, as each core needs to read the complete matrix B.
      - Offset `cGM` by `mIterIdx * singleCoreM * N`, enabling the current core to write results back to its assigned row block of matrix C.
    - Declare on-chip arrays through address-space qualifiers:
      - `a1Local`: Temporary storage of matrix A in L1 Buffer.
      - `a2Local`: Temporary storage of matrix A in L0A Buffer, for `asc_mmad` to read.
      - `b1Local`: Temporary storage of matrix B in L1 Buffer.
      - `b2Local`: Temporary storage of matrix B in L0B Buffer, for `asc_mmad` to read.
      - `cLocal`: Temporary storage of the matrix multiplication result in L0C Buffer.
    - Call [`asc_set_gm2l1_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_nz_para.md) to configure the destination Nz layout, and then call [`asc_copy_gm2l1_nd2nz`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md) to transfer matrices A and B from GM to L1 Buffer. During the transfer, the input data is converted from [ND](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) format to the [Nz](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) format required by Cube computation.
    - Call [`asc_sync_notify`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md) and [`asc_sync_wait`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md) for synchronization. `asc_copy_gm2l1_nd2nz` belongs to the MTE2 pipeline, and the subsequent L1 Buffer-to-L0A Buffer/L0B Buffer transfer belongs to the [MTE1](../../../../../docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md) pipeline. MTE1 must wait for MTE2 to complete, to avoid reading L1 Buffer data that has not finished transferring.
    - Call [`asc_copy_l12l0a`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) to transfer matrix A from L1 Buffer to L0A Buffer, and call [`asc_copy_l12l0b_transpose`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md) to transfer and transpose matrix B from L1 Buffer to L0B Buffer. L0A Buffer and L0B Buffer are input buffers read directly by the Cube matrix computation unit.
    - Call `asc_sync_notify` and `asc_sync_wait` for synchronization. The L1 Buffer-to-L0A Buffer/L0B Buffer transfers belong to the MTE1 pipeline, and the subsequent `asc_mmad` belongs to the `PIPE_M` pipeline. `PIPE_M` must wait for MTE1 to complete, to avoid reading L0A Buffer/L0B Buffer data that has not finished transferring.
    - Call [`asc_mmad`](../../../../../docs/zh/api/SIMD-API/c_api/cube_compute/asc_mmad.md)`(cLocal, a2Local, b2Local, baseM, baseK, baseN, asc_unit_flag_mode::DISABLE, false, false, true)` to execute matrix multiplication. Here `baseM = 128`, `baseN = 256`, and `baseK = 64`, corresponding to the matrix block size computed by a single core at one time.
    - Call `asc_sync_notify` and `asc_sync_wait` for synchronization. `asc_mmad` belongs to the `PIPE_M` pipeline, and the subsequent [`asc_copy_l0c2gm`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md) belongs to the FIX pipeline. The FIX pipeline must wait for `PIPE_M` to complete, to avoid reading L0C Buffer results that have not finished computing.
    - Call [`asc_set_l0c_copy_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_nz_para.md) to configure a single-matrix Nz-to-ND transfer, and then call `asc_copy_l0c2gm` to convert the `float` accumulation result in L0C Buffer to `half` and transfer it back to the matrix C output location in GM.
    - Finally, call [`asc_sync_pipe`](../../../../../docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md)`(PIPE_ALL)` to ensure that related pipeline tasks within the current core complete.

  - Invocation Implementation  
    Use the [kernel invocation operator](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md)`<<<>>>` to invoke the [kernel function](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/kernel_function.md). When invoking, pass matrix specifications, single-core computation amount, and basic tile size as template parameters, and pass Device-side A, B, C matrix addresses as runtime parameters.

- API Parameter Description:

  **[`asc_set_gm2l1_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_nz_para.md) and [`asc_copy_gm2l1_nd2nz`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1_nd2nz/asc_copy_gm2l1_nd2nz_arch_3510.md)** — Configure the destination Nz layout and perform GM-to-L1 Buffer ND-to-Nz format conversion:
  ```cpp
  asc_set_gm2l1_nz_para(
      matrix_num,             // Number of ND matrices in the source operand
      dst_nz_n_stride,        // Starting address offset between adjacent rows in destination Nz, unit: 32B
      dst_nz_c0_stride,       // Starting address offset between adjacent Z fractals in destination Nz, unit: 32B
      dst_nz_matrix_stride);  // Starting address offset between adjacent destination Nz matrices, unit: 32B

  asc_copy_gm2l1_nd2nz(
      dst,                    // Destination L1 Buffer address
      src,                    // Source GM address
      loop1_src_stride,       // Starting address offset between adjacent source rows, unit: byte
      l2_cache_ctl,           // L2 Cache management policy
      n_value,                // Number of rows in the ND matrix, unit: element
      d_value,                // Number of columns in the ND matrix, unit: element
      loop4_src_stride,       // Starting address offset between adjacent ND matrices, unit: byte
      enable_small_c0);       // Whether to enable SmallC0 mode
  ```
  For example, when transferring matrix A, first call `asc_set_gm2l1_nz_para(1, 1, baseM, 0)`, and then call `asc_copy_gm2l1_nd2nz(a1Local, aGM, K * sizeof(half), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, baseM, baseK, 0, false)` to convert baseM×baseK ND data to Nz format.

  **[`asc_copy_l12l0a`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md) and [`asc_copy_l12l0b_transpose`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)** — Describe the parameters for transferring matrix A from L1 Buffer to L0A Buffer and matrix B from L1 Buffer to L0B Buffer on Ascend 950PR/Ascend 950DT products:
  ```cpp
  asc_copy_l12l0a/asc_copy_l12l0b_transpose(
      dst,               // Destination L0A Buffer/L0B Buffer address
      src,               // Source L1 Buffer address
      m_start_position,  // Start fractal position in the M direction, unit: 16 elements
      k_start_position,  // Start fractal position in the K direction, unit: 32B
      m_step,            // Number of fractal rows transferred in the M direction
      k_step,            // Number of fractal columns transferred in the K direction
      src_stride,        // Source start address interval between adjacent K-direction fractals, unit: 512B
      dst_stride);       // Destination start address interval between adjacent K-direction fractals, unit: 512B
  ```
  On Ascend 950PR/Ascend 950DT products, the layout format in L0A Buffer is Nz. When transferring matrix A, use `asc_copy_l12l0a(a2Local, a1Local, 0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK)` to complete the A matrix Nz-to-Nz transfer in one operation. When transferring matrix B, use `asc_copy_l12l0b_transpose(b2Local, b1Local, 0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK)` to complete the B matrix Nz-to-Zn transfer in one operation.

  **[`asc_mmad`](../../../../../docs/zh/api/SIMD-API/c_api/cube_compute/asc_mmad.md)** — Describes matrix multiplication parameters:
  ```cpp
  asc_mmad(
      c_matrix,          // Starting address of result matrix C in L0C Buffer
      a_matrix,          // Starting address of left matrix A in L0A Buffer
      b_matrix,          // Starting address of right matrix B in L0B Buffer
      m,                 // Left matrix Height (M dimension), [0, 4095]
      k,                 // Left matrix Width/right matrix Height (K dimension), [0, 4095]
      n,                 // Right matrix Width (N dimension), [0, 4095]
      unit_flag_mode,    // Fine-grained parallelism control for asc_mmad and matrix transfer-out instructions
      disable_gemv,      // Whether to disable GEMV mode when M is 1
      c_matrix_source,   // C matrix initial value source, false=L0C Buffer, true=BiasTable
      c_matrix_init_val);// Whether to set the initial value of matrix C to 0
  ```
  This sample uses `asc_mmad(cLocal, a2Local, b2Local, baseM, baseK, baseN, asc_unit_flag_mode::DISABLE, false, false, true)` to compute a baseM×baseN output block and accumulate baseK elements in the K direction, clearing the initial value of matrix C before computation.

  **[`asc_set_l0c_copy_nz_para`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_set_l0c_copy_nz_para.md) and [`asc_copy_l0c2gm`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md)** — Configure Nz matrix conversion parameters and perform L0C Buffer-to-GM data transfer and precision conversion:
  ```cpp
  asc_set_l0c_copy_nz_para(
      matrix_num,          // Number of source Nz matrices
      src_matrix_stride,   // Starting address offset between adjacent source Nz matrices
      dst_matrix_stride);  // Starting address offset between adjacent destination matrices

  asc_copy_l0c2gm(
      dst,                   // Destination GM address
      src,                   // Source L0C Buffer address
      n_size,                // Source Nz matrix size in the N direction
      m_size,                // Source Nz matrix size in the M direction
      dst_stride,            // Number of elements per row in the destination ND matrix
      src_stride,            // Starting address offset between adjacent Z layouts in source Nz, unit: 64B
      l2_cache_mode,         // L2 Cache management policy for output GM data
      unit_flag_mode,        // Fine-grained parallelism control for asc_mmad and matrix transfer-out instructions
      quant_pre_mode,        // Preprocessing quantization mode; F322F16 indicates float-to-half
      relu_pre_mode,         // Preprocessing ReLU mode
      enable_channel_split,  // Whether to enable ChannelSplit
      enable_nz2nd,          // Whether to enable Nz-to-ND conversion
      enable_nz2dn,          // Whether to enable Nz-to-DN conversion
      enable_clip_relu_pre); // Whether to enable Clip ReLU in preprocessing
  ```
  This sample first calls `asc_set_l0c_copy_nz_para(1, 0, 0)` to configure a single-matrix transfer, and then calls `asc_copy_l0c2gm(cGM, cLocal, baseN, baseM, N, baseM, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, asc_unit_flag_mode::DISABLE, QuantMode_t::F322F16, asc_relu_pre_mode::NONE, false, true, false, false)` to convert the baseM×baseN float32 result in L0C Buffer to half, convert it from Nz to ND, and write it back to GM.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and run the sample.
- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default installation path is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.
- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program to run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output result correctness, confirm algorithm logic is correct
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, clear the cmake cache. Execute `rm CMakeCache.txt` in the build directory and run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture for Ascend 950PR/Ascend 950DT |

- Execution Result  
  The execution result is as follows, indicating successful precision comparison.
  ```bash
  test pass!
  ```
