# TILING\_KEY\_LIST<a name="ZH-CN_TOPIC_0000002516636745"></a>

<!-- md-trans-meta sourceCommit=9f6ce11d8b0e7d296d9c9a7af96f6025bf5b48d1 translatedAt=2026-08-27T18:17:21.564Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001610027821_section212607105720"></a>

The **TILING_KEY_LIST** function is used in a kernel function to determine whether the currently executed TilingKey matches the specified TilingKey configured on the Host side, thereby identifying the branch logic that satisfies the condition TilingKey == key1 or TilingKey == key2.

## Prototype<a name="zh-cn_topic_0000001610027821_section1630753514297"></a>

```
TILING_KEY_LIST(key1,key2)
```

## Parameters<a name="zh-cn_topic_0000001610027821_section129451113125413"></a>

<a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001610027821_zh-cn_topic_0000001389783361_p2340183613156"></a>key</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001610027821_p18857658124919"><a name="zh-cn_topic_0000001610027821_p18857658124919"></a><a name="zh-cn_topic_0000001610027821_p18857658124919"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001610027821_p53562135013"><a name="zh-cn_topic_0000001610027821_p53562135013"></a><a name="zh-cn_topic_0000001610027821_p53562135013"></a>key indicates the branch of a kernel function and must be a non-negative integer.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001610027821_section65498832"></a>

-   **TILING_KEY_LIST** is used in **if** and **else if** branches, and is not supported in the **else** branch. That is, when the **TILING_KEY_LIST** function is used to represent N branches, each branch must be represented separately by N **TILING_KEY_LIST(key1, key2)** calls.
-   Two **TilingKey** values are supported, and each **TilingKey** must be unique.
-   When using this API, you must set a default **Kernel** type. You can also configure a **Kernel** type for a specific **TilingKey**, and this configuration overrides the default **Kernel** type. The **Kernel** type can only be configured as **KERNEL_TYPE_MIX_AIC_1_1** or **KERNEL_TYPE_MIX_AIC_1_2**.
-   Kernel direct invocation projects are not supported.

## Example<a name="zh-cn_topic_0000001610027821_section97001499599"></a>

```
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (workspace == nullptr) {
        return;
    }
    KernelAdd op;
    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_1);
    op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
    // When TilingKey is 1 or 2, execute Process1; when it is 3 or 4, execute Process2.
    if (TILING_KEY_LIST(1,2)) {
        op.Process1();
    } else if (TILING_KEY_LIST(3,4)) {
        KERNEL_TASK_TYPE(4, KERNEL_TYPE_MIX_AIC_1_2);
        op.Process2();
    } 
    // Other code logic.
    ...
    // This example shows that when TilingKey is 3 or 4, ProcessOther is executed.
    if (TILING_KEY_LIST(3,4)) {
        op.ProcessOther();
    }
}
```

The corresponding Host-side Tiling function example (pseudocode):

```
ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Other code logic.
    ...
    if (context->GetInputShape(0) > 10) {
        context->SetTilingKey(1);
    } else if (some condition) {
        context->SetTilingKey(2);
    } else if (some condition) {
        context->SetTilingKey(3);
    } else if (some condition) {
        context->SetTilingKey(4);
    }
}
```

