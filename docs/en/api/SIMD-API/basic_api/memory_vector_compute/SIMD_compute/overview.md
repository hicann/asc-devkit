# Overview

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T01:56:39.186Z pushedAt=2026-11-18T01:25:30.752Z nl=2 -->

Vector computation is performed by the Vector compute unit. Both the source operands and destination operands of vector computation are stored in the Unified Buffer (UB).

Memory vector computation provides two computation modes: [continuous computation](continuous_compute.md#continuous-computing) and [high-dimensional splitting](high_dim_split.md#high-dimensional-split). It also supports selective data participation in computation through [mask](mask.md#mask).

Taking the Add API as an example, the following figure shows how the two computation modes are invoked. Continuous computation is simple and intuitive, making it suitable for processing continuous data of one-dimensional tensors. High-dimensional splitting is flexible and controllable, supporting iterative execution and address stride configuration. You can choose the appropriate computation mode based on your actual business requirements.

![](../../../../figures/multi_level_vector_api.png)
