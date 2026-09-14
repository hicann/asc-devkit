# Operator Debugging<a name="ZH-CN_TOPIC_0000001846719824"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T12:59:46.864Z pushedAt=2026-09-11T08:45:19.645Z -->

As described in [heterogeneous compute](../heterogeneous_compute.md), an Ascend C operator mainly consists of two parts: Tiling and Kernel.

Tiling runs on the CPU on the host side and can generally be debugged using traditional debugging methods (for example, the gdb tool).

Kernel runs on the NPU on the device side. Ascend C provides multiple debugging methods, including twin debugging and on-board debugging. For details about the debugging methods, see the [operator debugging](../../programming_guide/debug_and_tuning/overview.md).

