# Overview<a name="ZH-CN_TOPIC_0000001985173748"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T11:31:49.131Z pushedAt=2026-09-01T06:41:27.756Z -->

Graph mode is a running mode of neural network models. In graph mode, you first construct the model computation process into a graph, and then deliver the graph to Ascend hardware through GE for execution. Compared with delivering operators one by one, in graph mode, GE can accelerate model execution efficiency and reduce model memory usage through techniques such as computation graph optimization, multi-stream parallelism, memory reuse, and model sinking.

The development process of integrating operators into a graph is shown in the following figure: after the operator project is created, complete the operator prototype definition, kernel-side operator implementation, host-side tiling implementation, and operator graph integration development based on the project code framework. Then compile and deploy the operator through the project compilation script, after which you can execute the operator based on the graph IR, for example, by calling the custom operator through IR graph construction. This development process is based on the [engineering-based operator development](../aclnn_operator_development/overview.md). In addition to the operator implementation files required in engineering-based operator development, you also need to deliver the code files for integrating operators into a graph.

![](../../../figures/operator_development_e2e_process_34.png)

1.  Set up the environment.
    1.  For CANN software installation, see [Environment Setup](../../../getting_started/environment_setup.md).
    2.  [Create an operator project](../aclnn_operator_development/operator_project_creation.md). Use the msOpGen tool to create an operator development project.

2.  Implement the operator.
    -   [Define the operator prototype](../aclnn_operator_development/operator_prototype_definition.md). Use the prototype definition to describe the operator inputs, outputs, attributes, and related implementation information on the AI processor, and associate functions such as the tiling implementation.
    -   For kernel-side operator implementation and host-side tiling implementation, see [SIMD Operator Implementation](../../../operator_practice/simd_operator_impl/overview.md). Engineering-based operator development allows developers to call the Tiling API to perform tiling development based on the programming framework provided by CANN. The kernel side also provides corresponding APIs for developers to obtain tiling parameters. For details, see the [kernel-side operator implementation](../aclnn_operator_development/kernel_side_operator_implementation.md) and [host-side tiling implementation](../aclnn_operator_development/host_tiling_implementation/basic_process.md). The additional constraints introduced by these are also described in the preceding sections.

3.  [Integrate the operator into a graph (GE graph)](basic_development_process.md). In the scenario of integrating operators into a graph, you need to provide the implementation of operator graph adaptation functions such as shape inference.
4.  Compile and deploy the operator. Use the project compilation script to compile and deploy the operator. There are two methods: [operator package compilation](../aclnn_operator_development/operator_package_compilation/operator_project_compilation.md) and [operator dynamic library compilation](../aclnn_operator_development/operator_dynamic_and_static_library_compilation.md).
5.  [Compile and execute the graph](graph_compilation_and_execution.md): Execute the operator based on graph IR, for example, call a custom operator by constructing a graph with IR.
