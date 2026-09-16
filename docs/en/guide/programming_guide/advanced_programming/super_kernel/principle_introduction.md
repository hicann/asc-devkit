# Principle Introduction

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-08-26T12:14:29.488Z pushedAt=2026-09-03T11:12:55.777Z -->

## Overview

SuperKernel is a binary fusion technology for operators. Unlike source code fusion, it focuses on optimizing the binary scheduling scheme of kernel functions. Based on compiled binary code, it fuses multiple sub-kernels to create a super kernel function (SuperKernel for short). The SuperKernel integrates multiple kernel functions by calling them as sub-functions, thereby optimizing computing tasks and improving performance and resource utilization.

Compared with single-operator delivery, SuperKernel reduces the waiting time and scheduling overhead of task scheduling, and further optimizes the operator header overhead by utilizing the idle resources between tasks, thereby improving inference performance.

After SuperKernel fusion optimization is enabled, the system automatically identifies the operators that can be fused in the graph and generates a SuperKernel for computation. In addition, the framework provides the capability to calibrate the SuperKernel scope, allowing users to mark and optimize the operators within the fusion scope based on actual service requirements.

>[!NOTE] Note
>SuperKernel applies to the following models:
> - Atlas A3 training products/Atlas A3 inference products
> - Atlas A2 training products/Atlas A2 inference products
> - Ascend 950PR/Ascend 950DT. On these models, the SuperKernel feature is an experimental feature that will be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers should pay attention to later version updates when using this feature.

## Performance Benefits

Compared with dispatching individual operators, SuperKernel delivers performance benefits mainly in the following ways:

- **Reduced kernel launch overhead**: Sub-kernels that originally required multiple launches are now launched through a single SuperKernel launch, reducing task scheduling overhead.
- **Reduced task scheduling wait time**: Scheduling gaps between multiple independent tasks are avoided.
- **Pipelined parallelism between operators**: Through the inter-task synchronization APIs provided in SuperKernel ([SetNextTaskStart](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) and [WaitPreTaskEnd](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md)), some instructions between sub-kernels can be executed in parallel, further reducing the end-to-end latency.

## Supported Enabling Methods

Currently, the SuperKernel feature is enabled through the PyTorch graph mode in the following two ways:

| Enabling Method | Description |
| --- | --- |
| npugraph_ex backend | Pass `super_kernel_optimize=True` in the `options` of `torch.compile` to enable SuperKernel fusion. To control the fusion scope, use `torch.npu.super_kernel_scope_begin/end` for marking. For details, see [SuperKernel](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/npugraph_ex/advanced/superkernel.md). |
| GE graph mode | Mark the fusion scope with the `torchair.scope.super_kernel` scope and enable it together with the CompilerConfig of TorchAir. For details, see the section about [marking the SuperKernel scope in a graph](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/ascend_ir/features/advanced/super_kernel_scope.md). |

## Operator Adaptation Description

Common operators of mainstream models released in the CANN package already support SuperKernel fusion. If a user-defined Ascend C operator needs to support SuperKernel, it must meet certain adaptation conditions. For details, see the following operator adaptation and self-verification instructions:

- [Operator adaptation description](operator_adaptation_description.md): general constraints that an operator must meet to be fused into SuperKernel (independent of the specific enabling method).
- [Operator self-verification description](operator_self_verification_description.md): examples of verifying SuperKernel fusion for a single operator in two PyTorch graph modes.
- [Additional adaptation description for kernel launch operators](additional_adaptation_description_for_kernel_launch_operator.md): additional adaptation work required before an Ascend C operator developed using the `<<<>>>` method enters SuperKernel.
