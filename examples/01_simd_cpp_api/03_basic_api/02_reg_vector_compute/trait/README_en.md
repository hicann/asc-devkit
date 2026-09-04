# RegTrait Example

## Overview

This example demonstrates the usage of the register data type RegTensor and MaskReg template parameter regTrait in Ascend C Reg vector computation. It performs division at the register level using two different regTrait configurations (RegTraitNumOne and RegTraitNumTwo). By comparing the implementations under the two configurations, it shows the impact on computation performance and resource usage.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── trait
│   ├── scripts/             // Test script directory
│   │   └── gen_data.py      // Generate test input and golden data
│   ├── CMakeLists.txt       // Build configuration file
│   ├── data_utils.h         // Data read/write utility functions
│   ├── trait.asc            // Ascend C operator implementation & invocation example
│   └── README.md            // Example description document
```

## Example Description

- Example functionality:
  The example computes int64_t type data, implementing vector division using RegTraitNumOne and RegTraitNumTwo configurations for regTrait as RegTensor and MaskReg template parameters. The computation formula is as follows:
  $$z_i = \frac{x_i}{y_i}$$

- Example specifications
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td align="center">y</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[10240]</td><td align="center" colspan="2">int64_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">trait</td></tr>
  </table>

- Example implementation
    - Implementation flow:
        1. Allocate memory on the Host side and initialize input data
        2. Transfer data from Host memory to Device global memory
        3. Call the kernel function to execute computation on the Vector core
        4. Inside the kernel function: transfer input data from Global Memory to Unified Buffer via DataCopy
        5. Call the VF (Vector Function) function via asc_vf_call on the Unified Buffer
        6. Inside the VF function, create RegTensor using the regTrait template parameter and perform LoadAlign, Div, and StoreAlign operations
        7. Transfer computation results from Unified Buffer back to Global Memory
        8. Perform precision verification and golden data comparison on the Host side

    - Multi-scenario description:
        - Scenario 1 (SCENARIO_NUM=1): Use RegTraitNumOne as the regTrait template parameter for RegTensor and MaskReg
            - Implementation function: TraitNumOneVF<T>
            - Implementation:
              `AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> xReg, yReg, zReg;`
              `AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL, AscendC::Reg::RegTraitNumOne>();`
            - Supports b8/b16/b32/b64 type data. For b64 type data, the single instruction launch benefit is reduced.

        - Scenario 2 (SCENARIO_NUM=2): Use RegTraitNumTwo as the regTrait template parameter for RegTensor and MaskReg
            - Implementation function: TraitNumTwoVF<T>
            - Implementation:
              `AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumTwo> xReg, yReg, zReg;`
              `AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL, AscendC::Reg::RegTraitNumTwo>();`
            - Only supports b64/complex32 type data. Increases the number of elements processed per instruction launch, improving parallelism.

    - Performance comparison:
      The following table shows the performance data when the example runs on Ascend 950 series products:

        | Scenario | Cores | Task Duration(us) | aiv_total_cycles | aiv_time(us) | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio | aiv_mte2_time(us) | aiv_mte2_ratio | aiv_mte3_time(us) | aiv_mte3_ratio |
        |:---:|:---:|:-----------------:|:----------------:|:------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:--------------:|:-----------------:|:--------------:|
        |  1  |  1  |      58.168       |      95271       |    57.92     |      56.09       |     0.969     |        0.268        |      0.004       |       1.082       |     0.019      |       0.463       |     0.008      |
        |  2  |  1  |      39.563       |      64543       |    39.318    |      37.471      |     0.953     |        0.273        |      0.007       |       1.082       |     0.038      |       0.471       |     0.012      |

        Scenario 2 uses a RegTensor bit width 2x that of Scenario 1. With the same data volume, the overall execution time decreases from 58.168 us to 39.563 us, a total reduction of 18.605 us, representing a performance improvement of approximately 32%. The instruction execution cycles decrease sharply from 95,271 to 64,543. This demonstrates that a wider register configuration effectively eliminates instruction splitting and pipeline wait cycles caused by bit width limitations, unleashing the computing power of the Vector unit.
      <br>
      **Performance metric description:**

        | Field | Description |
        |:---:|:---|
        | Task Duration(us)|Total task duration, including scheduling to the accelerator, execution on the accelerator, and response completion time.|
        | aiv_total_cycles|Total CPU cycles consumed by the task executing on the Vector Core.|
        | aiv_time|Theoretical execution time of the task on the AI Vector Core, in us.|
        | aiv_vec_time(us) | Time consumed by vec-type instructions (vector computation instructions), in us. |
        | aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles. |
        | aiv_scalar_time(us) | Time consumed by scalar-type instructions (scalar computation instructions), in us. |
        | aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles. |
        | aiv_mte2_time(us) | Time consumed by mte2-type instructions (GM->UB transfer instructions), in us. |
        | aiv_mte2_ratio | Ratio of mte2-type instruction cycles to total cycles. |
        | aiv_mte3_time(us) | Time consumed by mte3-type instructions (UB->GM transfer instructions), in us. |
        | aiv_mte3_ratio | Ratio of mte3-type instruction cycles to total cycles. |

    - Constraints:
        - Input data length must be a multiple of GetVecLen()
        - The divisor cannot be 0. In practical applications, a check for zero divisors is required
        - The example currently only supports the int64_t data type

## Build and Run

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  # Scenario 2: Use RegTraitNumTwo
  SCENARIO_NUM=2
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM; make -j;                           # Build the project
  python3 ../scripts/gen_data.py;                                           # Generate test input data
  ./demo                                                                    # Run the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.
- Build option description

  | Option | Values | Description |
  |:--------------------------|:----------------------|:---------------------------------------------|
  | `SCENARIO_NUM`            | `1` (default), `2`        | 1: Supports RegTraitNumOne; <br/>2: Supports RegTraitNumTwo;       |
  | `CMAKE_ASC_RUN_MODE`      | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation                      |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`            | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
