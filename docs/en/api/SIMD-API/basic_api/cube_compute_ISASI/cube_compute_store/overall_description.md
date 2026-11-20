# Overview<a name="ZH-CN_TOPIC_0000002538231192"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-11-02T03:59:53.019Z pushedAt=2026-11-18T01:25:06.745Z nl=2 -->

The copy-out of matrix computation is a core category of APIs used for data movement in the Ascend C programming framework. It mainly implements efficient data transfer between the L0C Buffer and Global Memory, L1 Buffer, and Unified Buffer (UB). This API family provides multiple data movement modes to meet the data movement requirements of matrix computation, and is typically used together with the Mmad API.

The copy-out APIs of matrix computation support multiple inline movement scenarios, including **inline quantization, inline ReLU, inline format conversion (NZ2ND, NZ2DN), inline channel split, inline channel merge, and UB dual-destination mode**. By flexibly configuring different parameter structures, developers can precisely control key information such as the data movement path, start position, movement length, and inline movement capability, fully leveraging the hardware data movement capability to maximize the overall execution performance of the operator.

<!-- npu="950,A3,910b" id1 -->
The overall flow of the copy-out of matrix computation is shown in the following figure:
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
**Figure 1** Overall flow of the copy-out of matrix computation ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_execution_flow_a2a3.png)
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** Overall flow of the copy-out of matrix computation ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_execution_flow_a5.png)
<!-- end id3 -->

The memory layout of the Fixpipe Buffer under different quantization modes is as follows:

**Figure 3** Fixpipe Buffer memory layout

![](../../../../figures/fixpipe_buffer.png)
