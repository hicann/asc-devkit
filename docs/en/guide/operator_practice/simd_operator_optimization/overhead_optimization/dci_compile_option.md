# Setting the DCI Compilation Option to Reduce Operator Tail Overhead<a name="ZH-CN_TOPIC_0000002531100207"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:49:21.036Z pushedAt=2026-11-03T07:05:13.344Z nl=2 -->

>[!NOTE] Note
>This performance optimization suggestion applies to the following models:
><!-- npu="950" id1 -->
>- Ascend 950PR & 950DT products
><!-- end id1 -->

[Priority] High

[Description] When an operator finishes execution, the DCache must be invalidated to prevent subsequent operators from being affected by continuing to use the data in the DCache. You can add --cce-no-dcache-flush to the compilation options to insert a DCI (DataCacheInvalid) instruction at the end of the operator to invalidate the DCache. If this option is not enabled, a DCCI (DataCacheCleanAndInvalid) instruction is inserted by default to invalidate the DCache.

Compared with inserting a DCCI instruction, inserting a DCI instruction eliminates the process of synchronizing data from the DCache to the GM (Clean), which provides a certain performance advantage. Inserting DCCI is an additional fault-tolerance guarantee. If a developer rewrites GM memory using \* \_\_gm\_\_, or calls the GlobalTensor.SetValue function without correctly calling the DataCacheCleanAndInvalid API to ensure cache consistency, the DCCI instruction automatically inserted by the compilation framework can ensure that the operator precision is correct.

Therefore, in the following scenarios, you can enable this compilation option to reduce the operator tail overhead:

-   The operator rewrites GM memory using \* \_\_gm\_\_, or calls the GlobalTensor.SetValue function and correctly uses the DataCacheCleanAndInvalid API to manually flush data from the DCache back to the GM, ensuring cache consistency. It does not rely on the DCCI instruction automatically inserted by the compilation framework to ensure consistency.
-   The operator does not contain code that rewrites GM memory using \* \_\_gm\_\_, or calls the GlobalTensor.SetValue function.
