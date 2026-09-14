# Overview<a name="ZH-CN_TOPIC_0000002013747665"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T11:49:43.910Z pushedAt=2026-09-03T02:56:02.033Z -->

Engineering-based operator development refers to a series of processes, including operator implementation, compilation and deployment, and automatic generation of single-operator call code, based on an automatically generated **custom operator project**.

This is a standard development process, and you are advised to develop operators by following it. In this way, the operator development code is more standardized, unified, and easy to maintain. In addition, this method takes into account the integration of features such as single-operator API calls, operator graph integration, and AI framework calls, making it easier for you to implement these features with the CANN framework.

The following figure shows the engineering-based operator development process:

![](../../../figures/operator_development_e2e_process.png)

1.  Set up the environment.
    1.  For CANN software installation, see [Environment Setup](../../../getting_started/environment_setup.md).
    2.  [Create an operator project](./operator_project_creation.md). Use the msOpGen tool to create an operator development project.

2.  Implement the operator.
    -   [Define the operator prototype](./operator_prototype_definition.md). Describe the operator inputs, outputs, attributes, and related implementation information on the AI processor through prototype definition, and associate functions such as the tiling implementation.
    -   For kernel-side operator implementation and host-side tiling implementation, see [SIMD Operator Implementation](../../../operator_practice/simd_operator_impl/overview.md). For engineering-based operator development, developers can call the tiling API to perform tiling development based on the programming framework provided by CANN. The kernel side also provides corresponding APIs for developers to obtain tiling parameters. For details, see [Kernel-side Operator Implementation](./kernel_side_operator_implementation.md) and [Host-side Tiling Implementation](./host_tiling_implementation/basic_process.md). The additional constraints introduced by this approach are also described in the preceding sections.

3.  Compile and deploy the operator through the project compilation script. Two methods are available: [operator package compilation](./operator_package_compilation/operator_project_compilation.md) and [operator dynamic library compilation](./operator_dynamic_and_static_library_compilation.md).
4.  [Call the single-operator API](./single_operator_api_call.md) to execute the operator using the C language API.
