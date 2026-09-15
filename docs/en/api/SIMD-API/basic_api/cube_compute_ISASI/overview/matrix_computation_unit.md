# Cube Computation Unit<a name="ZH-CN_TOPIC_0000002538071190"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T15:06:45.317Z -->

The Cube compute unit is dedicated to executing cube operations. The dedicated buffers it directly accesses are as follows: the L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the initialized accumulation values and the cube computation results.

<!-- npu="910b,A3,950" id1 -->
The highlighted part in the following figure shows the Cube compute unit and the dedicated buffers it directly accesses.
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
**Figure 1** [NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) cube computation unit architecture diagram<a name="zh-cn_topic_0000002513373312_fig88330137205"></a>  
![](../../../../figures/architecture_of_cube_compute_unit_a2a3.png "Cube computation unit architecture diagram - A2A3")
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** [NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) cube computation unit architecture diagram<a name="zh-cn_topic_0000002513373312_fig1082103513519"></a>  
![](../../../../figures/architecture_of_cube_compute_unit_950.png "Cube computation unit architecture diagram - 950")
<!-- end id3 -->

The storage units associated with cube computation are as follows:

| Buffer Type | Description |
| ----------- | ------ |
| L0A Buffer | Internal physical storage unit of the AI Core, typically used to store the left cube for cube computation. |
| L0B Buffer | Internal physical storage unit of the AI Core, typically used to store the right cube for cube computation. |
| L0C Buffer | Internal physical storage unit of the AI Core, typically used to store the results of cube computation and the initialized accumulation values. |
| L1 Buffer | Internal physical storage unit of the AI Core with relatively large capacity, typically used to cache the input data for cube computation. The input for cube computation generally needs to be moved from GM to the L1 Buffer, and then moved to the L0A Buffer and L0B Buffer respectively. |
| Fixpipe Buffer | Internal physical storage unit of the AI Core, typically used to store data such as the quantization parameters required during Fixpipe transfer. |
| BiasTable Buffer | Bias storage, an internal physical storage unit of the AI Core, typically used to store the bias data required for cube computation. |
