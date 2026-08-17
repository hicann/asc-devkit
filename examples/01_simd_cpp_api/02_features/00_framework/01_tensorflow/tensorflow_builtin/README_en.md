# Custom Example Project + TensorFlow Built-in Operator Example

## Overview

This example demonstrates how to map an Ascend C custom operator to a TensorFlow built-in operator based on the Add computation, and invoke it through TensorFlow.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tensorflow_builtin
│   ├── run_add_custom.py       // TensorFlow invocation script (mapped to the built-in AddV2 operator)
│   └── README.md               // Example documentation
```

## Code Implementation

After the example project is created, a framework/tf_plugin directory is generated under the project directory to store the TensorFlow framework adaptation plugin implementation files.

This example demonstrates mapping the Add computation to the TensorFlow built-in operator AddV2. The core process is as follows:
1. Use `np.random.uniform` to generate random input data and define input nodes via `tf.compat.v1.placeholder`.
2. Build the computation graph: use `tf.math.add` to perform tensor addition.
3. Create CPU and NPU sessions respectively, and execute the computation graph via `session.run` with input data.
4. Use `np.allclose` to compare the NPU and CPU computation results to verify computation correctness.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.
- Build, package, and deploy the custom example project

  Before running this example, navigate to the [Custom Example Project Example](../../../99_acl_based/00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

  > [!NOTE] Notice
  > The plugin code needs to be adapted. The path is: `examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/framework/tf_plugin/tensorflow_add_custom_plugin.cc`. Modify the TensorFlow invocation operator name OriginOpType in the plugin code to "AddV2" as shown below:
  >
  > ```cpp
  > REGISTER_CUSTOM_OP("AddCustom")
  >   .FrameworkType(TENSORFLOW)      // type: TENSORFLOW
  >   .OriginOpType("AddV2")      // name in tf module
  >   .ParseParamsByOperatorFn(AutoMappingByOpFn);
  > ```

- Install the TensorFlow plugin package

  Refer to the "Install Framework Plugin Package" section in [TensorFlow 2.6.5 Model Migration](https://www.hiascend.com/document/redirect/canncommercial-tfmigr26) for detailed installation guide and steps.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  python3 run_add_custom.py
  ```

  The following output indicates successful execution.

  ```text
  test pass
  ```
