# MaskMergeMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T21:28:31.698Z -->

| Parameter | Description |
| :-- | :-- | 
| MaskMergeMode | Specifies the mode for writing data to registers.<br>enum class MaskMergeMode { <br>    UNKNOWN, <br>    MERGING, <br>    ZEROING <br>};<br>&bull;&nbsp;&nbsp;**UNKNOWN**: Select this mode when the conversion type does not support **MaskMergeMode**.<br>&bull;&nbsp;&nbsp;**ZEROING**: Elements not selected by the mask are set to zero in **dst**.<br>&bull;&nbsp;&nbsp;**MERGING**: Elements not selected by the mask retain their original values in **dst**. |

```cpp
enum class MaskMergeMode {
    UNKNOWN,
    MERGING,
    ZEROING
};
```

Using the Duplicate API as an example, the ZEROING and MERGING modes are illustrated as follows:

**Figure 1** ZEROING/MERGING comparison<a id="fig1"></a>

![](../../../../figures/zeroing_vs_merging.png "ZEROING/MERGING comparison")
