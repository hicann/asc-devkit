# Operator Debugging<a name="ZH-CN_TOPIC_0000001846719824"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:46:21.806Z pushedAt=2026-11-03T07:05:13.282Z nl=2 -->

From the [heterogeneous computing](../heterogeneous_computing.md) section, you can learn that an Ascend C operator mainly consists of two parts: Tiling and Kernel implementation.

Tiling runs on the CPU on the host side and can generally be debugged using traditional debugging methods (for example, the **gdb** tool).

The kernel function implementation runs on the device-side NPU. Ascend C provides multiple debugging methods, including twin debugging and on-board debugging. For details about the debugging methods, see [Operator Debugging](../../programming_guide/debug_and_tuning/overview.md).
