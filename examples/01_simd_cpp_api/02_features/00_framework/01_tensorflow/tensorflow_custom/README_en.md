# Custom Operator Project + TensorFlow Custom Operator Example

## Overview

This example demonstrates how to map the Ascend C custom operator Add computation to a TensorFlow custom operator and invoke the Ascend C operator through TensorFlow.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tensorflow_custom
│   ├── CMakeLists.txt                // Build project file
│   ├── custom_assign_add_custom.cc   // TensorFlow custom operator implementation
│   ├── run_add_custom_tf.py          // TensorFlow invocation script (mapped to the custom AddCustom operator)
│   └── README.md                     // Example documentation
```

## Code Implementation

After the operator project is created, a framework/tf_plugin directory is generated under the operator project directory to store the TensorFlow framework adaptation plugin implementation files.

This example uses the custom operator Add and maps it to a TensorFlow custom operator. The core process is as follows:
1. Load the custom operator library file via `tf.load_op_library` to obtain the custom operator interface `add_custom`.
2. Construct input data, use `tf.compat.v1.placeholder` to define input tensors, and compute the results of `tf.math.add` and `add_custom` respectively.
3. Configure `ConfigProto`, enable `NpuOptimizer`, and disable remapping and memory optimization to ensure the operator executes as expected.
4. Call `np.allclose` to compare the results of the standard TensorFlow addition operator and the custom operator to verify computation correctness.

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.
- Build, package, and deploy the custom operator project

  Before running this example, navigate to the [Custom Operator Project Example](../../../99_acl_based/00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

  > [!NOTE] Notice
  > The plugin code needs to be adapted. The path is: `examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/framework/tf_plugin/tensorflow_add_custom_plugin.cc`. Modify the TensorFlow invocation operator name OriginOpType in the plugin code to "AddCustom" as shown below:
  >
  > ```cpp
  > REGISTER_CUSTOM_OP("AddCustom")
  >   .FrameworkType(TENSORFLOW)      // type: TENSORFLOW
  >   .OriginOpType("AddCustom")      // name in tf module
  >   .ParseParamsByOperatorFn(AutoMappingByOpFn);
  > ```
    Before running this example, navigate to the [Custom Operator Project Example](../../../99_acl_based/00_acl_compilation/custom_op) directory to complete building, packaging, and deployment.

    > [!NOTE] Notice
    > The plugin code needs to be adapted. The path is: `examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/framework/tf_plugin/tensorflow_add_custom_plugin.cc`. Modify the TensorFlow invocation operator name OriginOpType in the plugin code to "AddCustom" as shown below:
    >
    > ```cpp
    > REGISTER_CUSTOM_OP("AddCustom")
    >   .FrameworkType(TENSORFLOW)      // type: TENSORFLOW
    >   .OriginOpType("AddCustom")      // name in tf module
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
  mkdir -p build; cd build
  cmake .. && make -j
  python3 ../run_add_custom_tf.py
  ```

  The following output indicates successful execution.

  ```text
  test pass
  ```
