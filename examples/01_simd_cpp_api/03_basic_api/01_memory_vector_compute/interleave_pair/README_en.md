# Interleave/DeInterleave Example

## Overview

This example implements element interleave and de-interleave functionality based on the Interleave and DeInterleave interfaces. Interleave stores elements from two source operands in an interleaved manner into two result operands. DeInterleave stores elements from two source operands in a de-interleaved manner into two result operands. The example supports switching between different scenarios through compilation parameters to help developers understand the usage and implementation differences of these two interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── interleave_pair
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── interleave_pair.asc     // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Scenario Description

This example switches between different scenarios through the compilation parameter `SCENARIO_NUM`:

<table border="2">
<caption>Table 1: Scenario Configuration</caption>
<tr><th>scenarioNum</th><th>Interface</th><th>Input Shape</th><th>Output Shape</th><th>Data Type</th><th>Description</th></tr>
<tr><td>1</td><td>Interleave</td><td>[1, 512], [1, 512]</td><td>[1, 512], [1, 512]</td><td>half</td><td>Store elements from two source operands in interleaved manner into two result operands</td></tr>
<tr><td>2</td><td>DeInterleave</td><td>[1, 512], [1, 512]</td><td>[1, 512], [1, 512]</td><td>half</td><td>Store elements from two source operands in de-interleaved manner into two result operands</td></tr>
</table>

**Scenario 1: Interleave Element Interleaving**
- Input shape: src0=[1, 512], src1=[1, 512]
- Output shape: dst0=[1, 512], dst1=[1, 512]
- Data type: half
- Parameter: count=512
- Implementation:

    ```cpp
    AscendC::Interleave(dst0Local, dst1Local, src0Local, src1Local, count);
    ```

- Description: Interleave elements from src0 and src1 into dst0 and dst1. dst0 stores the interleaved result of the first half of src0 and the first half of src1; dst1 stores the interleaved result of the second half of src0 and the second half of src1
- Example:
  - Input src0: [1 2 3 ... 512]
  - Input src1: [513 514 515 ... 1024]
  - Output dst0: [1 513 2 514 ... 256 768]
  - Output dst1: [257 769 258 770 ... 512 1024]

**Scenario 2: DeInterleave Element De-interleaving**
- Input shape: src0=[1, 512], src1=[1, 512]
- Output shape: dst0=[1, 512], dst1=[1, 512]
- Data type: half
- Parameter: count=512
- Implementation:

    ```cpp
    AscendC::DeInterleave(dst0Local, dst1Local, src0Local, src1Local, count);
    ```

- Description: De-interleave elements from src0 and src1 into dst0 and dst1. dst0 stores elements at odd index positions from src0 and src1; dst1 stores elements at even index positions from src0 and src1
- Example:
  - Input src0: [1 2 3 ... 512]
  - Input src1: [513 514 515 ... 1024]
  - Output dst0: [1 3 5 ... 511 513 515 ... 1023]
  - Output dst1: [2 4 6 ... 512 514 516 ... 1024]

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
  SCENARIO_NUM=1  # Set the scenario number
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output_dst0.bin ./output/output_dst1.bin ./output/golden_dst0.bin ./output/golden_dst1.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT, this example only supports this architecture |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1 (Interleave interleaving), 2 (DeInterleave de-interleaving) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
