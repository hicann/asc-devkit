# Custom Operator Project + ONNX Model Invocation Example

## Overview

This example uses the LeakyRelu computation to demonstrate how to invoke a custom operator through an ONNX network.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── onnx_plugin
│   ├── CMakeLists.txt           // Build project file
│   ├── leaky_relu.py            // Script to generate the LeakyRelu ONNX model
│   ├── main.cpp                 // Operator invocation main program (loads the OM model to execute LeakyRelu inference)
│   └── README.md                // Example documentation
```

## Example Implementation

After completing the development and deployment of the custom operator, the example functionality can be verified through ONNX model invocation. After converting the LeakyRelu ONNX model to an OM model, the application can load the model and execute inference.

This example uses the `aclmdlExecute` interface to perform model inference. The core process is as follows:
1. Load the OM model via `aclmdlLoadFromFile` and obtain the model ID, then call `aclmdlGetDesc` to retrieve model description information.
2. Call `aclmdlCreateDataset` to create input/output datasets, and allocate device memory based on `aclmdlGetInputSizeByIndex`/`aclmdlGetOutputSizeByIndex` to create DataBuffers.
3. Construct host-side input data, copy it to the device side via `aclrtMemcpy`, call `aclmdlExecute` to perform inference, and copy the results back to the host side for result verification.

## Build and Run

  Run the following steps in the root directory of this example to build and execute the example.
  - Build, package, and deploy the custom example project

    Before running this example, navigate to the [Custom Operator Project Example](../../../99_acl_based/00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

- Install ONNX

  ```bash
  pip3 install onnx==1.16.0
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build; cd build
  python3 ../leaky_relu.py
  # Use the ATC model conversion tool to convert the *.onnx model to a *.om model
  atc --model=./leaky_relu.onnx --framework=5 --soc_version=${soc_version} --output=./leaky_relu_custom --input_shape="X:8,16,1024" --input_format=ND
  cmake .. && make -j
  ./execute_leaky_relu_op
  ```

  > Obtain the AI processor model `<soc_version>` as follows:
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
  >   Example projects created based on AI processor models of the same series share the same basic functionality (example development, compilation, and deployment based on the project).

  The following output indicates successful execution.

  ```text
  test pass
  ```
