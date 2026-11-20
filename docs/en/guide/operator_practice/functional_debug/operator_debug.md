# Operator Debugging<a name="ZH-CN_TOPIC_0000001846719824"></a>

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T10:27:44.794Z pushedAt=2026-11-19T06:32:19.722Z nl=2 -->

From the [heterogeneous computing](../heterogeneous_computing.md) section, you can learn that an Ascend C operator mainly consists of two parts: tiling and kernel implementation.

Tiling runs on the CPU on the host side and can generally be debugged using traditional debugging methods (for example, the **gdb** tool).

The kernel function implementation runs on the device-side NPU. Ascend C provides multiple debugging methods, including twin debugging and on-board debugging. For details about the debugging methods, see [Operator Debugging](../../programming_guide/debug_and_tuning/overview.md).
