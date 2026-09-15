# Overview<a name="ZH-CN_TOPIC_0000002538231192"></a>

<!-- md-trans-meta sourceCommit=5afce9079f40f95eb7b22e64d4da4de38a1a5378 translatedAt=2026-08-27T15:36:51.201Z -->

The copy-out of cube computation is a core category of APIs for data movement in the Ascend C programming framework. It mainly implements efficient data transfer between the L0C Buffer and Global Memory, L1 Buffer, and Unified Buffer (UB). This API series provides multiple data movement modes to meet the data movement requirements in cube computation, and is typically used in conjunction with the Mmad API.

The copy-out APIs of cube computation support multiple inline movement scenarios, including **inline quantization, inline ReLU, inline format conversion (NZ2ND, NZ2DN), inline channel split, inline channel merge, and UB dual-destination mode**. By flexibly configuring different parameter structures, developers can precisely control key information such as the data movement path, start position, movement length, and inline movement capabilities, fully leveraging the hardware data movement capability to maximize the overall execution performance of the operator.

<!-- npu="950,A3,910b" id1 -->
The overall process of cube computation copy-out is shown in the following figure:
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
**Figure 1** Overall process of cube computation copy-out ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))

![](../../../../figures/fixpipe_execution_flow_a2a3.png)
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** Overall process of cube computation copy-out ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md))

![](../../../../figures/fixpipe_execution_flow_a5.png)
<!-- end id3 -->

The memory layout of the Fixpipe Buffer corresponding to different quantization modes is as follows:

**Figure 3** Fixpipe Buffer memory layout

![](../../../../figures/fixpipe_buffer.png)
