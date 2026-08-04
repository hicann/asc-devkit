# PostLiteral<a name="ZH-CN_TOPIC_0000001956862301"></a>

| 参数名 | 描述 |
| :-- | :-- | 
| PostLiteral | 用于控制是否开启UB地址自动更新功能（post update）。<br><pre>enum class PostLiteral { <br>    POST_MODE_NORMAL, <br>    POST_MODE_UPDATE <br>};</pre>&bull;&nbsp;&nbsp;POST_MODE_NORMAL：POST_MODE_NORMAL场景使用，UB操作数地址不更新。连续非对齐搬入（LoadUnAlign）不支持POST_MODE_NORMAL模式；<br>&bull;&nbsp;&nbsp;POST_MODE_UPDATE：POST_MODE_UPDATE场景使用，UB地址同时作为输入和输出，每次调用会更新。 |

```cpp
enum class PostLiteral {
    POST_MODE_NORMAL,
    POST_MODE_UPDATE
};
```
