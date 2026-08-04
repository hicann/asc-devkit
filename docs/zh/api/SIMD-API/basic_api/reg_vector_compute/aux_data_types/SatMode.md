# SatMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

| 参数名 | 描述 |
| :-- | :-- | 
| SatMode | 用于设置Cast类型转换饱和与不饱和模式。<br><pre>enum class SatMode { <br>    UNKNOWN = -1, <br>    NO_SAT, <br>    SAT <br>};</pre>&bull;&nbsp;&nbsp;UNKNOWN：当转换类型不支持SatMode时，选择该模式；<br>&bull;&nbsp;&nbsp;NO_SAT：不饱和模式；<br>&bull;&nbsp;&nbsp;SAT：饱和模式。 |

```cpp
enum class SatMode {
    UNKNOWN = -1,
    NO_SAT,
    SAT
};
```
