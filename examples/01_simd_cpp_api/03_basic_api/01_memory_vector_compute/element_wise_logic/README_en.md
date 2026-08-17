# Element-wise Logic Operation Example

## Overview

This example implements bitwise logic operation functionality based on And, Ors, ShiftLeft, and ShiftRight interfaces. The And interface performs bitwise AND on two source operands. The Ors interface performs OR between each element in a vector and a scalar. The ShiftLeft interface performs left shift on source operands (tensor form). The ShiftRight interface performs right shift on source operands (scalar form). The example supports switching between different scenarios through compilation parameters to help developers understand the usage and implementation differences of these interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── element_wise_logic
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── element_wise_logic.asc  // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Scenario Description

This example switches between different scenarios through the compilation parameter `SCENARIO_NUM`:

<table border="2">
<caption>Table 1: Scenario Configuration</caption>
<tr><th>scenarioNum</th><th>Interface</th><th>Input Shape</th><th>Output Shape</th><th>Data Type</th><th>Description</th></tr>
<tr><td>1</td><td>And</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>Bitwise AND on two source operands</td></tr>
<tr><td>2</td><td>Ors</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>Scalar at front, src0[0] as scalar OR with src1 vector (only supported on dav-3510)</td></tr>
<tr><td>3</td><td>ShiftLeft</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>Left shift with shift amount specified by tensor (only supported on dav-3510)</td></tr>
<tr><td>4</td><td>ShiftRight</td><td>[1, 512]</td><td>[1, 512]</td><td>uint16</td><td>Right shift with shift amount specified by constant SHIFT_BITS=2</td></tr>
</table>

**Scenario 1: And Bitwise AND**
- Input shape: src0=[1, 512], src1=[1, 512]
- Output shape: dst=[1, 512]
- Data type: uint16
- Parameter: count=512
- Implementation:

    ```cpp
    AscendC::And(dstLocal, src0Local, src1Local, COUNT);
    ```

- Description: Perform bitwise AND on each element of src0 and src1, result stored in dst
- Example:
  - Input src0: [1 2 3 ... 512]
  - Input src1: [512 511 510 ... 1]
  - Output dst: [1 0 3 ... 0]

**Scenario 2: Ors Vector-Scalar OR (Scalar at Front) ---- This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: src0=[1, 512] (src0Local[0] as scalar), src1=[1, 512] (vector)
- Output shape: dst=[1, 512]
- Data type: uint16
- Parameter: count=512
- Implementation:

    ```cpp
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<AscendC::BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, COUNT);
    ```

- Description: Scalar at front, src0Local[0] as scalar (left operand), src1Local as vector (right operand), perform bitwise OR on each element of src1 with src0Local[0], result stored in dst
- Example:
  - Input src0[0]: 1
  - Input src1: [1 2 3 ... 512]
  - Output dst: [1 3 3 5 5 .. 513]

**Scenario 3: ShiftLeft Left Shift (Tensor Form) ---- This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: src0=[1, 512] (data to shift, uint16), src1=[1, 512] (left shift amounts, int16)
- Output shape: dst=[1, 512]
- Data type: uint16
- Parameter: count=512
- Implementation:

    ```cpp
    AscendC::ShiftLeft(dstLocal, src0Local, src1Local, COUNT);
    ```

- Description: Left shift each element of src0 by the corresponding shift amount in src1, result stored in dst. src1 stores left shift amounts and does not support negative values
- Example:
  - Input src0: [1 2 3 ... 512]
  - Input src1: [2 2 2 ... 2] (left shift amounts)
  - Output dst: [4 8 12 ... 2048]

**Scenario 4: ShiftRight Right Shift (Scalar Form)**
- Input shape: src0=[1, 512]
- Output shape: dst=[1, 512]
- Data type: uint16
- Parameter: count=512, SHIFT_BITS=2
- Implementation:

    ```cpp
    AscendC::ShiftRight(dstLocal, src0Local, SHIFT_BITS, COUNT);
    ```

- Description: Right shift each element of src0 by SHIFT_BITS bits, result stored in dst. Unsigned data types perform logical right shift, signed data types perform arithmetic right shift
- Example:
  - Input src0: [1 2 3 4 5 ... 512]
  - Output dst: [0 0 0 1 1 1 1 2 2 2 ... 128]

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT. Note: Scenarios 2 and 3 only support dav-3510, automatically switched during compilation |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4` | Scenario number: 1 (And bitwise AND), 2 (Ors vector-scalar OR), 3 (ShiftLeft tensor form), 4 (ShiftRight scalar form) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
