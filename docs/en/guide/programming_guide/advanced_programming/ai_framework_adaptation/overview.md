# Overview<a name="ZH-CN_TOPIC_0000001983023284"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T14:10:05.902Z pushedAt=2026-11-03T07:05:13.417Z nl=2 -->

This section describes how AI frameworks invoke custom operators. As shown in the following figure, PyTorch supports both operator-by-operator and graph modes, while TensorFlow supports only graph mode.

When an AI framework invokes custom operators, in addition to the code implementation files required for CANN framework invocation, plugin adaptation development is also required.

![](../../../figures/kernelLaunch.png)
