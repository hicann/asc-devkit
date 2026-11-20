# Overview<a name="ZH-CN_TOPIC_0000001983023284"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:50:43.656Z pushedAt=2026-11-19T06:32:19.885Z nl=2 -->

This section describes how AI frameworks call custom operators. As shown in the following figure, PyTorch supports both the single-operator mode and the graph mode, while TensorFlow supports only the graph mode.

When an AI framework calls custom operators, in addition to the code implementation files required for the CANN framework call, plugin adaptation development is also required.

<img src="../../../figures/kernelLaunch.png" alt="" width="600px" height="478px">
