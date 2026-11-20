# Overview<a name="ZH-CN_TOPIC_0000001985173748"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:55:03.970Z pushedAt=2026-11-19T06:32:19.907Z nl=2 -->

Graph mode is a running mode of neural network models. In graph mode, you first construct the model computation process into a graph, and then GE delivers the graph to Ascend hardware for execution. Compared with delivering operators one by one, in graph mode, GE can accelerate model execution and reduce model memory usage through technical means such as computation graph optimization, multi-stream parallelism, memory reuse, and model sinking.

The development process of integrating operators into a graph is shown in the following figure: after the operator project is created, based on the project code framework, you complete the operator prototype definition, kernel function implementation, host-side tiling implementation, and operator graph integration development. Then, compile and deploy the operator through the project build script. After that, you can execute the operator based on graph IR, for example, by calling the custom operator through IR graph construction. This development process is based on [engineering operator development](../aclnn_operator_development/overview.md). In addition to the operator implementation files required in engineering operator development, you also need to deliver the code files for integrating operators into a graph.

<img src="../../../figures/op_e2e_34.png" alt="" width="920px" height="281px">

1.  Set up the environment.
    1.  For CANN software installation, see [Environment Setup](../../../getting_started/environment_setup.md).
    2.  [Create an operator project](../aclnn_operator_development/aclnn_quick_start.md#create-project). Use the msOpGen tool to create an operator development project.

2.  Implement the operator.
    -   [Define the operator prototype](../aclnn_operator_development/design_and_implementation/operator_prototype_definition.md). Use the prototype definition to describe the operator inputs, outputs, attributes, and related implementation information on the AI processor, and associate functions such as the tiling implementation.
    -   For kernel function implementation and host-side tiling implementation, see [SIMD operator implementation](../../../operator_practice/simd_operator_impl/overview.md). Engineering operator development supports developers in calling tiling APIs to perform tiling development based on the programming framework provided by CANN. The kernel function side also provides corresponding APIs for developers to obtain tiling parameters. For details, see [kernel-side operator implementation](../aclnn_operator_development/design_and_implementation/kernel_operator_implementation.md) and [host-side tiling implementation](../aclnn_operator_development/design_and_implementation/host_tiling_implementation.md). The additional constraints introduced by this are also described in the preceding sections.

3.  [Integrate the operator into a GE graph](basic_development_flow.md). In the operator graph integration scenario, you need to provide the implementation of operator graph integration adaptation functions such as shape inference.
4.  Compile and deploy the operator. Use the project compilation script to compile and deploy operators. There are two methods: [operator package compilation](../aclnn_operator_development/compilation_and_deployment/basic_process.md) and [operator dynamic library compilation](../aclnn_operator_development/compilation_and_deployment/dynamic_static_lib_compilation.md).
5.  [Compile and execute the graph](graph_compilation_and_execution.md): Execute operators based on graph IR, for example, call custom operators through IR-based graph construction.
