# Atomic Operation Overview<a name="ZH-CN_TOPIC_0000002555621180"></a>

<!-- md-trans-meta sourceCommit=8eaad1685e91635426612725c1a6e7fed1a0f05a translatedAt=2026-11-02T02:09:03.478Z pushedAt=2026-11-18T01:25:06.567Z nl=2 -->

The inline atomic operation APIs for data movement are used to enable atomic operations for subsequent data movement whose destination address is GM. For the involved APIs, see [Table 1](#table1). As shown in the left sub-figure of Figure 1, when atomic operations are not enabled, after the data movement that writes to GM is complete, the original data in GM is completely overwritten by the newly moved data. As shown in the right sub-figure of Figure 1, after the inline atomic operation API for data movement is called, the system enables atomic operations for subsequent data movement that writes to GM. In this case, after the data movement is complete, the final data in GM is determined jointly by the original GM data and the newly moved data.

**Figure 1**  Inline atomic accumulation effect of data movement  
![](../../../figures/atomic_operation_pipelined_data_movement_effect.png "Inline atomic accumulation effect of data movement")

**Table 1**  Inline atomic operation APIs for data movement<a name="table1"></a>

| Corresponding API | API Description |
| --- | --- |
| [SetAtomicAdd](SetAtomicAdd.md) | Enables atomic accumulation for subsequent data movement whose destination address is GM. Atomic accumulation process: sums the content to be copied and the existing content in GM, and then writes the sum result to GM. |
| [SetAtomicType](SetAtomicType.md) | Sets different data types for atomic operations by configuring template parameters. |
| [DisableDmaAtomic](DisableDmaAtomic.md) | Disables the inline atomic operation function for data movement. |
| [SetAtomicMax(ISASI)](SetAtomicMax_ISASI.md) | Sets whether the data subsequently moved to GM performs atomic comparison: compares the content to be copied with the existing content in GM, and then writes the maximum value to GM. |
| [SetAtomicMin(ISASI)](SetAtomicMin_ISASI.md) | Sets whether the data subsequently moved to GM performs atomic comparison: compares the content to be copied with the existing content in GM, and then writes the minimum value to GM. |

The Scalar atomic operation API performs single-point atomic computation on a specified GM address. For the APIs involved, see [Table 2](#table2). Unlike the data movement inline atomic operation API, the Scalar atomic operation API does not affect subsequent instructions that move data to GM.

As shown in the left sub-figure of Figure 2, when the **AtomicAdd** API is not used, multiple AI Cores performing accumulation operations on the same GM address simultaneously overwrite each other. The operations are not atomic, and the final result is unpredictable. As shown in the right sub-figure, after the **AtomicAdd** API is used, the accumulation operations of each AI Core are executed serially, ensuring the atomicity of each accumulation operation. The final result meets expectations.

**Figure 2**  Scalar atomic accumulation effect  
![](../../../figures/scalar_atomic_operation_diagram.png "Scalar atomic accumulation effect")

**Table 2**  Scalar atomic operation APIs<a name="table2"></a>

| Corresponding API | API Description |
| --- | --- |
| [AtomicAdd](AtomicAdd.md) | Performs an atomic addition operation on a specified GM address. It sums the old value (`old_value`) at the GM address pointed to by `address` and the input scalar value (`value`), writes the sum result (`new_value`) back to the GM address, and returns the value before modification (`old_value`) at that address. |
| [AtomicMin](AtomicMin.md) | Performs an atomic minimum operation on a specified GM address. It compares the old value (`old_value`) at the GM address pointed to by `address` with the input scalar value (`value`), writes the smaller value (`new_value`) back to the GM address, and returns the value before modification (`old_value`) at that address. |
| [AtomicMax](AtomicMax.md) | This API performs an atomic maximum operation on a specified GM address. It compares the old value (`old_value`) at the GM address pointed to by `address` with the input scalar value (`value`), writes the larger value (`new_value`) back to the GM address, and returns the value before modification (`old_value`). |
| [AtomicCas](AtomicCas.md) | Performs an atomic comparison operation on a specified GM address. It reads the old value (`old_value`) at the GM address pointed to by `address` and compares it with the input scalar value `value1`: if they are equal, the input scalar value `value2` is written to the GM address; if they are not equal, the value at the GM address remains unchanged. Returns the value before modification (`old_value`) at that address. |
| [AtomicExch](AtomicExch.md) | Performs an atomic exchange operation in GM memory. It reads the old value (`old_value`) at the GM address pointed to by `address`, replaces the old value with the input scalar value (`value`) and stores it back to the same address, and returns the value before modification (`old_value`) at that address. |
| [SetStoreAtomicConfig(ISASI)](SetStoreAtomicConfig_ISASI.md) | Sets the scalar atomic operation configuration: operation type (only the sum operation is supported) and data type. |
| [GetStoreAtomicConfig(ISASI)](GetStoreAtomicConfig_ISASI.md) | Obtains the scalar atomic operation configuration set by SetStoreAtomicConfig. |
