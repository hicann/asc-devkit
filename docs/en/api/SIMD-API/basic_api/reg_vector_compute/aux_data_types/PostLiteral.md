# PostLiteral<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T21:28:41.190Z -->

| Parameter | Description |
| :-- | :-- | 
| PostLiteral | Controls whether to enable the UB address auto-update feature (post update).<br>enum class PostLiteral { <br>    POST_MODE_NORMAL, <br>    POST_MODE_UPDATE <br>};<br>&bull;&nbsp;&nbsp;**POST_MODE_NORMAL**: Used in the POST_MODE_NORMAL scenario, where the UB operand address is not updated. Consecutive unaligned load-in (LoadUnAlign) does not support the POST_MODE_NORMAL mode.<br>&bull;&nbsp;&nbsp;**POST_MODE_UPDATE**: Used in the POST_MODE_UPDATE scenario, where the UB address serves as both input and output and is updated on each call. |

```cpp
enum class PostLiteral {
    POST_MODE_NORMAL,
    POST_MODE_UPDATE
};
```
