# ShapeInfo<a name="ZH-CN_TOPIC_0000001848102730"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:59:51.795Z -->

## Description<a name="zh-cn_topic_0000001441184464_section618mcpsimp"></a>

**ShapeInfo** is used to store the shape information of a **LocalTensor** or **GlobalTensor**.

## Prototype<a name="zh-cn_topic_0000001441184464_section620mcpsimp"></a>

-   <a name="li1555613824219"></a>**ShapeInfo** structure definition

    ```cpp
    struct ShapeInfo {
    public:
        __aicore__ inline ShapeInfo();
        __aicore__ inline ShapeInfo(const uint8_t inputShapeDim, const uint32_t inputShape[],
            const uint8_t inputOriginalShapeDim, const uint32_t inputOriginalShape[], const DataFormat inputFormat);
        __aicore__ inline ShapeInfo(const uint8_t inputShapeDim, const uint32_t inputShape[], const DataFormat inputFormat);
        __aicore__ inline ShapeInfo(const uint8_t inputShapeDim, const uint32_t inputShape[]);
        uint8_t shapeDim;
        uint8_t originalShapeDim;
        uint32_t shape[K_MAX_DIM];
        uint32_t originalShape[K_MAX_DIM];
        DataFormat dataFormat;
    };
    ```

-   Obtains the cumulative product of all dims in the shape.

    ```cpp
    __aicore__ inline int GetShapeSize(const ShapeInfo& shapeInfo)
    ```

## Function Description<a name="zh-cn_topic_0000001441184464_section622mcpsimp"></a>

**Table 1**  ShapeInfo structure parameters

<a name="zh-cn_topic_0000001441184464_table18149577913"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001441184464_row61411571196"><th class="cellrowborder" valign="top" width="19.59%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001441184464_p2093713281104"><a name="zh-cn_topic_0000001441184464_p2093713281104"></a><a name="zh-cn_topic_0000001441184464_p2093713281104"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="80.41%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001441184464_p393813285106"><a name="zh-cn_topic_0000001441184464_p393813285106"></a><a name="zh-cn_topic_0000001441184464_p393813285106"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001441184464_row8906103284616"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p71771922134011"><a name="p71771922134011"></a><a name="p71771922134011"></a>shapeDim</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001441184464_p14953134584410"><a name="zh-cn_topic_0000001441184464_p14953134584410"></a><a name="zh-cn_topic_0000001441184464_p14953134584410"></a>Existing shape dimension.</p></td>
</tr>
<tr id="row2723145173511"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p137231651173516"><a name="p137231651173516"></a><a name="p137231651173516"></a>shape</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="p7723195115357"><a name="p7723195115357"></a><a name="p7723195115357"></a>Existing shape.</p></td>
</tr>
<tr id="row3506131575511"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p115061915185516"><a name="p115061915185516"></a><a name="p115061915185516"></a>originalShapeDim</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="p95071815115516"><a name="p95071815115516"></a><a name="p95071815115516"></a>Original shape dimension.</p></td>
</tr>
<tr id="row6541252145511"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p7541952195517"><a name="p7541952195517"></a><a name="p7541952195517"></a>**originalShape**</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="p95414526553"><a name="p95414526553"></a><a name="p95414526553"></a>Original shape.</p></td>
</tr>
<tr id="row23258805616"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p732512811563"><a name="p732512811563"></a><a name="p732512811563"></a>**dataFormat**</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="p991274611916"><a name="p991274611916"></a><a name="p991274611916"></a>Data layout format, of the **DataFormat** type, defined as follows:</p>
<a name="screen12216174110564"></a><a name="screen12216174110564"></a><pre class="screen" codetype="Cpp" id="screen12216174110564">enum class DataFormat : uint8_t {
    ND = 0,
    NZ,
    NCHW,
    NC1HWC0,
    NHWC,
};</pre></td>
</tr>
</tbody>
</table>

**Table 2** **GetShapeSize** parameter description

<a name="table153364918102"></a>
<table><thead align="left"><tr id="row7363209171013"><th class="cellrowborder" valign="top" width="19.81%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.21%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.97999999999999%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row2036317913108"><td class="cellrowborder" valign="top" width="19.81%" headers="mcps1.2.4.1.1 "><p id="p57906662716"><a name="p57906662716"></a><a name="p57906662716"></a>**shapeInfo**</p></td>
<td class="cellrowborder" valign="top" width="12.21%" headers="mcps1.2.4.1.2 "><p id="p126771387103"><a name="p126771387103"></a><a name="p126771387103"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.97999999999999%" headers="mcps1.2.4.1.3 "><p id="p736312951011"><a name="p736312951011"></a><a name="p736312951011"></a><a href="#li1555613824219">ShapeInfo</a> type, the shape information of **LocalTensor** or **GlobalTensor**.</p></td>
</tr>
</tbody>
</table>

