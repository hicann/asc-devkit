# Custom Operator Project + aclnn Single-Operator API Invocation Example

## Overview

This example introduces how to execute a fixed-shape operator using the aclnn `OpType` single-operator API approach, based on the sample custom operator project.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |
| Atlas 200I/500 A2 Inference Products | >= CANN 9.0.0 |
| Atlas Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── aclnn_invocation
│   ├── CMakeLists.txt          // Build project file
│   ├── main.cpp                // Operator invocation main program
│   └── README.md               // Example documentation
```

## Code Implementation

After completing the development and deployment of the custom operator, the single-operator functionality can be verified through single-operator API invocation. For details, refer to the [Single-Operator API Invocation](https://hiascend.com/document/redirect/CannCommunityAscendCInVorkSingleOp) section and the "Single-Operator API Execution" section in [Single-Operator Invocation](https://hiascend.com/document/redirect/CannCommunityCppOpcall).

Single-operator API execution is a C-language API-based approach that does not require a single-operator description file for offline model conversion. The single-operator API interface can be called directly.

This example uses the two-stage interfaces `aclnnAddCustomGetWorkspaceSize` and `aclnnAddCustom` to execute the operator computation. The core process is as follows:
1. Create input/output `aclTensor` objects and prepare device-side data.
2. Call `aclnnAddCustomGetWorkspaceSize` to obtain the workspace size required for this computation, and allocate the corresponding device memory.
3. Call `aclnnAddCustom` to execute the computation, synchronize the stream via `aclrtSynchronizeStream`, and copy the results back to the host side for verification.

## Build and Run

- Build, package, and deploy the custom operator project

  Before running this example, navigate to the [Custom Operator Project Example](../../00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following steps in the root directory of this example.

  ```bash
  mkdir -p build && cd build
  python3 ../scripts/gen_data.py
  cmake .. && make -j
  ./execute_add_op
  ```

  The following output indicates successful execution.

  ```text
  test pass
  ```
