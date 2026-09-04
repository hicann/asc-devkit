# Subnormal Example

## Overview

This example demonstrates the behavioral differences of the Div operation in Ascend C Reg vector computation when Subnormal mode is enabled or disabled. Through two different Div precision algorithm configurations (PRECISION_1ULP_FTZ_FALSE and PRECISION_1ULP_FTZ_TRUE), it shows the impact of Subnormal floating-point number support on computation results.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
subnormal
│   ├── scripts/             // Test script directory
│   │   └── gen_data.py      // Generate test input and golden data
│   ├── CMakeLists.txt       // Build configuration file
│   ├── data_utils.h         // Data read/write utility functions
│   ├── subnormal.asc        // Ascend C operator implementation & invocation example
│   └── README.md            // Example description document
```

## Example Description

- Example functionality:

  The example computes float type data, using PRECISION_1ULP_FTZ_FALSE and PRECISION_1ULP_FTZ_TRUE to configure DivSpecificMode as the precision algorithm parameter for the Div operation, executing division. The computation formula is as follows:
  $$z_i = \frac{x_i}{y_i}$$

- Example specifications
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">subnormal</td></tr>
  </table>

- Example implementation
    - Implementation flow:
        1. Allocate memory on the Host side and initialize input data
        2. Transfer data from Host memory to Device global memory
        3. Call the kernel function to execute computation on the Vector core
        4. Inside the kernel function: transfer input data from Global Memory to Unified Buffer via DataCopy
        5. Call the VF (Vector Function) function via asc_vf_call on the Unified Buffer
        6. Inside the VF function, use the DivSpecificMode template parameter to perform LoadAlign, Div, and StoreAlign operations
        7. Transfer computation results from Unified Buffer back to Global Memory
        8. Perform precision verification and golden data comparison on the Host side

  - Subnormal and FTZ concept description:
      - Subnormal: Floating-point numbers where the exponent is all zeros and the mantissa is not all zeros, representing extremely small values close to 0. The IEEE 754 standard supports Subnormal numbers, providing better numerical precision and gradual underflow characteristics.
      - FTZ: A mode that forces Subnormal numbers to 0. Although it simplifies hardware design and improves performance, it sacrifices numerical precision.

  - Multi-scenario description:
      - Scenario 1 (SCENARIO_NUM=1): Use PRECISION_1ULP_FTZ_FALSE as the precision algorithm parameter for Div
          - Implementation function: SubnormalFTZFalseVF<T>
          - Implementation:
            ```cpp
            static constexpr AscendC::Reg::DivSpecificMode mode = {ZEROING, false, PRECISION_1ULP_FTZ_FALSE};
            AscendC::Reg::Div<T, &mode>(zReg, xReg, yReg, mask);
            ```
          - Description: Supports Subnormal data computation, uses a single instruction to compute the result, with a maximum precision error of 1 ulp. When the division result is a Subnormal number, the value is preserved.
          - Application scenarios: Scientific computing, numerical simulation, and high-precision computation scenarios that require precise handling of Subnormal floating-point numbers.

      - Scenario 2 (SCENARIO_NUM=2): Use PRECISION_1ULP_FTZ_TRUE as the precision algorithm parameter for Div
          - Implementation function: SubnormalFTZTrueVF<T>
          - Implementation:
            ```cpp
            static constexpr AscendC::Reg::DivSpecificMode mode = {ZEROING, false, PRECISION_1ULP_FTZ_TRUE};
            AscendC::Reg::Div<T, &mode>(zReg, xReg, yReg, mask);
            ```
          - Description: Does not support Subnormal data computation. FTZ mode uses a single instruction to compute the result, with a maximum precision error of 1 ulp. When the division result is a Subnormal number, it is forced to 0.
          - Application scenarios: Deep learning, image processing, and real-time inference scenarios where Subnormal precision requirements are not high, simplifying hardware implementation and improving computation performance.

    - Computation data scenario comparison:

      | No. | Div Combination | Dividend | Divisor | Scenario 1 (golden match) | Analysis | Scenario 2 Output (precision loss) | Analysis |
      |:---:|:---------:|:------------:|:----------:|:-------------:|:------------------|:-----------:|:--------------------------------|
      | 1  | N / N = S | $2^{-126}$ |    4.0     |  0x00200000   | Result is subnormal $2^{-128}$ | 0x00000000  | Result falls in Subnormal range, forced to zero        |
      | 2  | N / S = N | $2^{-125}$ | $2^{-127}$ |  0x40800000   | Result is normal $2^{2}$     | 0x7f800000  | Divisor S treated as 0, any non-zero positive divided by 0 is infinity         |
      | 3  | S / N = S | $2^{-130}$ |    2.0     |  0x00040000   | Result is subnormal $2^{-131}$ | 0x00000000  | Dividend S treated as 0 on input, result is 0               |
      | 4  | S / N = N | $2^{-130}$ | $2^{-20}$  |  0x08800000   | Result is normal $2^{110}$   | 0x00000000  | Dividend S treated as 0 on input, result is 0               |
      | 5  | S / S = N | $2^{-140}$ | $2^{-145}$ |  0x42000000   | Result is normal $2^{5}$     | 0x7fffffff  | Both dividend S and divisor S treated as 0, $0 \div 0$, result is NaN |

      - N: normal data, normal number.
      - S: Subnormal data, subnormal number.

  - Constraints:
      - Input data length must be a multiple of GetVecLen()
      - The divisor cannot be 0. In practical applications, a check for zero divisors is required
      - The example currently only supports the float data type

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
  # Scenario 2: Use PRECISION_1ULP_FTZ_TRUE (FTZ mode)
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
  | `SCENARIO_NUM`            | `1` (default), `2`        | 1: Supports subnormal computation; <br/>2: Does not support subnormal computation;       |
  | `CMAKE_ASC_RUN_MODE`      | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation                      |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`            | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
