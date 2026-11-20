# MaskMergeMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=6b81d2e6322744fc9adc8665d94bfafab36b3550 translatedAt=2026-11-05T02:15:54.385Z pushedAt=2026-11-18T01:25:30.785Z nl=2 -->

| Parameter | Description |
| :-- | :-- | 
| MaskMergeMode | Specifies the mode for writing data to registers.<br>enum class MaskMergeMode { <br>    UNKNOWN, <br>    MERGING, <br>    ZEROING <br>};<br>&bull;&nbsp;&nbsp;UNKNOWN: Select this mode when the conversion type does not support MaskMergeMode.<br>&bull;&nbsp;&nbsp;ZEROING: Elements not selected by the mask are set to zero in dst.<br>&bull;&nbsp;&nbsp;MERGING: Elements not selected by the mask retain their original values in dst. |

```cpp
enum class MaskMergeMode {
    UNKNOWN,
    MERGING,
    ZEROING
};
```

Taking the Duplicate API as an example, the ZEROING and MERGING modes are illustrated as follows:

**Figure 1** ZEROING/MERGING comparison<a id="fig1"></a>

![](../../../../figures/zeroing_merging_comparison.png "ZEROING/MERGING comparison")
