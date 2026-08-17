## Add Operator Sample Using C_API (Async Scenario, Manual Pipeline Synchronization)

## Overview

This sample implements the Add operator using C_API interfaces, based on asynchronous data movement, compute interfaces, and manually added synchronization instructions.

## Supported Products

- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure

```
├── c_api_delicacy_async_add
│   ├── CMakeLists.txt      // Build project file
│   ├── README.md           // Documentation
│   └── c_api_add.asc       // C_API operator implementation & invocation sample
```

## Operator Description

- Operator Function:  
The Add operator adds two inputs and returns the sum. The corresponding mathematical expression is:  
  ```
  z = x + y
  ```
- Operator Specification:
  <table>
  <tr><td rowspan="1" align="center">Operator Type(OpType)</td><td colspan="4" align="center">Add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">8 * 2048</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">8 * 2048</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">z</td><td align="center">8 * 2048</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Operator Implementation: 

  C_API input data must first be moved to on-chip memory, then compute interfaces are used to add the two input parameters and obtain the final result, which is then moved to external memory. The computation process uses asc_sync_notify and asc_sync_wait to control synchronization between pipelines.

  The Add operator implementation consists of 3 steps:

  Step 1: Move inputs x and y from Global Memory to Local Memory, storing them in xLocal and yLocal respectively.
  
  Step 2: Perform the addition operation on xLocal and yLocal, and store the computation result in zLocal.
  
  Step 3: Move the output data from zLocal to output z in Global Memory.

- Invocation Implementation  
  Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Execute the following steps in the root directory of this sample to build and run the operator.
- Configure Environment Variables  
  Select the appropriate environment variable configuration command based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on your current environment.
  - Default path, CANN software package installed by root user
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, CANN software package installed by non-root user
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, CANN software package installed
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Sample Execution
  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake ..;make -j;             # Build project
  ./c_api_add_example           # Execute sample
  ```
  The following result indicates successful accuracy verification.
  ```bash
  [Success] Case accuracy is verification passed.
  ```