# RoundMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T21:30:18.243Z -->

| Parameter | Description |
| :-- | :-- | 
| RoundMode | Used to set the rounding mode.<br>enum class RoundMode { <br>    CAST_NONE = 0, <br>    CAST_RINT, <br>    CAST_FLOOR,<br>    CAST_CEIL,<br>    CAST_ROUND,<br>    CAST_TRUNC,<br>    CAST_ODD,<br>    CAST_HYBRID <br>};<br>&bull;&nbsp;&nbsp;CAST_NONE: When precision loss occurs during conversion, it is processed in CAST_RINT mode; when no precision loss occurs, no rounding is performed.<br>&bull;&nbsp;&nbsp;CAST_RINT: Round to the nearest even.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first part of the value to be rounded is 0, no carry is performed.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first part of the value to be rounded is 1 and the subsequent bits are not all 0, carry is performed.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first part of the value to be rounded is 1 and the subsequent bits are all 0: when the last part of the non-rounding part is 0, no carry is performed. When the last part of the non-rounding part is 1, carry is performed.<br>&bull;&nbsp;&nbsp;CAST_FLOOR: Round toward negative infinity.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 0 (positive), no carry is performed.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 1 (negative): when all parts of the value to be rounded are 0, no carry is performed. Otherwise, carry is performed.<br>&bull;&nbsp;&nbsp;CAST_CEIL: Round toward positive infinity.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 1 (negative), no carry is performed.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 0 (positive): when all parts of the value to be rounded are 0, no carry is performed. Otherwise, carry is performed.<br>&bull;&nbsp;&nbsp;CAST_ROUND: Round half up. If the first part of the value to be rounded is 0, no carry is performed. Otherwise, carry is performed.<br>&bull;&nbsp;&nbsp;CAST_TRUNC: Directly discard the value to be rounded.<br>&bull;&nbsp;&nbsp;CAST_ODD: Round to the nearest odd.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If all parts of the value to be rounded are 0, no carry is performed.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the value to be rounded is not all 0: when the last part of the non-rounding part is 1, no carry is performed. When the last part of the non-rounding part is 0, carry is performed.<br>&bull;&nbsp;&nbsp;CAST_HYBRID: Random rounding, currently referring specifically to random rounding when the output result is of the hifloat8_t data type. |

```cpp
enum class RoundMode {
    CAST_NONE = 0,
    CAST_RINT,
    CAST_FLOOR,
    CAST_CEIL,
    CAST_ROUND,
    CAST_TRUNC,
    CAST_ODD,
    CAST_HYBRID
};
```
