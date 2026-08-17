# Custom Operator Project + aclop Single-Operator Model Invocation Example

## Overview

This example introduces how to execute a fixed-shape operator using the `aclopExecuteV2` single-operator model approach, based on the sample custom operator project.

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
├── aclop_invocation
│   ├── add_custom.json         // Single-operator description file (ATC model conversion input)
│   ├── CMakeLists.txt          // Build project file
│   ├── main.cpp                // Operator invocation main program
│   └── README.md               // Example documentation
```

## Code Implementation

After completing the development and deployment of the custom operator, the single-operator functionality can be verified through single-operator model invocation. For details, refer to the "Single-Operator Mode Execution" section in [Single-Operator Invocation](https://hiascend.com/document/redirect/CannCommunityCppOpcall).

For single-operator model invocation, the single-operator offline model must be generated in advance via `atc --singleop` in offline mode, and the model directory must be configured in the application.

This example uses the `aclopExecuteV2` interface to execute the single-operator model. The core process is as follows:
1. Create input/output tensor descriptions via `aclCreateTensorDesc` and prepare device-side data buffers using `aclCreateDataBuffer`.
2. Call `aclopSetModelDir` to specify the model directory, then use `aclopExecuteV2` to execute the operator computation.
3. Synchronize the stream via `aclrtSynchronizeStream` and copy the device-side results back to the host side for result verification.

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
  # Use the ATC model conversion tool to generate the single-operator offline model
  atc --singleop=../add_custom.json --output=. --soc_version=${soc_version}
  cmake .. && make -j
  ./execute_add_op
  ```

  > Obtain the AI processor model `${soc_version}` as follows:
  > - For the following product models: Run the `npu-smi info` command on the server with the Ascend AI processor installed to query the **Name** information. The actual configuration value is AscendName. For example, if the **Name** value is xxxyy, the actual configuration value is Ascendxxxyy.
  >   - Atlas A2 Training Series Products / Atlas A2 Inference Series Products
  >   - Atlas 200I/500 A2 Inference Products
  >   - Atlas Inference Series Products
  >   - Atlas Training Series Products
  >
  > - For the following product models: Run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server with the Ascend AI processor installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234. Where:
  >
  >   id: device ID, obtained from the NPU ID queried by the `npu-smi info -l` command
  >
  >   chip_id: chip ID, obtained from the Chip ID queried by the `npu-smi info -m` command
  >   - Ascend 950PR/Ascend 950DT
  >   - Atlas A3 Training Series Products / Atlas A3 Inference Series Products
  >
  >   Operator projects created based on AI processor models of the same series share the same basic functionality (operator development, compilation, and deployment based on the project).

  The following output indicates successful execution.

  ```text
  test pass
  ```
