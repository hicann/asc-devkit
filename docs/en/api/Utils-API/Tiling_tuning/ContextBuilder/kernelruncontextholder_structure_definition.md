# KernelRunContextHolder Structure Definition<a name="ZH-CN_TOPIC_0000002114052957"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T15:54:52.886Z pushedAt=2026-09-11T06:35:23.041Z -->

## Description<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_section618mcpsimp"></a>

This structure is the final construction result of the **ContextBuilder** class. You can obtain internal operator information or the **KernelContext** class object through the specified interfaces.

## Prototype<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_section620mcpsimp"></a>

```
struct KernelRunContextHolder {
    KernelRunContextHolder();
    ~KernelRunContextHolder();
    template<typename T>
    T *GetContext() const
    {
        return reinterpret_cast<T*>(context);
    }
    gert::ComputeNodeInfo *MutableComputeNodeInfo()
    {
        return reinterpret_cast<gert::ComputeNodeInfo *>(computeNodeExtendHolder.get());
    }
    std::unique_ptr<ValueHolderImpl> valueHolder;
    std::unique_ptr<uint8_t[]> computeNodeExtendHolder;
    KernelRunContext *context {nullptr};
};
```

## Description<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_section622mcpsimp"></a>

**Table 1** Function description

<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_table18149577913"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_row61411571196"><th class="cellrowborder" valign="top" width="15.409999999999998%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p2093713281104"><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p2093713281104"></a><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p2093713281104"></a>Function Name</p></th>
<th class="cellrowborder" valign="top" width="21.34%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p1593811282101"><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p1593811282101"></a><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p1593811282101"></a>Input Parameter</p></th>
<th class="cellrowborder" valign="top" width="63.24999999999999%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p393813285106"><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p393813285106"></a><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p393813285106"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_row8906103284616"><td class="cellrowborder" valign="top" width="15.409999999999998%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p39061832114616"><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p39061832114616"></a><a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_p39061832114616"></a>GetContext</p></td>
<td class="cellrowborder" valign="top" width="21.34%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001872423361_p945293784316"><a name="zh-cn_topic_0000001872423361_p945293784316"></a><a name="zh-cn_topic_0000001872423361_p945293784316"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001872423361_p7697185182511"><a name="zh-cn_topic_0000001872423361_p7697185182511"></a><a name="zh-cn_topic_0000001872423361_p7697185182511"></a>Obtains the pointer to the context member variable cast to template T, where T can be KernelContext or its subclasses such as TilingContext.</p></td>
</tr>
<tr id="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_row433315311399"><td class="cellrowborder" valign="top" width="15.409999999999998%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001872423361_p69171825453"><a name="zh-cn_topic_0000001872423361_p69171825453"></a><a name="zh-cn_topic_0000001872423361_p69171825453"></a>MutableComputeNodeInfo</p></td>
<td class="cellrowborder" valign="top" width="21.34%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001872423361_p1591714255514"><a name="zh-cn_topic_0000001872423361_p1591714255514"></a><a name="zh-cn_topic_0000001872423361_p1591714255514"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001872423361_p149168253519"><a name="zh-cn_topic_0000001872423361_p149168253519"></a><a name="zh-cn_topic_0000001872423361_p149168253519"></a>Returns the constructed gert::ComputeNodeInfo class pointer.</p></td>
</tr>
</tbody>
</table>

**Table 2** Variable description

<a name="zh-cn_topic_0000001872423361_table161170462134"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001872423361_row511724611310"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001872423361_p91171467133"><a name="zh-cn_topic_0000001872423361_p91171467133"></a><a name="zh-cn_topic_0000001872423361_p91171467133"></a>Variable Name</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001872423361_p61171646141315"><a name="zh-cn_topic_0000001872423361_p61171646141315"></a><a name="zh-cn_topic_0000001872423361_p61171646141315"></a>Variable Meaning</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001872423361_row12117104671311"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001872423361_p13118746101310"><a name="zh-cn_topic_0000001872423361_p13118746101310"></a><a name="zh-cn_topic_0000001872423361_p13118746101310"></a>valueHolder</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001872423361_p1611810464138"><a name="zh-cn_topic_0000001872423361_p1611810464138"></a><a name="zh-cn_topic_0000001872423361_p1611810464138"></a>Smart pointer that ensures the internal value of KernelRunContextHolder is not destructed.</p></td>
</tr>
<tr id="zh-cn_topic_0000001872423361_row121189469136"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001872423361_p3118646181314"><a name="zh-cn_topic_0000001872423361_p3118646181314"></a><a name="zh-cn_topic_0000001872423361_p3118646181314"></a>computeNodeExtendHolder</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001872423361_p73531246172518"><a name="zh-cn_topic_0000001872423361_p73531246172518"></a><a name="zh-cn_topic_0000001872423361_p73531246172518"></a>Smart pointer that can be converted to the ComputeNodeInfo class.</p></td>
</tr>
<tr id="zh-cn_topic_0000001872423361_row1367219464226"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001872423361_p7673164602210"><a name="zh-cn_topic_0000001872423361_p7673164602210"></a><a name="zh-cn_topic_0000001872423361_p7673164602210"></a>context</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001872423361_p56731046172212"><a name="zh-cn_topic_0000001872423361_p56731046172212"></a><a name="zh-cn_topic_0000001872423361_p56731046172212"></a>Pointer to the KernelRunContext class.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_section633mcpsimp"></a>

None

## Example<a name="zh-cn_topic_0000001872423361_zh-cn_topic_0000001441184464_section642mcpsimp"></a>

```
auto holder = context_ascendc::ContextBuilder().Inputs().Outputs().BuildKernelRunContext();
if (holder != nullptr) {
    gert::KernelContext* tilingParseContext = holder->GetContext<gert::KernelContext>();
    gert::ComputeNodeInfo* info = holder->MutableComputeNodeInfo();
}
```

