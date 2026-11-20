# ICPU\_SET\_TILING\_KEY<a name="ZH-CN_TOPIC_0000002044842850"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T05:50:29.386Z pushedAt=2026-11-18T01:25:06.949Z nl=2 -->

## Description<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section259105813316"></a>

The header file path is `"tools/cpudebug/include/kern_fwk.h"`.

Used to specify the tilingKey for this CPU debugging session. During debugging execution, only the branch corresponding to this tilingKey in the operator kernel function (Kernel) is executed.

## Prototype<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section2067518173415"></a>

```cpp
#define ICPU_SET_TILING_KEY(tilingKey)
```

## Parameters<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section158061867342"></a>

<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_zh-cn_topic_0235751031_table33761356"></a>
**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| tilingKey | Input | Specifies the tilingKey used for this CPU debugging. The parameter type is uint64_t.|

## Return Value<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section794123819592"></a>

- To preserve API compatibility, it is recommended to compile with [<<<>>>](../../../../../guide/programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md).
- It is recommended to pass a positive integer for **tilingKey**. If it is set to a negative number or 0, a warning is issued and debugging continues. If 0 is passed, the branch with **tilingKey** 0 is executed; passing a negative number for **tilingKey** results in undefined behavior.
- This API must be called before [ICPU_RUN_KF](ICPU_RUN_KF.md).

## Example<a name="zh-cn_topic_0000002000279997_zh-cn_topic_0000001610028277_section82241477610"></a>

```cpp
ICPU_SET_TILING_KEY(10086);
ICPU_RUN_KF(sort_kernel0, coreNum, (uint8_t*)x, (uint8_t*)y);
```
