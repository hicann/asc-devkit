# Overview

<!-- md-trans-meta sourceCommit=693fbd4c936677f1c45b391eccc6b9b290f0c189 translatedAt=2026-10-23T13:30:30.021Z pushedAt=2026-11-03T07:05:13.392Z nl=2 -->

This document provides basic information, describing the scope, reading order, and prerequisites of the Aclnn operator engineering development documentation set. It is recommended that you first confirm that the environment meets the requirements, then follow [Quick Start](./aclnn_quick_start.md) to run the AddCustom sample. After running it successfully, return to the corresponding sections to fill in the details step by step.


## What Is Engineering-Based Operator Development

### Key Terms

The following core concepts are introduced first:

| Name | Meaning |
|------|------|
| Custom operator project | A project skeleton written or generated based on the operator prototype definition, including the prototype definition, Tiling implementation, Kernel implementation, and CMake build configuration. |
| Operator prototype | Describes the operator's inputs, outputs, attributes, data types, formats, Shape derivation, and other information. |
| Host side | The preparation logic running on the CPU side, such as Shape derivation, Tiling computation, and workspace size setting. |
| Kernel side | The computation logic running on the AI Core, which is the part where the operator actually performs computation. |
| Tiling | Splits computation tasks based on information such as the input Shape and data type, and passes the parameters required by the Kernel to the Kernel side. |
| aclnn single operator API | A C language API automatically generated based on the operator prototype after the operator project is compiled. Applications can directly call the custom operator through this API. |

### What Is Engineering-based Development

Engineering-based operator development refers to a development approach that completes operator implementation, compilation and deployment, and call verification based on a **custom operator project**, mainly targeting CANN-related scenario integration: the single operator call scenario is integrated through the aclnn API, and the operator graph scenario is integrated through the GE graph execution link. Through engineering-based development, the operator prototype, Host-side Tiling, kernel function implementation, and compilation and deployment configuration can maximize the reuse of the same deliverable across the two scenarios. Compared with manually organizing the compilation process and separately integrating the call links, engineering-based development uses msOpGen to generate the project skeleton and CMake to manage compilation. The compilation output automatically includes the aclnn API and the files such as the prototype definition required for graph integration. During deployment, you only need to install the .run package or link the static/dynamic library.

### Integration Methods

Around the same operator project, the common integration methods are as follows:

| Integration Method | Description | Application Scenario |
|---------|------|---------|
| Single-Operator API Call | Directly call the operator through the generated aclnn API and execute the operator based on the C language API. | Operator function verification and single-operator call. |
| Operator Graph Integration | After adding graph integration adaptation code such as shape inference, execute the custom operator through the GE graph execution link. | Graph mode, IR graph construction, and multi-operator combination. |
| AI Framework Call | After adding framework plugin adaptation on top of the engineered operator, call the custom operator through frameworks such as PyTorch/TensorFlow. | Integration into AI training or inference processes. |

## Recommended Reading Path

If you are new to this topic, start with [Quick Start](./aclnn_quick_start.md) and run through the end-to-end process of the AddCustom operator in five minutes. After that, follow the path below to fill in the details step by step.

This guide is organized by development process into design and implementation, compilation and deployment, call verification, and reference documentation. For your first read, focus only on the `[Basic]` documents, and refer to the `[Extended]` documents when you encounter specific issues.

### Design and Implementation

`[Basic]` (read in order):
- [Operator Function Design](./design_and_implementation/operator_function_design.md) — Supported scope, scenario division, and kernel function organization.
- [Operator Prototype Definition](./design_and_implementation/operator_prototype_definition.md) — Input/output attribute definition and shape inference.
- [Host-Side Tiling Implementation](./design_and_implementation/host_tiling_implementation.md) — Tiling functions and data structures, and usage constraints.
- [Kernel Function-Side Operator Implementation](./design_and_implementation/kernel_operator_implementation.md) — Kernel function programming and API usage.

`[Advanced]` (refer to when you encounter specific issues):
- [Passing Attribute Information Through TilingData](./design_and_implementation/tiling_data_attributes.md) — Applicable to scenarios where operator attributes need to be passed to the kernel function side.
- [Multi-Branch Strategy](./design_and_implementation/multi_branch_strategy.md) — Applicable to scenarios where the operator needs to split kernel function entries by scenario.
- [GenSimplifiedKey Callback Implementation](./design_and_implementation/gen_simplified_key.md) — Applicable to scenarios where a precompiled binary needs to be selected by a custom key.
- [Tiling implementation for use with advanced APIs](./design_and_implementation/tiling_with_advanced_api.md) — applicable to scenarios where complex operators such as Matmul/Conv are developed.
- [Enabling Tiling Sink](../operator_graph_development/enable_tiling_sink.md) — applicable to scenarios where Tiling sink needs to be enabled in the operator graph scenario.

### Compilation and Deployment

`[Basic]`:
- [Basic Compilation and Deployment Process](./compilation_and_deployment/basic_process.md) — Compilation configuration, operator package packaging, and deployment.

`[Advanced]`:
- [Multi-Operator Package Organization](./compilation_and_deployment/multi_operator_package.md) — Multi-operator package splitting strategy.
- [Compiling Operator Dynamic and Static Libraries](./compilation_and_deployment/dynamic_static_lib_compilation.md) — Applicable to scenarios where operators need to be integrated into applications as libraries.
- [Cross Compilation](./compilation_and_deployment/cross_compilation.md) — Applicable to scenarios where the development environment and runtime environment have different architectures.
- [Compilation Process Debugging](./compilation_and_deployment/compilation_debug.md) — Applicable to scenarios where compilation issues are encountered.
- [Compilation Acceleration](./compilation_and_deployment/compilation_acceleration.md) — Applicable to scenarios where build acceleration is required.
- [Simplified Key Binary Configuration JSON](./compilation_and_deployment/simplified_key_json_configuration.md) — Applicable to scenarios where multiple binaries are compiled by custom keys.

### Call Verification

`[Basic]`:
- [Single Operator API Call](./invocation/single_operator_api_call.md) — Verifies the operator function by calling the aclnn API.

`[Advanced]`:
- [Runtime Loading Mechanism](./invocation/runtime_loading_mechanism.md) — Applicable to scenarios where you need to understand the two-stage call model and how code writing affects runtime behavior.
- [Runtime Selection Process Based on Simplified Key](./invocation/simplified_key_runtime_flow.md) — Applicable to scenarios where you need to understand how the runtime selects a precompiled binary.

### Reference Documents

The following content is for reference and can be retrieved as needed during development:

- [Naming Conversion Table](./appendix/naming_conversion_table.md) — Conversion rules for operator type names, file names, and kernel function names.
- [CMake Function Reference](./appendix/cmake_function_reference.md) — Detailed description of CMake function parameters and usage.
- [External Sample Link Summary](./appendix/external_sample_links.md) — Summary of external samples and API links referenced in the documentation.
