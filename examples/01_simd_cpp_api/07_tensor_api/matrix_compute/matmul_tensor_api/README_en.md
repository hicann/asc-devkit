# Static Matmul Operator Example Based on Tensor API Implementation

## Overview

This example implements multi-core matrix multiplication computation based on the static Tensor API programming paradigm. It uses the high-level abstraction interfaces of Tensor API to complete matrix data movement, slicing, and multiply-accumulate processes.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.1.0 |

> **Note:** This example depends on CANN features that are not yet officially released. Please use the latest CANN master package.

## Directory Structure

```text
├── matmul_tensor_api
│   ├── scripts
│   │   ├── gen_data.py         // Input data and golden data generation script
│   │   └── verify_result.py    // Golden value comparison file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul.asc              // Ascend C sample implementation
│   └── README.md               // Sample documentation
```

## Sample Description

- Sample Function:
  Matmul calculation formula:
  $$
  C = A * B
  $$
- Sample Specifications:
  This example uses parameters M = 512, N = 1024, K = 512, and invokes 16 cores to complete the computation. The input specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_custom</td></tr>
  </table>

- Sample Implementation:
  - Implementation Process:
    <table>
    <tr><th align="left">Step</th><th align="left">Tensor API Operation</th><th align="left">Function</th><th align="left">Layout Transformation</th></tr>
    <tr><td align="left">1</td><td align="left">Constant Tiling Parameters</td><td align="left">Pass to kernel through template parameters</td><td align="left">Not applicable</td></tr>
    <tr><td align="left">2</td><td align="left">make_tensor + slice</td><td align="left">Create GM tensor and slice to get data block processed by current core</td><td align="left">ND format</td></tr>
    <tr><td align="left">3</td><td align="left">copy(copy_gm_to_l1)</td><td align="left">Move A matrix and B matrix data from GM to L1</td><td align="left">ND->NZ format conversion</td></tr>
    <tr><td align="left">4</td><td align="left">copy(copy_l1_to_l0a/copy_l1_to_l0b)</td><td align="left">Move data from L1 to L0A and L0B</td><td align="left">L1->L0A: NZ->NZ<br>L1->L0B: NZ->ZN</td></tr>
    <tr><td align="left">5</td><td align="left">mmad</td><td align="left">Complete matrix multiply-accumulate computation</td><td align="left">Matrix multiplication result is NZ format</td></tr>
    <tr><td align="left">6</td><td align="left">copy(copy_l0c_to_gm)</td><td align="left">Move computation result from L0C to GM</td><td align="left">NZ->ND format conversion</td></tr>
    </table>

  - Tensor API Core Interfaces:
    1. **Tensor Creation Interface**: Use make_tensor + make_mem_ptr + make_frame_layout to create tensors at each level
       - GM Tensor: nd_ext_layout_ptn layout (ND format extension)
       - L1/L0 Tensor: nz_layout_ptn/zn_layout_ptn layout (adapted to cube compute unit)

    2. **Data Movement Interface**: Use copy and its atom object
       - copy_gm_to_l1: Data movement from GM to L1, automatically handles ND→NZ format conversion
       - copy_l1_to_l0a/copy_l1_to_l0b: Data movement from L1 to L0A/L0B
       - copy_l0c_to_gm: Data movement from L0C to GM, automatically handles NZ→ND format conversion

    3. **Matrix Multiplication Interface**: Use mmad and its atom object
       - Automatically manages accumulation control (init_with_zero parameter)

    4. **Slicing Interface**: Use slice + make_coord + make_shape to get tensor sub-regions
       - Implements multi-core logic: each core processes different data blocks
       - Implements tiling logic: baseM/baseK/baseN basic block partitioning

  - Invocation Implementation
    Use the kernel call operator <<<>>> to invoke the kernel function.

## Tensor API Implementation Features

This example primarily uses the following Tensor API capabilities for implementation:

| Feature | Example Implementation |
|---------|------------------------|
| Memory Management | Use array method to allocate memory, automatically manage offsets |
| Tensor Representation | Use tensor objects to directly describe GM, L1, L0 data |
| Data Movement | Use copy to complete cross-level movement |
| Format Conversion | Rely on layout patterns to automatically complete NZ / ZN conversion |
| Multi-core Logic | Use slice to get data block processed by current core |
| Computation Interface | Use mmad to complete matrix multiply-accumulate |

## Build and Run

Execute the following steps in the root directory of this example to build and run the sample.

- Configure Environment Variables
  Configure environment variables according to the [installation method](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) of the CANN development kit package on the current environment. **Currently only [CANN master](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) is supported**.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program to run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output result correctness, confirm algorithm logic is correct
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, you need to clean the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Build Option Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  > **Note:** This example only supports dav-3510 architecture (corresponding to Ascend 950PR/Ascend 950DT).

- Execution Result
  The execution result is as follows, indicating that the precision comparison succeeded.

  ```bash
  test pass!
  ```
