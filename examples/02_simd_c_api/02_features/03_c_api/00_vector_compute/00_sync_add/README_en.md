# Add Operator Example Using C_API (Synchronous Scenario)
## Overview
This example demonstrates the Add operator using C_API interfaces, implemented based on synchronous data transfer and compute interfaces.
## Supported Products
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products
## Directory Structure
```
├── 00_sync_add
│   ├── CMakeLists.txt         // CMake build file
│   └── c_api_add.asc          // Operator implementation & invocation example
```

## Operator Description
- Operator Function:  
  The Add operator adds two data inputs and returns the sum. The corresponding mathematical expression is:  
  ```
  z = x + y
  ```

- Operator Specifications:
  <table>
  <tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">Add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">z</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Operator Implementation:

  - Kernel Implementation  

    For C_API, input data must first be transferred to on-chip storage, then the compute interface is used to add the two input parameters and produce the final result, which is then transferred back to external storage.

    The Add operator implementation consists of 3 steps:

    Step 1: Transfer input x and y from Global Memory to Local Memory, stored in xLocal and yLocal respectively.
    
    Step 2: Perform addition on xLocal and yLocal, and store the result in zLocal.
    
    Step 3: Transfer the output data from zLocal to output z in Global Memory.

  - Invocation Implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run
Run the following steps in the root directory of this example to build and execute the operator.
- Configure Environment Variables
  Select the corresponding command to configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  - Default path, root user installs the CANN package
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```
    
  - Default path, non-root user installs the CANN package
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, install the CANN package
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Run the Example
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake ..;make -j;             # Build the project
  # Run the following in the build directory
  ./c_api_add_example           # Run the example
  ```
  The following output indicates that the accuracy verification is successful.
  ```bash
  [Success] Case accuracy is verification passed.
  ```
