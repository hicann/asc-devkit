# Setting DCI Compilation Options to Reduce Operator Tail Overhead<a name="ZH-CN_TOPIC_0000002531100207"></a>

<!-- md-trans-meta sourceCommit=636f19d186bc97094509fa434fab0ea55d11c790 translatedAt=2026-08-26T14:34:47.831Z -->

>[!NOTE] Note
>This performance optimization suggestion applies to the following models:
>- Ascend 950PR/Ascend 950DT

[Priority] High

[Description] When an operator finishes execution, the DCache must be invalidated to prevent subsequent operators from being affected by continuing to use the data in the DCache. You can add --cce-no-dcache-flush to the compilation options to insert a DCI (DataCacheInvalid) instruction at the end of the operator to invalidate the DCache. If this option is not enabled, a DCCI (DataCacheCleanAndInvalid) instruction is inserted by default to invalidate the DCache.

Compared with inserting a DCCI instruction, inserting a DCI instruction eliminates the process of synchronizing data from the DCache to the GM (Clean), providing a certain performance advantage. Inserting DCCI is an additional fault-tolerance guarantee. If developers use \* \_\_gm\_\_ to modify GM memory, or call the GlobalTensor.SetValue function without correctly calling the DataCacheCleanAndInvalid API to ensure cache consistency, the DCCI automatically inserted by the compilation framework can exactly ensure normal operator precision.

Therefore, in the following scenarios, you can enable this compilation option to reduce operator tail overhead:

-   The operator uses \* \_\_gm\_\_ to modify GM memory, or calls the GlobalTensor.SetValue function while correctly using the DataCacheCleanAndInvalid API to manually flush data from the DCache back to the GM to ensure cache consistency. It does not rely on the DCCI instruction automatically inserted by the compilation framework to ensure consistency.
-   The operator does not contain code that uses \* \_\_gm\_\_ to modify GM memory or calls the GlobalTensor.SetValue function.

