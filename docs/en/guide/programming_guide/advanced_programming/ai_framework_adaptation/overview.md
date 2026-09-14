# Overview<a name="ZH-CN_TOPIC_0000001983023284"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T12:07:07.895Z pushedAt=2026-09-03T07:45:44.490Z -->

This section describes how AI frameworks call custom operators. As shown in the following figure, PyTorch supports both the single-operator mode and the graph mode, while TensorFlow supports only the graph mode.

When an AI framework calls a custom operator, in addition to the code implementation files required for a CANN framework call, plugin adaptation development is also required.

![](../../../figures/kernelLaunch.png)
