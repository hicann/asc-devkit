# Computation Performance Optimization Based on Global Mask Reuse

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:24:51.293Z -->

> [!NOTE] Note
>
> This performance optimization suggestion applies to the following models:
>
> - <cann-filter npu-type="A3">Atlas A3 training products/Atlas A3 inference products</cann-filter>
>
> - <cann-filter npu-type="910b">Atlas A2 training products/Atlas A2 inference products</cann-filter>

[Priority] High

[Description] When **multiple vector computation operations are executed consecutively** (such as multiple Adds, Muls, and so on) and these operations share **the same mask configuration parameters** (that is, the mask length, mask value, and mask mode are consistent), there is room for performance optimization. In this case, if the internal mask setting is enabled each time a computation API is called (that is, **isSetMask=true**), a complete mask initialization process is forced to be repeated: including setting the mask mode SetMaskCount, setting the mask value (SetVectorMask), resetting the mask (ResetMask), and resetting the mask mode (SetMaskNorm). This series of operations consumes a large number of cycles at the hardware level, especially in high-frequency computation, which slows down the overall execution efficiency. When the computation scale is small, because the actual computation time is short while the fixed overhead of mask configuration accounts for a high proportion of the total time, the optimization strategy can significantly reduce the relative overhead and achieve a high proportion of performance improvement.

[Optimization Solution]

In these scenarios, when the mask configuration remains unchanged, follow the core principle of "**configure globally once, then reuse**": before all consecutive computation operations are executed, call the **SetVectorMask** API to complete the one-time, complete setting of the **global mask**; subsequently, all subsequent vector computation API calls should set the **isSetMask** parameter to **false** to explicitly instruct the API to skip the internal mask setting process and directly **reuse the preconfigured** global mask state.

**Figure 1** Code optimization  
![](../../../figures/code_optimization.png)<a id="fig1-code-optimization"></a>

Theoretical analysis: SetMaskCount, SetMask, ResetMask, and SetMaskNorm each take 1 cycle per call. When the Adds computation API is called 180 times consecutively, a total of 4 × 180 = 720 cycles are saved.
