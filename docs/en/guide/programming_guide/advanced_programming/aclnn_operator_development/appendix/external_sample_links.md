# External Sample Links

<!-- md-trans-meta sourceCommit=78baa767400d3af08b5a397cacdcc8b46362de90 translatedAt=2026-11-09T10:45:01.233Z pushedAt=2026-11-19T06:32:19.851Z nl=2 -->

This document is for reference and summarizes the external code samples and API reference links cited in the guide, making it easier for developers to quickly find and reference them.

## Code Samples

### Basic Operator Implementation

- AddCustom sample in the [AddCustom operator sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op).
  - Description: This sample demonstrates a basic complete operator implementation, including TilingData definition, tiling function, and kernel function code.

### Generating a Custom Operator Project Template Using the msOpGen Tool

- [msOpGen sample](https://gitcode.com/cann/asc-tools/tree/master/examples/03_msopgen).
  - Description: This sample demonstrates how to use the msOpGen tool to generate a custom operator project template based on the AddCustom operator, and verifies the generated custom operator through an ACLNN call example.

### Passing Attributes Through TilingData

- leaky_relu_custom sample in [LeakyReluCustom](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/op_host/leaky_relu_custom).
  - Description: This sample describes passing attribute information through **TilingData**.

### Template Programming Operator Implementation

- [Kernel function template programming sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op/op_host/add_custom_template).
  - Description: This sample demonstrates an example of using kernel function template parameters to implement compile-time branch control.

### Compilation and Integration

- [Static library integration and usage sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib).
  - Description: This sample shows how to compile an operator into a static library and integrate it into an application.

- [aclnn single-operator call sample](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation).
  - Description: This sample shows how to call a custom operator through the aclnn API.

## API Reference

### Tiling API

- [Tiling data structure](../../../../../api/Utils-API/Tiling_data_structure_register/Tiling_data_structure_register.md).
  - Description: Reference for the tiling data structure.

- [GetLibApiWorkSpaceSize](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md).
  - Description: Method for obtaining the size of the workspace reserved by the Ascend C API.

### Template Programming API

- [Template parameter definition](../../../../../api/Utils-API/Tiling_template_programming/template_params_definition.md).
  - Description: Definitions of template parameter macros such as **ASCENDC_TPL_ARGS_DECL** and **ASCENDC_TPL_ARGS_SEL**.

### Prototype Registration API

- [OpAICoreConfig registration API](../../../../../api/Utils-API/prototype_register_management/REGISTER_OP_AICORE_CONFIG.md).
  - Description: The **OpAICoreConfig** class is used for differentiated prototype registration across multiple hardware platforms.

## Other References

- [Graph Development](https://gitcode.com/cann/ge/blob/master/docs/zh/user_guides/graph_dev/README.md).
  - Description: A complete guide to Ascend Graph development, applicable to operator graph integration scenarios.
