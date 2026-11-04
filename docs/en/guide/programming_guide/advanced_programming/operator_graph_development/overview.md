# Overview<a name="ZH-CN_TOPIC_0000001985173748"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T14:41:53.053Z pushedAt=2026-11-03T07:05:13.435Z nl=2 -->

Graph mode is a running mode of neural network models. In graph mode, you first construct the model computation process into a graph, and then deliver the graph to Ascend hardware through GE for execution. Compared with delivering individual operators one by one, in graph mode, GE can accelerate model execution efficiency and reduce model memory usage through technical means such as computational graph optimization, multi-stream parallelism, memory reuse, and model sinking.

The development process of integrating operators into a graph is shown in the following figure: after the operator project is created, complete the operator prototype definition, kernel-side operator implementation, host-side tiling implementation, and operator graph integration development based on the project code framework. Then compile and deploy the operator through the project compilation script. After that, you can execute operators based on graph IR, for example, calling custom operators through IR graph construction. This development process is based on [project-based operator development](../aclnn_operator_development/overview.md). In addition to providing the operator implementation files required in project-based operator development, you also need to deliver the code files for integrating operators into a graph.

![](../../../figures/op_e2e_34.png)

1.  Set up the environment.
    1.  For CANN software installation, see [Environment Setup](../../../getting_started/environment_setup.md).
    2.  [Create an operator project](../aclnn_operator_development/aclnn_quick_start.md#create-project). Use the msOpGen tool to create an operator development project.

2.  Implement the operator.
    -   [Operator prototype definition](../aclnn_operator_development/design_and_implementation/operator_prototype_definition.md). Use the prototype definition to describe the operator input, output, attributes, and other information, as well as the related implementation information of the operator on the AI processor, and associate functions such as the tiling implementation.
    -   For kernel-side operator implementation and host-side tiling implementation, see [SIMD operator implementation](../../../operator_practice/simd_operator_impl/overview.md). Project-based operator development supports developers in calling Tiling APIs to perform tiling development based on the programming framework provided by CANN. The kernel side also provides corresponding APIs for developers to obtain tiling parameters. For details, see [Kernel-side operator implementation](../aclnn_operator_development/design_and_implementation/kernel_operator_implementation.md) and [Host-side tiling implementation](../aclnn_operator_development/design_and_implementation/host_tiling_implementation.md). The additional constraints introduced by this are also described in the preceding sections.

3.  [Integrate the operator into a graph (GE graph)](basic_development_flow.md). In the operator graph integration scenario, you need to provide the implementation of operator graph integration adaptation functions such as shape inference.
4.  Perform compilation and deployment. Compile and deploy the operator through the project compilation script. There are two methods: [operator package compilation](../aclnn_operator_development/compilation_and_deployment/basic_process.md) and [operator dynamic library compilation](../aclnn_operator_development/compilation_and_deployment/dynamic_static_lib_compilation.md).
5.  Perform [graph compilation and execution](graph_compilation_and_execution.md): Execute operators based on the graph IR, for example, call custom operators through IR graph construction.
