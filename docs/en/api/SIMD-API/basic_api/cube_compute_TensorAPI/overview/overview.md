# Overview

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T17:07:29.570Z -->

## Cube Computation Unit

For details about the cube computation unit, see [LINK](../../cube_compute_ISASI/overview/matrix_computation_unit.md).


## Cube Computation Workflow
Tensor API provides two types of APIs for the cube computation programming model, which respectively carry the data movement capability and computation capability of each path in the Cube core, as shown in the following figure:

**Figure 1**  Basic cube computation workflow<a name="zh-cn_topic_0000002535567224_fig135639216483"></a>

![Basic cube computation workflow](../../../../figures/cube_computation_workflow_tensor_api.png)

1. Use the `Copy` API to move the Global Memory data of the original matrices A and B to the L1 Buffer (if Bias/inline quantization exists, it is also moved to the L1 Buffer through `Copy`). For details, see [Cube Computation Input](../cube_compute_load/matrix_computation_load.md).

2. Use the `Copy` API to load matrices A and B into the L0A Buffer and L0B Buffer respectively for computation (if Bias/inline quantization exists, use `Copy` to move the Bias data/quantization coefficient data in the L1 Buffer to the BiasTable Buffer/Fixpipe Buffer). For details, see [Cube Computation Input](../cube_compute_load/matrix_computation_load.md).

3. Use the `Mmad` API to perform cube computation on the data in the L0A Buffer, L0B Buffer, and BiasTable Buffer, and output the result to the L0C Buffer. For details, see [Mmad Computation](../mmad_compute/mmad_computation.md).

4. Use the `Copy` API to process the data in the L0C Buffer and move it out to Global Memory. The `Copy` API can use the Fixpipe Buffer data to perform operations such as inline quantization and ReLU. For details, see [Cube Computation Output](../cube_compute_store/matrix_computation_storage.md).
