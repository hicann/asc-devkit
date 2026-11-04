# External Sample Link Summary

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T13:34:06.780Z pushedAt=2026-11-03T07:05:13.393Z nl=2 -->

This document is for reference and summarizes the external code samples and API reference links cited in the guide, making it easier for developers to quickly find and reference them.

## Code Samples

### Basic Operator Implementation

- [AddCustom operator sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op) in the AddCustom sample.
  - Note: Demonstrates a basic complete operator implementation, including TilingData definition, Tiling function, and kernel function code.

### Generating a Custom Operator Project Template Using the msOpGen Tool

- [msOpGen sample](https://gitcode.com/cann/asc-tools/tree/9.2.0/examples/03_msopgen).
  - Description: Demonstrates how to use the msOpGen tool to generate a custom operator project template based on the AddCustom operator, and verifies the generated custom operator through an ACLNN call example.

### Passing Attributes Through TilingData

- The leaky_relu_custom sample in [LeakyReluCustom](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/op_host/leaky_relu_custom).
  - Description: Passes attribute information through TilingData.

### Template Programming Operator Implementation

- [Kernel template programming sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/op_host/add_custom_template).
  - Description: Demonstrates an example of using Kernel template parameters to implement compile-time branch control.

### Compilation and Integration Examples

- [Static library integration and usage example](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib).
  - Description: Shows how to compile an operator into a static library and integrate it into an application.

- [aclnn single-operator invocation example](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation).
  - Description: Shows how to invoke a custom operator through the aclnn API.

## API Reference

### Tiling API

- [Tiling data structure](../../../../../api/Utils-API/Tiling_data_structure_register/Tiling_data_structure_register.md).
  - Note: Reference for the Tiling data structure.

- [GetLibApiWorkSpaceSize](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md).
  - Note: Method for obtaining the size of the workspace reserved by the Ascend C API.

### Template Programming API

- [Template Parameter Definition](../../../../../api/Utils-API/Tiling_template_programming/template_params_definition.md).
  - Description: Definitions of template parameter macros such as ASCENDC_TPL_ARGS_DECL and ASCENDC_TPL_ARGS_SEL.

### Prototype Registration API

- [OpAICoreConfig registration API](../../../../../api/Utils-API/prototype_register_management/REGISTER_OP_AICORE_CONFIG.md).
  - Description: The OpAICoreConfig class is used for differentiated prototype registration across multiple hardware platforms.

## Other References

- [Graph Development](https://gitcode.com/cann/ge/blob/9.2.0/docs/en/user_guides/graph_dev/README.md).
  - Description: A complete guide to Ascend Graph development, applicable to operator graph integration scenarios.
