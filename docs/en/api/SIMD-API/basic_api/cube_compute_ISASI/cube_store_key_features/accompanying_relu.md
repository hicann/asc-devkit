# In-line ReLU<a name="ZH-CN_TOPIC_0000002538231206"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T04:21:54.923Z pushedAt=2026-11-18T01:25:06.781Z nl=2 -->

## Description

The matrix computation move-out process supports the on-the-fly ReLU capability. The following on-the-fly ReLU capabilities are currently supported: In the **DataCopyCO12DstParams** structure parameter definition, **reluPre** can be set to 1 to enable Normal ReLU, and combined with **clipReluPre**, the ClipReLU capability can be further enabled.

- When the parameter is set to 0, the on-the-fly ReLU capability is disabled.

- When the parameter is set to 1, Normal ReLU is enabled. No additional register configuration is required, and the activation operation `y = max(0, x)` is performed on the output data.

- When Normal ReLU is enabled (**reluPre**=1) and **clipReluPre**=1 is also configured with the SetFixPipeClipRelu API called, Normal ReLU + ClipReLU is enabled. In this case, `y = max(0, x)` is performed first, followed by `y = min(clipReluMaxVal, y)`, where clipReluMaxVal is set through the SetFixPipeClipRelu API. For details, see [SetFixPipeClipRelu](../cube_store_aux_config/SetFixPipeClipRelu.md).

For details about combined use with on-the-fly quantization, see [On-the-Fly Quantization and On-the-Fly ReLU Scenario Combination](accompanying_quantization_and_relu_scenario_combination.md).
