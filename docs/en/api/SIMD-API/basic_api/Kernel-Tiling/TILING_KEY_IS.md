# TILING\_KEY\_IS<a name="ZH-CN_TOPIC_0000001586995454"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:29:56.566Z pushedAt=2026-10-26T07:41:44.419Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/Kernel-Tiling/TILING_KEY_IS_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001610027821_section212607105720"></a>

Determines in the **Kernel** whether the tiling\_key of the current execution equals a **key** set on the host side at runtime, thereby identifying a **kernel** branch of tiling\_key==key.

## Function Prototype<a name="zh-cn_topic_0000001610027821_section1630753514297"></a>

```
TILING_KEY_IS(key)
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
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001610027821_p53562135013"><a name="zh-cn_topic_0000001610027821_p53562135013"></a><a name="zh-cn_topic_0000001610027821_p53562135013"></a>**key** indicates the branch of a kernel (Kernel) and must be a non-negative integer.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001610027821_section65498832"></a>

-   TILING_KEY_IS applies to if and else if branches, and does not support the else branch. That is, when TILING_KEY_IS functions are used to represent N branches, N TILING_KEY_IS\(key\) must be used to represent each branch.
-   Kernel launch projects are not supported yet.

## Example<a name="zh-cn_topic_0000001610027821_section97001499599"></a>

```
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    if (workspace == nullptr) {
        return;
    }
    KernelAdd op;
    op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
    // When TilingKey is 1, execute Process1; when it is 2, execute Process2; when it is 3, execute Process3.
    if (TILING_KEY_IS(1)) {
        op.Process1();
    } else if (TILING_KEY_IS(2)) {
        op.Process2();
    } else if (TILING_KEY_IS(3)) {
        op.Process3();
    }
    // Other code logic.
    ...
    // This example shows that when TilingKey is 3, ProcessOther is executed.
    if (TILING_KEY_IS(3)) {
        op.ProcessOther();
    }
}
```

The matching host-side tiling function example (pseudocode):

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
    }
}
```
