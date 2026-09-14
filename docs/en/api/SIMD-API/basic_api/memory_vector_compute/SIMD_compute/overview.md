# Overview

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T14:54:10.978Z -->

Vector computation is performed by the Vector compute unit. Both the source operands and destination operands of vector computation are stored in the Unified Buffer (UB).

Memory vector computation provides two computation modes: [continuous computation](continuous_computation.md#continuous-computation) and [high-dimensional partitioning](high_dimension_slicing.md#high-dimensional-slicing). It also supports [mask](mask.md#mask) to enable selective participation of data in computation.

Using the Add API as an example, the following figure shows how the two computation modes are invoked. Continuous computation is simple and intuitive, and is suitable for processing continuous data of one-dimensional tensors. High-dimensional partitioning is flexible and controllable, and supports iterative execution and address interval configuration. Developers can select an appropriate computation mode based on actual service requirements.

![](../../../../figures/multi_level_vector_api.png)
