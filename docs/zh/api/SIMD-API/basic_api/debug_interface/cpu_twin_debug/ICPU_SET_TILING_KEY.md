# ICPU\_SET\_TILING\_KEY<a name="ZH-CN_TOPIC_0000002044842850"></a>

## 功能说明<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section259105813316"></a>

头文件路径为：`"tools/cpudebug/include/kern_fwk.h"`。

用于指定本次CPU调测使用的tilingKey。调测执行时，将只执行算子核函数中该tilingKey对应的分支。

## 函数原型<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section2067518173415"></a>

```cpp
#define ICPU_SET_TILING_KEY(tilingKey)
```

## 参数说明<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section158061867342"></a>

<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_zh-cn_topic_0235751031_table33761356"></a>
**表1**  模板参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| tilingKey | 输入 | 指定本次CPU调测使用的tilingKey。参数类型为uint64_t。|

## 返回值说明<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section794123819592"></a>

- 为了保留接口兼容，推荐[<<<>>>](../../../../../guide/编程指南/调试调优/功能调试/CPU域孪生调试.md)编译使用。
- tilingKey建议传入正整数，如果设置为负数或者0，将会告警并继续调测。如果传入0，将会执行tilingKey为0的分支；tilingKey传入负数，将导致未定义的行为。
- 该接口需要在[ICPU_RUN_KF](ICPU_RUN_KF.md)前调用。

## 调用示例<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section82241477610"></a>

```cpp
ICPU_SET_TILING_KEY(10086);
ICPU_RUN_KF(sort_kernel0, coreNum, (uint8_t*)x, (uint8_t*)y);
```
