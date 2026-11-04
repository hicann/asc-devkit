# Principle Introduction

<!-- md-trans-meta sourceCommit=ffba65a285b93c584314c43929ffaa58f62204b1 translatedAt=2026-10-23T14:44:28.222Z pushedAt=2026-11-03T07:05:13.439Z nl=2 -->

## Overview

SuperKernel is a binary fusion technology for operators. Unlike source code fusion, it focuses on optimizing the binary scheduling scheme of kernel functions. Based on compiled binary code, it fuses multiple sub-kernel functions (Kernels) to create a super kernel function (SuperKernel for short). The SuperKernel integrates multiple kernel functions (Kernels) by calling them as sub-functions, thereby optimizing computing tasks and improving performance and resource utilization.

Compared with single-operator delivery, SuperKernel reduces the waiting time and scheduling overhead of task scheduling, and further optimizes the operator header overhead by utilizing the idle resources between tasks, thereby improving inference performance.

After SuperKernel fusion optimization is enabled, the system automatically identifies the operators that can be fused in the graph and generates a SuperKernel for computation. In addition, the framework provides the capability to calibrate the SuperKernel scope, allowing users to mark and optimize the operators within the fusion scope based on actual service requirements.

>[!NOTE] Note
>SuperKernel applies to the following models:
> <!-- npu="A3" id1 -->
> - Atlas A3 products
> <!-- end id1 -->
> <!-- npu="910b" id2 -->
> - Atlas A2 products
> <!-- end id2 -->
> <!-- npu="950" id3 -->
> - Ascend 950PR&950DT products: SuperKernel can be enabled only through the npugraph_ex backend, and the GE graph mode is not supported.
> <!-- end id3 -->

## Performance Benefits

Compared with dispatching individual operators, SuperKernel delivers performance benefits mainly in the following ways:

- **Reduced kernel function launch overhead**: Sub-kernel functions (Kernels) that originally required multiple launches are now launched through a single SuperKernel launch, reducing task scheduling overhead.
- **Reduced task scheduling wait time**: Scheduling gaps between multiple independent tasks are avoided.
- **Pipeline parallelism between operators**: Through the inter-task synchronization APIs provided in SuperKernel ([SetNextTaskStart](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) and [WaitPreTaskEnd](../../../../api/SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md)), some instructions between sub-kernel functions (Kernels) can be executed in parallel, further reducing the end-to-end latency.

## Supported Enabling Methods

Currently, the SuperKernel feature is enabled through the PyTorch graph mode in the following two ways:

| Enabling Method | Description |
| --- | --- |
| npugraph_ex backend | Pass `super_kernel_optimize=True` in the `options` of `torch.compile` to enable SuperKernel fusion. To control the fusion scope, use `torch.npu.super_kernel_scope_begin/end` for marking. For details, see [SuperKernel](https://www.hiascend.com/document/detail/en/Pytorch/2620/devguide/TorchAir/docs/en/npugraph_ex/advanced/superkernel.md). |
| GE graph mode | Mark the fusion scope using the `torchair.scope.super_kernel` scope, and enable it together with the CompilerConfig of TorchAir. For details, see [Marking the SuperKernel Scope in a Graph](https://www.hiascend.com/document/detail/en/Pytorch/2620/devguide/TorchAir/docs/en/ascend_ir/features/advanced/super_kernel_scope.md). |

## Operator Adaptation Description

Common operators of mainstream models released in the CANN package already support SuperKernel fusion. If a user-defined Ascend C operator needs to support SuperKernel, it must meet certain adaptation conditions. For details, see the following operator adaptation and self-verification instructions:

- [Operator Adaptation](operator_adaptation.md): General constraints that an operator must satisfy to be fused into SuperKernel (independent of the specific enabling method).
- [Operator Self-Verification](operator_self_verification.md): Examples of verifying SuperKernel fusion for a single operator in the two PyTorch graph modes.
- [Additional Adaptation for Kernel Function Direct-Call Operators](kernel_direct_call_adaptation.md): Additional adaptation work required before entering SuperKernel for Ascend C operators developed using the `<<<>>>` method.
