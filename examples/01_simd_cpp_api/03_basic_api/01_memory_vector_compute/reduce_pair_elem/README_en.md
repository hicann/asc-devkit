# `ReducePairElem<SUM>` Example

## Overview

This example implements pair-wise element summation based on the `ReducePairElem<SUM>` interface in a reduction scenario. It sums adjacent pairs of elements from data (a1, a2, a3, a4, a5, a6...) to produce (a1+a2, a3+a4, a5+a6, ...), that is, it performs summation reduction on each pair (adjacent elements composed of even and odd indices), with the output element count being half of the input.

Note: `ReducePairElem` is the renamed API after CANN 9.1.0. For CANN 9.0.0 and earlier versions, use `PairReduceSum`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

``` text
├── reduce_pair_elem
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── reduce_pair_elem.asc    // Ascend C example implementation & invocation
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example calls the `ReducePairElem<SUM>` interface to perform summation on all adjacent element pairs in the input. The example specifications are shown in the table below:

  <table border="2">
  <caption>Table 1: ReducePairElem&lt;SUM&gt; Example Specifications</caption>
  <tr>
  <td rowspan="1" align="center">Example Type (OpType)</td>
  <td colspan="4" align="center">ReducePairElem&lt;SUM&gt;</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
   <tr><td rowspan="2" align="center">Example Output</td></tr>
   <tr><td align="center">z</td><td align="center">[1, 64]</td><td align="center">half</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_pair_elem_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a `ReducePairElem<SUM>` example with fixed input shape x[1, 128] and output shape z[1, 64].

  In the kernel, `ReducePairElem<SUM>` sums each pair of adjacent elements (even index and odd index) in `srcLocal` and stores the results in `dstLocal`.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;             # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
