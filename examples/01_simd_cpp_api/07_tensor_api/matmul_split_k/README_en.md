# Split-K Matmul Operator Example Based on Tensor API

## Overview

This example implements multi-core matrix multiplication based on the static Tensor API programming paradigm. It uses the high-level Tensor API interfaces for data movement, slicing, and multiply-accumulate operations. The computation is split along the K axis, and each core's partial result is accumulated into the output matrix with atomic addition.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR&950DT products | >= CANN 9.2.0 |

## Directory Structure

```text
├── matmul_split_k
│   ├── scripts
│   │   ├── gen_data.py         // Input data and golden data generation script
│   │   └── verify_result.py    // Golden value comparison file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_split_k.asc      // Ascend C sample implementation
│   └── README.md               // Example documentation
```

## Example Description

- Example functionality:
  Matmul calculation formula:
  $$
  C = A * B
  $$
- Example specifications:
  This example uses M = 16, N = 16, and K = 1024. The K axis is evenly split into four parts, and four cores perform the computation. The input and output specifications are shown in the following table.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_split_k_kernel</td></tr>
  </table>

- Example implementation:
  - Implementation process:
    <table>
    <tr><th align="left">Step</th><th align="left">Tensor API Operation</th><th align="left">Function</th><th align="left">Layout Transformation</th></tr>
    <tr><td align="left">1</td><td align="left">Constant Tiling Parameters</td><td align="left">Define the parameters required by the kernel through compile-time constants</td><td align="left">Not applicable</td></tr>
    <tr><td align="left">2</td><td align="left">make_tensor + slice</td><td align="left">Create GM tensors, split the K axis into four equal parts, and slice the data block processed by the current core according to its core index</td><td align="left">ND format</td></tr>
    <tr><td align="left">3</td><td align="left">copy(copy_gm_to_l1)</td><td align="left">Move the current K-axis slices of matrices A and B from GM to L1</td><td align="left">ND->NZ format conversion</td></tr>
    <tr><td align="left">4</td><td align="left">copy(copy_l1_to_l0a/copy_l1_to_l0b)</td><td align="left">Move data from L1 to L0A and L0B</td><td align="left">L1->L0A: NZ->NZ<br>L1->L0B: NZ->ZN</td></tr>
    <tr><td align="left">5</td><td align="left">mmad</td><td align="left">Complete matrix multiply-accumulate computation for the current K-axis slice</td><td align="left">The matrix multiplication result is in NZ format</td></tr>
    <tr><td align="left">6</td><td align="left">asc_set_atomic_add_float + copy(copy_l0c_to_gm)</td><td align="left">Enable float atomic addition and accumulate the L0C results from all cores into the same GM output matrix</td><td align="left">NZ->ND format conversion</td></tr>
    </table>

    The host clears the GM output matrix C before launching the kernel. After the atomic accumulation write-back is complete, the kernel calls `asc_disable_dma_atomic` to disable DMA atomic addition.

  - Tensor API core interfaces:
    1. **Tensor creation interface**: Use `make_tensor` + `make_mem_ptr` + `make_frame_layout` to create tensors at each memory level.
       - GM tensors: `nd_ext_layout_ptn` layout (ND format extension)
       - L1/L0 tensors: `nz_layout_ptn`/`zn_layout_ptn` layouts (adapted to the cube compute unit)

    2. **Data movement interface**: Use `copy` and its atom objects.
       - `copy_gm_to_l1`: Move data from GM to L1 and automatically perform ND to NZ format conversion.
       - `copy_l1_to_l0a`/`copy_l1_to_l0b`: Move data from L1 to L0A/L0B.
       - `copy_l0c_to_gm`: Move data from L0C to GM and automatically perform NZ to ND format conversion.

    3. **Matrix multiplication interface**: Use `mmad` and its atom objects.
       - Accumulation control is managed automatically through the `init_with_zero` parameter.

    4. **Slicing interface**: Use `slice` + `make_coord` + `make_shape` to obtain tensor sub-regions.
       - Implement the multi-core split: split the K axis evenly so that each core processes one K-axis slice.

    5. **Atomic accumulation interface**: Use `asc_set_atomic_add_float` to configure float atomic addition for L0C to GM write-back.
       - Accumulate the results of all K-axis slices into the same output matrix, then use `asc_disable_dma_atomic` to disable atomic addition after write-back.

  - Invocation implementation:
    Use the kernel call operator `<<<>>>` to invoke the kernel function.

## Tensor API Implementation Features

This example primarily uses the following Tensor API capabilities:

| Feature | Example Implementation |
|---------|------------------------|
| Memory management | Allocate memory with arrays and manage offsets automatically |
| Tensor representation | Use tensor objects to describe GM, L1, and L0 data directly |
| Data movement | Use `copy` for movement between memory levels |
| Format conversion | Rely on layout patterns to automatically convert between NZ and ZN |
| Multi-core split | Use `slice` along the K axis to obtain the data slice processed by the current core |
| Computation interface | Use `mmad` for matrix multiply-accumulate and atomic addition to accumulate partial results |

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package in the current environment. **Currently only [CANN master](../../../../docs/en/quick_start.md#cann-install) is supported.**
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default NPU mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify the output result
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter. Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then run cmake again.

- Build option description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR&950DT products |

  > **Note:** This example only supports dav-3510 architecture (corresponding to Ascend 950PR&950DT products).

- Execution result
  The following execution result indicates that the precision comparison succeeded.

  ```bash
  test pass!
  ```
