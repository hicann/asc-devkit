# Overview

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T03:17:43.698Z pushedAt=2026-10-27T13:03:35.363Z nl=2 -->

## Cube Computation Unit

For details about the cube computation unit, see [LINK](../../basic_api/cube_compute_ISASI/overview/cube_compute_unit.md).

## Cube Computation Workflow

The Tensor API provides two types of APIs for the cube computation programming model, which respectively carry the movement capability and computation capability of each path in the Cube core, as shown in the following figure:

**Figure 1**  Basic cube computation workflow<a name="zh-cn_topic_0000002535567224_fig135639216483"></a>

![Basic cube computation workflow](../../../figures/matrix_computation_workflow_tensor_api.png)

1. Use the `copy` API to move the Global Memory data of the original cubes A and B to the L1 Buffer (if Bias/in-place quantization exists, it is also moved to the L1 Buffer through `copy`). For details, see [Loading for Cube Computation](cube_compute_load/cube_compute_load.md).

2. Use the `copy` API to load cubes A and B into the L0A Buffer and L0B Buffer respectively to prepare for computation (if Bias/in-place quantization exists, use `copy` to move the Bias data/quantization coefficient data in the L1 Buffer to the BiasTable Buffer/Fixpipe Buffer). For details, see [Loading for Cube Computation](cube_compute_load/cube_compute_load.md).

3. Use the `mmad` API to perform cube computation on the data in the L0A Buffer, L0B Buffer, and BiasTable Buffer, and output the result to the L0C Buffer. For details, see [mmad Computation](mmad_compute/mmad_compute.md).

4. Use the `copy` API to process the data in the L0C Buffer and move it out to Global Memory. The `copy` API can use the Fixpipe Buffer data to perform operations such as in-place quantization and Relu. For details, see [Storing for Cube Computation](cube_compute_store/cube_compute_store.md).
