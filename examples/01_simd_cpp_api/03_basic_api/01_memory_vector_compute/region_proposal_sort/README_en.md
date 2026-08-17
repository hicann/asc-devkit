# Region Proposal Sort Pipeline Example

## Overview

This example implements a complete sorting pipeline based on four interfaces: ProposalConcat, RpSort16, MrgSort4, and ProposalExtract. First, ProposalConcat merges consecutive score values into Region Proposal format. Then RpSort16 sorts each group of 16 Region Proposals in descending order by the score field. Next, MrgSort4 merges 4 sorted groups of Region Proposals into 1 group. Finally, ProposalExtract extracts the score field from the merged Region Proposals to obtain globally descending-sorted consecutive score values.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas Inference Series Products AI Core | >= CANN 9.0.0 |

## Directory Structure

```
├── region_proposal_sort
│   ├── scripts
│   │   ├── gen_data.py                    // Script for generating input data and ground truth data
│   │   └── verify_result.py               // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── region_proposal_sort.asc           // Ascend C example implementation & invocation example
│   └── README.md                          // Example documentation
```

## Example Description

- Example function:  
  Sort 64 score values through the complete pipeline and output them. A Region Proposal consists of 8 half elements, where the 5th element (offset=4) is the score field.

- Example specifications:

<table border="2">
<caption>Table 1: Region Proposal Sort Example Specifications</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">region_proposal_sort_custom</td></tr>
</table>

- Example implementation:  
  This example implements the complete Region Proposal sorting pipeline:
  1. **ProposalConcat**: Merge 16 consecutive score values into the score field position (offset=4) of 16 Region Proposals, 4 groups total, 1 repeat per group
  2. **RpSort16**: Sort each group of 16 Region Proposals in descending order by score field, 4 groups total
  3. **MrgSort4**: Merge 4 sorted groups of Region Proposals into 1 group, results sorted in descending order by score
  4. **ProposalExtract**: Extract the score field from the merged 64 Region Proposals to obtain 64 descending-sorted consecutive score values

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
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2002` (default) | NPU architecture: dav-2002 corresponds to Atlas Inference Series Products AI Core |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
