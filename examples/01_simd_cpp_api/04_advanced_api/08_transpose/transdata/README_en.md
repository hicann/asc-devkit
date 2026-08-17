# TransData Example

## Overview

This example uses the TransData high-level API to implement data layout format conversion, supporting conversion of input data layout to a target layout format.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── transdata
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── transdata.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

Convert the input data layout to a target layout format.  
In addition to dimension order changes, this involves splitting the C axis and N axis. Specifically, the C axis is split into C1 and C0 axes, and the N axis is split into N1 and N0 axes. For data types with a bit width of 16, C0 and N0 are fixed at 16. The formulas for C1 and N1 are as follows:

$$ C1 = (C + C0 - 1) / C0 $$

$$ N1 = (N + N0 - 1) / N0 $$

This example supports the following four data format conversion scenarios:

### Scenario 1: NCDHW -> FRACTAL_Z_3D (mode = 1)

- Example specifications:
  <table>
  <caption>Table 1: Scenario 1 Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> transdata </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[16, 16, 1, 3, 5]</td><td align="center">half</td><td align="center">NCDHW</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 1, 3, 5, 1, 16, 16]</td><td align="center">half</td><td align="center">FRACTAL_Z_3D</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transdata_custom</td></tr>
  </table>

  **Note**:
  - Input shape: [N, C, D, H, W] = [16, 16, 1, 3, 5]
  - Output shape: [D, C1, H, W, N1, N0, C0] = [1, 1, 3, 5, 1, 16, 16]

### Scenario 2: FRACTAL_Z_3D -> NCDHW (mode = 2)

- Example specifications:
  <table>
  <caption>Table 2: Scenario 2 Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> transdata </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 1, 3, 5, 1, 16, 16]</td><td align="center">half</td><td align="center">FRACTAL_Z_3D</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[16, 16, 1, 3, 5]</td><td align="center">half</td><td align="center">NCDHW</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transdata_custom</td></tr>
  </table>

  **Note**:
  - Input shape: [D, C1, H, W, N1, N0, C0] = [1, 1, 3, 5, 1, 16, 16]
  - Output shape: [N, C, D, H, W] = [16, 16, 1, 3, 5]
  - This is the inverse operation of Scenario 1

### Scenario 3: NCDHW -> NDC1HWC0 (mode = 3)

- Example specifications:
  <table>
  <caption>Table 3: Scenario 3 Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> transdata </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[16, 16, 1, 3, 5]</td><td align="center">half</td><td align="center">NCDHW</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[16, 1, 1, 3, 5, 16]</td><td align="center">half</td><td align="center">NDC1HWC0</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transdata_custom</td></tr>
  </table>

  **Note**:
  - Input shape: [N, C, D, H, W] = [16, 16, 1, 3, 5]
  - Output shape: [N, D, C1, H, W, C0] = [16, 1, 1, 3, 5, 16]

### Scenario 4: NDC1HWC0 -> NCDHW (mode = 4)

- Example specifications:
  <table>
  <caption>Table 4: Scenario 4 Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> transdata </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[16, 1, 1, 3, 5, 16]</td><td align="center">half</td><td align="center">NDC1HWC0</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[16, 16, 1, 3, 5]</td><td align="center">half</td><td align="center">NCDHW</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transdata_custom</td></tr>
  </table>

  **Note**:
  - Input shape: [N, D, C1, H, W, C0] = [16, 1, 1, 3, 5, 16]
  - Output shape: [N, C, D, H, W] = [16, 16, 1, 3, 5]
  - This is the inverse operation of Scenario 3

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO=$SCENARIO ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py --mode $SCENARIO  # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  For example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO=$SCENARIO ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO=$SCENARIO ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO` | `1` (default), `2`, `3`, `4` | Scenario: 1=NCDHW to FRACTAL_Z_3D, 2=FRACTAL_Z_3D to NCDHW, 3=NCDHW to NDC1HWC0, 4=NDC1HWC0 to NCDHW |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
