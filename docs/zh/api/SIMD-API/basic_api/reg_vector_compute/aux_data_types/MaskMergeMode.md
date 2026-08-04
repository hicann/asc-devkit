# MaskMergeMode<a name="ZH-CN_TOPIC_0000001956862301"></a>

| 参数名 | 描述 |
| :-- | :-- | 
| MaskMergeMode | 用于指定写入寄存器数据模式。<br><pre>enum class MaskMergeMode { <br>    UNKNOWN, <br>    MERGING, <br>    ZEROING <br>};</pre>&bull;&nbsp;&nbsp;UNKNOWN：当转换类型不支持MaskMergeMode时，选择该模式；<br>&bull;&nbsp;&nbsp;ZEROING：mask未选择的元素在dst中置零；<br>&bull;&nbsp;&nbsp;MERGING：mask未选择的元素对应dst元素中保留dst原值。 |

```cpp
enum class MaskMergeMode {
    UNKNOWN,
    MERGING,
    ZEROING
};
```

以Duplicate接口为例，ZEROING和MERGING模式图示如下：

**图1** ZEROING/MERGING对比<a id="fig1"></a>

![](../../../../figures/ZEROING和MERGING对比.png "ZEROING/MERGING对比")
