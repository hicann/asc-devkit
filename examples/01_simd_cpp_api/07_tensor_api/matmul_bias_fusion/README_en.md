# Matmul Bias CV Fusion Example Based on Tensor API

## Overview

This example implements Cube and Vector CV-fused computation with the Tensor API programming model. The Cube side performs matrix multiplication with Bias, and the Vector side adds the Residual element by element to the Cube result:

$$
Y = A \times B + Bias + Residual
$$

The example uses one `__mix__(1, 2)` kernel to start one AIC and two AIVs. Cube transfers the L0C result to two Vector sub-blocks along the M dimension by using `dual_dst_mode::split_m` of `copy_l0c_to_ub`. Each Vector sub-block adds its corresponding Residual data and writes the result back to Global Memory. This demonstrates CV data exchange and pipeline coordination in one kernel.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

> **Note:** This example depends on CANN features that have not been officially released. Use the latest CANN master package.

## Directory Structure

```text
├── matmul_bias_fusion
│   ├── scripts
│   │   ├── gen_data.py                    // Input data and float32 ground-truth generation script
│   │   └── verify_result.py               // Output and ground-truth comparison script
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── matmul_bias_fusion.asc             // Ascend C sample implementation and invocation example
│   └── README.md                          // Example description document
```

## Example Description

- Example functionality:

  1. **Cube-side Matmul and Bias fusion**

     The Cube side transfers A and B from Global Memory to L1, and then to L0A/L0B, before using `mmad` for matrix multiplication. Bias is transferred from Global Memory through L1 and the Bias table to L0Bias. It is passed to only the first `mmad` of each output tile; subsequent K iterations accumulate the matrix multiplication result without adding Bias again.

  2. **Vector-side Residual addition**

     Cube transfers the `float` result in L0C to UB. `dual_dst_mode::split_m` divides one tile evenly along the M dimension and sends the two halves to the two AIVs. Each AIV reads its half of the Cube result and the corresponding Residual data from Global Memory, calls `ComputeAdd` for element-wise addition, and writes the result back to Global Memory.

  3. **CV pipeline coordination**

     Two UB slots (`PIPE_DEPTH=2`) are reused alternately for output tiles. Before reusing a slot, AIC waits for both AIVs to finish consuming the previous tile. After the L0C-to-UB transfer, AIC notifies the AIVs. After both AIVs finish the addition and UB-to-Global Memory transfer, they notify AIC. `asc_sync_block_wait` and `asc_sync_block_arrive` implement the AIC/AIV synchronization, so the Vector path does not require a separate kernel.

  4. **Computation formula**

     Bias is broadcast along the column dimension, and Residual is added element by element:

     $$
     T_{m,n}=\sum_{k=0}^{K-1} A_{m,k}B_{k,n}+Bias_n
     $$
     $$
     Y_{m,n}=T_{m,n}+Residual_{m,n}
     $$

- `ComputeAdd` implementation:

  `ComputeAdd` is a Vector function marked with `__simd_vf__`. Both its inputs and output are one-dimensional UB Tensors. The function first creates a valid-element mask with `update_mask<float>(length)`, then performs masked `load` operations on the two input Tensors, computes `lhs + rhs`, and finally writes the result with `store`. `length` is the number of valid elements in the current tail block, so padding elements in a tile are not accessed.

  ```cpp
  template <typename Src0Tensor, typename Src1Tensor, typename DstTensor>
  __simd_vf__ inline void ComputeAdd(Src0Tensor src0, Src1Tensor src1,
      DstTensor dst, uint32_t length)
  {
      const auto coord = asc::te::make_coord(0);
      auto mask = asc::te::experimental::update_mask<float>(length);
      auto lhs = asc::te::experimental::load(src0, coord).with_mask(mask);
      auto rhs = asc::te::experimental::load(src1, coord).with_mask(mask);
      asc::te::experimental::store(dst, coord, lhs + rhs);
  }
  ```

- Example specifications:

  The default shape is `M=1024`, `N=1024`, and `K=256`. Each Cube block processes an output region of `singleCoreM=256` by `singleCoreN=128`, and 32 mixed blocks are launched.

  <table border="2">
  <caption>Example Specifications</caption>
  <tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="left">Matmul with Bias and Residual</td></tr>
  <tr><td rowspan="6" align="left">Example Input</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">A (Matrix A)</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">B (Matrix B)</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">Bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">Residual</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling structure</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Example Output</td><td align="left">Y (Output matrix)</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="left">matmul_bias_fusion</td></tr>
  </table>

  The default compile-time parameter values are as follows:

  | Parameter | Default | Description |
  | :--- | :---: | :--- |
  | `BASE_M` | 128 | M dimension of a Cube output tile |
  | `BASE_N` | 128 | N dimension of a Cube output tile |
  | `BASE_K` | 64 | Basic block size of the Cube K dimension |
  | `STEP_M` | 1 | L1 M dimension step |
  | `STEP_N` | 1 | L1 N dimension step |
  | `STEP_K` | 4 | L1 K dimension step |
  | `VECTOR_LEN` | 64 | Maximum number of elements in one Vector addition |
  | `PIPE_DEPTH` | 2 | Number of UB slots |

  The default runtime tiling parameter values are as follows:

  | Parameter | Default | Description |
  | :--- | :---: | :--- |
  | `m` | 1024 | Number of rows of matrix A |
  | `n` | 1024 | Number of columns of matrix B |
  | `k` | 256 | Number of columns of matrix A / number of rows of matrix B |
  | `singleCoreM` | 256 | M dimension processed by one Cube block |
  | `singleCoreN` | 128 | N dimension processed by one Cube block |
  | `singleCoreK` | 256 | K dimension processed by one Cube block |

## Example Implementation

### Tensor Layouts and Data Movement

| Tensor | Layout | Purpose |
| :--- | :--- | :--- |
| A, B, Bias, Residual, and Y in Global Memory | `nd_ext_layout_ptn` (ND) | Describe input and output data in Global Memory |
| A and B in L1 | `nz_layout_ptn` (NZ) | Prepare data for Cube transfer and computation |
| Bias in L1 | `nd_ext_layout_ptn` (ND) | Temporarily store the Bias vector |
| L0A | `nz_layout_ptn` (NZ) | Cube matrix A |
| L0B | `zn_layout_ptn` (ZN) | Cube matrix B |
| L0C | `nz_layout_ptn` (NZ) | Float matrix multiplication accumulation result |
| Tile and Vector temporary Tensors in UB | ND/one-dimensional layout | AIC-to-AIV data handoff and Vector computation |

### Implementation Process

| Step | Tensor API/C API operation | Function | Layout or execution unit |
| :--- | :--- | :--- | :--- |
| 1 | `make_tensor`, `make_mem_ptr`, `make_frame_layout` | Create GM, L1, L0, and UB Tensor views, and obtain the current M/N region from the block index | GM uses ND; Cube buffers use NZ/ZN |
| 2 | `copy(copy_gm_to_l1)` | Transfer A, B, and Bias to L1 | A/B: ND to NZ; Bias remains ND |
| 3 | `copy(copy_l1_to_l0a/copy_l1_to_l0b/copy_l1_to_biastable)` | Transfer Cube inputs and Bias to L0A, L0B, and L0Bias | Prepare Cube computation |
| 4 | `mmad` | Complete matrix multiply-accumulate along K and inject Bias in the first iteration | AIC/Cube |
| 5 | `copy(copy_l0c_to_ub)` | Split the L0C tile into two M halves with `dual_dst_mode::split_m` | Transfer the L0C result to the UB of the two AIVs |
| 6 | `asc_sync_block_wait/arrive` | Pass ready and consumed events for each slot between AIC and the two AIVs | CV synchronization |
| 7 | `copy(copy_ub_to_ub)`, `copy(copy_gm_to_ub)` | Read the Cube half-tile and Residual into temporary Vector UB buffers | AIV/Vector |
| 8 | `ComputeAdd` | Perform masked load, element-wise addition, and store for valid elements | AIV/Vector |
| 9 | `copy(copy_ub_to_gm)` | Write the addition result to the corresponding Global Memory region of Y | AIV to Global Memory |

### Cube-side Matmul Flow

Each AIC block uses `block_idx` to locate a `singleCoreM` by `singleCoreN` region and divides that region into output tiles using `BASE_M` and `BASE_N`. For each tile, A and B are transferred along the K dimension in `BASE_K` blocks and passed to `mmad`. The first call sets `init_with_zero=true` and passes L0Bias; subsequent calls set `init_with_zero=false` and accumulate the matrix multiplication result. The resulting L0C tile is therefore `A x B + Bias`.

### Vector-side Addition Flow

Each AIV uses `asc_get_sub_block_id()` to determine its M half. After receiving the notification for the corresponding AIC slot, it processes each row and each `VECTOR_LEN`-element column block. It first copies the Cube result from UB to a Vector temporary Tensor, then transfers Residual from Global Memory to UB, calls `ComputeAdd`, and finally copies the output Tensor to the corresponding Y slice in Global Memory. The two AIVs process non-overlapping row ranges.

### Dual-destination L0C-to-UB Transfer and Synchronization

`copy_l0c_to_ub` uses the following trait:

```cpp
{ asc::te::round_mode::default_round, false, false,
  asc::te::dual_dst_mode::split_m }
```

For each output tile, AIC writes the first M half to sub-block 0 and the second M half to sub-block 1. Two slot groups are reused according to `sequence % PIPE_DEPTH`. The AIC-to-AIV flags are 8 and 9, and the AIV-to-AIC flags are 10 and 11. Before reusing a slot, AIC waits on `PIPE_MTE2` for both AIVs to finish. After the transfer, AIC notifies the AIVs on `PIPE_FIX`. Each AIV waits for data on `PIPE_S` and notifies AIC on `PIPE_MTE3` after writing the result back. This prevents UB from being overwritten before Vector consumption and establishes the complete CV pipeline dependency.

## Tensor API Implementation Features

| Feature | Implementation in this example |
| :--- | :--- |
| Tensor representation | Use Tensor objects to describe data at the GM, L1, L0C, and UB levels |
| Matrix computation | Use `mmad` for Cube-side matrix multiply-accumulate; inject Bias only in the first K iteration |
| CV fusion | Use one `__mix__(1, 2)` kernel to connect one AIC and two AIVs |
| Dual-destination transfer | Use `dual_dst_mode::split_m` of `copy_l0c_to_ub` to split along M |
| Vector computation | Use the `__simd_vf__` function `ComputeAdd` for addition |
| Data synchronization | Use fixed flags with `asc_sync_block_wait/arrive` for slot-level handoff |
| Tail-block handling | Limit the valid region with `curM`, `curN`, and `length` to avoid reading padding |

## Build and Run

Execute the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment. **Currently only [CANN master](../../../../docs/en/quick_start.md#cann-install) is supported.**
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example
  Execute the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                                                  # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j;          # Build the project, default NPU mode
  python3 ../scripts/gen_data.py                                                               # Generate test input data
  ./demo                                                                                       # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin                      # Verify output correctness and confirm algorithm logic
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then run CMake again.

- Build option description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `CANN_ASC_USE_EXPERIMENTAL` | `ON` | Required Tensor API switch for building this example |

  > **Note:** This example only supports the dav-3510 architecture (corresponding to Ascend 950PR/Ascend 950DT).

- Execution result

  The following execution result indicates that the precision comparison succeeded.

  ```bash
  test pass!
  ```
