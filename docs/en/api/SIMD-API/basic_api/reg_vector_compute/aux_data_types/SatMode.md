# SatMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T21:30:06.899Z -->

| Parameter | Description |
| :-- | :-- | 
| SatMode | Used to set the saturation and non-saturation modes for Cast type conversion.<br>enum class SatMode { <br>    UNKNOWN = -1, <br>    NO_SAT, <br>    SAT <br>};<br>&bull;&nbsp;&nbsp;UNKNOWN: Select this mode when the conversion type does not support SatMode.<br>&bull;&nbsp;&nbsp;NO_SAT: Non-saturation mode.<br>&bull;&nbsp;&nbsp;SAT: Saturation mode. |

```cpp
enum class SatMode {
    UNKNOWN = -1,
    NO_SAT,
    SAT
};
```
