# Add Operator Example Using C_API (Synchronous Scenario)

## Overview

This example uses C_API interfaces to implement the Add operator example, based on synchronous data movement and computation interfaces.

## Supported Products

- Atlas A3 Training Series Products/Atlas A3 Inference Series Products
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products

## Directory Structure
```
├── c_api_sync_add
│   ├── CMakeLists.txt         // cmake build file
│   └── c_api_add.asc          // operator implementation & invocation example
```

## Operator Description

- Operator Function:  
  The Add operator implements the function of adding two data values and returning the addition result. The corresponding mathematical expression is:  
  ```
  z = x + y
  ```

- Operator Specifications:
  <table>
  <tr><td rowspan="1" align="center">Operator Type(OpType)</td><td colspan="4" align="center">Add</td></tr>
  </tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  </tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">z</td><td align="center">2048*8</td><td align="center">float</td><td align="center">ND</td></tr>
  </tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Operator Implementation:

  - Kernel Implementation  

    C_API input data needs to be moved to on-chip memory first, then the computation interface is used to complete the addition of two input parameters to obtain the final result, and then moved to external storage.

    The implementation flow of the Add operator is divided into 3 steps:

    The first step moves the inputs x and y from Global Memory to Local Memory, storing them in xLocal and yLocal respectively.
    
    The second step performs addition on xLocal and yLocal, and the computation result is stored in zLocal.
    
    The third step moves the output data from zLocal to the output z on Global Memory.

  - Invocation Implementation  
    The kernel function is invoked using the kernel call operator <<<>>>.

## Compilation and Execution

Execute the following steps in the root directory of this example to compile and execute the operator.
- Configure Environment Variables  
  Please select the corresponding command to configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  - Default path, CANN software package installed by root user
    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    ```

  - Default path, CANN software package installed by non-root user
    ```bash
    source $HOME/Ascend/cann/set_env.sh
    ```

  - Specified path install_path, CANN software package installation
    ```bash
    source ${install_path}/cann/set_env.sh
    ```

- Example Execution
  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake ..;make -j;             # Compile the project
  # Execute the following content in the build directory
  ./c_api_add_example           # Execute the example
  ```
  The execution result is as follows, indicating that the accuracy comparison is successful.
  ```bash
  [Success] Case accuracy is verification passed.
  ```
