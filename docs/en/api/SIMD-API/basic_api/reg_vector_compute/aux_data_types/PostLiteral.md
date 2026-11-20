# PostLiteral<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-05T02:16:09.916Z pushedAt=2026-11-18T01:25:30.787Z nl=2 -->

| Parameter | Description |
| :-- | :-- | 
| PostLiteral | Controls whether to enable the Unified Buffer (UB) address auto-update feature (post update).<br>enum class PostLiteral { <br>    POST_MODE_NORMAL, <br>    POST_MODE_UPDATE <br>};<br>&bull;&nbsp;&nbsp;POST_MODE_NORMAL: Used in the POST_MODE_NORMAL scenario, where the UB operand address is not updated. Consecutive unaligned load (LoadUnAlign) does not support the POST_MODE_NORMAL mode.<br>&bull;&nbsp;&nbsp;POST_MODE_UPDATE: Used in the POST_MODE_UPDATE scenario, where the UB address serves as both input and output and is updated on each call. |

```cpp
enum class PostLiteral {
    POST_MODE_NORMAL,
    POST_MODE_UPDATE
};
```
