# On-the-Fly ReLU<a name="ZH-CN_TOPIC_0000002538231206"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:35:23.843Z -->

## Description

The storage process of cube computation supports the on-the-fly ReLU capability. The following on-the-fly ReLU capabilities are currently supported. In the **DataCopyCO12DstParams** structure parameter definition, **reluPre** can be set to 1 to enable Normal ReLU, and combined with **clipReluPre**, the ClipReLU capability can be further enabled.

- When the parameter is set to 0, the on-the-fly ReLU capability is not enabled.

- When the parameter is set to 1, Normal ReLU is enabled. No additional register configuration is required, and the activation operation `y = max(0, x)` is performed on the output data.

- When Normal ReLU is enabled (**reluPre**=1), **clipReluPre**=1 is configured at the same time, and the SetFixPipeClipRelu API is called, Normal ReLU + ClipReLU is enabled. In this case, `y = max(0, x)` is performed first, followed by `y = min(clipReluMaxVal, y)`, where **clipReluMaxVal** is set through the SetFixPipeClipRelu API. For details, see [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md).

For details about the combined use with on-the-fly quantization, see [In-path Quantization and In-path ReLU Scenario Combination](on_the_fly_quantization_and_relu_combination.md).
