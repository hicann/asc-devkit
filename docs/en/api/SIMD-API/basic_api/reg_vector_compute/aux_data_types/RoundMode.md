# RoundMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=6b81d2e6322744fc9adc8665d94bfafab36b3550 translatedAt=2026-11-05T02:17:11.396Z pushedAt=2026-11-18T01:25:30.788Z nl=2 -->

| Parameter | Description |
| :-- | :-- | 
| RoundMode | Used to set and control the rounding mode.<br>enum class RoundMode { <br>    CAST_NONE = 0, <br>    CAST_RINT, <br>    CAST_FLOOR,<br>    CAST_CEIL,<br>    CAST_ROUND,<br>    CAST_TRUNC,<br>    CAST_ODD,<br>    CAST_HYBRID <br>};<br>&bull;&nbsp;&nbsp;**CAST_NONE**: When precision loss occurs during conversion, it is handled in **CAST_RINT** mode; when no precision loss occurs, no rounding is performed.<br>&bull;&nbsp;&nbsp;**CAST_RINT**: Round to the nearest even.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first bit of the part to be rounded is 0, no carry.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first bit of the part to be rounded is 1 and the subsequent bits are not all 0, carry.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the first bit of the part to be rounded is 1 and the subsequent bits are all 0: when the last bit of the non-rounded part is 0, no carry. When the last bit of the non-rounded part is 1, carry.<br>&bull;&nbsp;&nbsp;**CAST_FLOOR**: Round toward negative infinity.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 0 (positive), no carry.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 1 (negative): when the part to be rounded is all 0, no carry. Otherwise, carry.<br>&bull;&nbsp;&nbsp;**CAST_CEIL**: Round toward positive infinity.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 1 (negative), no carry.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the sign bit (S) is 0 (positive): when the part to be rounded is all 0, no carry. Otherwise, carry.<br>&bull;&nbsp;&nbsp;**CAST_ROUND**: Round half up. If the first bit of the part to be rounded is 0, no carry. Otherwise, carry.<br>&bull;&nbsp;&nbsp;**CAST_TRUNC**: Directly discard the part to be rounded.<br>&bull;&nbsp;&nbsp;**CAST_ODD**: Round to the nearest odd.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the part to be rounded is all 0, no carry.<br>&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;&nbsp;If the part to be rounded is not all 0: when the last bit of the non-rounded part is 1, no carry. When the last bit of the non-rounded part is 0, carry.<br>&bull;&nbsp;&nbsp;**CAST_HYBRID**: Random rounding, currently referring specifically to random rounding when the output result is of the **hifloat8_t** data type. |

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
