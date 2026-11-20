# Matrix Computation Unit<a name="ZH-CN_TOPIC_0000002538071190"></a>

<!-- md-trans-meta sourceCommit=2f9021e91c029d6fd00e108fa5e17d2275791fa1 translatedAt=2026-11-02T04:45:03.393Z pushedAt=2026-11-18T01:25:06.823Z nl=2 -->

The Cube compute unit is dedicated to executing matrix computation. The dedicated caches it directly accesses are as follows: **L0A Buffer** is used to store the left matrix, **L0B Buffer** is used to store the right matrix, and **L0C Buffer** is used to store the initialization accumulation values and matrix computation results.

<!-- npu="910b,A3,950" id1 -->
The highlighted part in the following figure shows the Cube compute unit and the dedicated caches it directly accesses.
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
**Figure 1** [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) matrix computation unit architecture diagram<a name="zh-cn_topic_0000002513373312_fig88330137205"></a>  
![](../../../../figures/architecture_of_cube_compute_unit_a2a3.png "Matrix Computation Unit Architecture Diagram - A2A3")
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) matrix computation unit architecture diagram<a name="zh-cn_topic_0000002513373312_fig1082103513519"></a>  
![](../../../../figures/npu_3510_hw_arch_cube.png "Matrix Computation Unit Architecture Diagram - 950")
<!-- end id3 -->

The storage units associated with matrix computation are as follows:

| Buffer Type | Description |
| ----------- | ------ |
| L0A Buffer | Internal physical storage unit of the AI Core, typically used to store the left matrix for matrix computation. |
| L0B Buffer | Internal physical storage unit of the AI Core, typically used to store the right matrix for matrix computation. |
| L0C Buffer | Internal physical storage unit of the AI Core, typically used to store the results of matrix computation and the matrix accumulation initialization values. |
| L1 Buffer | Internal physical storage unit of the AI Core with a relatively large capacity, typically used to cache the input data for matrix computation. The input for matrix computation generally needs to be moved from GM to the L1 Buffer, and then moved to the L0A Buffer and L0B Buffer respectively. |
| Fixpipe Buffer | Internal physical storage unit of the AI Core, typically used to store data such as the quantization parameters required during Fixpipe data movement. |
| BiasTable Buffer | Bias storage, an internal physical storage unit of the AI Core, typically used to store the bias data required for matrix computation. |
