# COPY\_TILING\_WITH\_ARRAY<a name="ZH-CN_TOPIC_0000002414439318"></a>

<!-- md-trans-meta sourceCommit=9f6ce11d8b0e7d296d9c9a7af96f6025bf5b48d1 translatedAt=2026-08-27T18:11:13.953Z -->

## Applicable Products<a name="section1550532418818"></a>

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Copies the array content of a specified size to the target array and returns a pointer to the copied array. It is suitable for copying a member array variable of a structure. This macro copies the specified array to the stack, making it suitable for scenarios that involve frequently accessing Tiling data, and it can accelerate data access.

## Prototype<a name="zh-cn_topic_0000001526206862_section1630753514297"></a>

```
COPY_TILING_WITH_ARRAY(arr_type, arr_count, src_ptr, dst_ptr)
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

<a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row472122142712"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p6721528274"><a name="p6721528274"></a><a name="p6721528274"></a>**arr_type**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p17721152142712"><a name="p17721152142712"></a><a name="p17721152142712"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p372120282711"><a name="p372120282711"></a><a name="p372120282711"></a>Specifies the type of the array to be copied.</p></td>
</tr>
<tr id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2340183613156"></a>**arr_count**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p320343694214"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p320343694214"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a><a name="zh-cn_topic_0000001526206862_zh-cn_topic_0000001389783361_p2684123934216"></a>Specifies the size of the array to be copied.</p></td>
</tr>
<tr id="zh-cn_topic_0000001526206862_row1239183183016"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001526206862_p223953193015"><a name="zh-cn_topic_0000001526206862_p223953193015"></a><a name="zh-cn_topic_0000001526206862_p223953193015"></a>**src_ptr**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001526206862_p7239938308"><a name="zh-cn_topic_0000001526206862_p7239938308"></a><a name="zh-cn_topic_0000001526206862_p7239938308"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001526206862_p72396320307"><a name="zh-cn_topic_0000001526206862_p72396320307"></a><a name="zh-cn_topic_0000001526206862_p72396320307"></a>Pointer to the **tiling_struct** structure.</p></td>
</tr>
<tr id="row17376172914010"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p1237615295409"><a name="p1237615295409"></a><a name="p1237615295409"></a>**dst_ptr**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p6376162913400"><a name="p6376162913400"></a><a name="p6376162913400"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p1537672974019"><a name="p1537672974019"></a><a name="p1537672974019"></a>Returns the pointer to the copied array of type **arr_type** and size **arr_count**.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   This macro must be used in the operator Kernel code, and the passed `dst_ptr` parameter does not need a declared type.
-   This macro must be used together with [GET_TILING_DATA_PTR_WITH_STRUCT](GET_TILING_DATA_PTR_WITH_STRUCT.md). The input parameter `src_ptr` is the pointer obtained by GET_TILING_DATA_PTR_WITH_STRUCT.
-   The array pointed to by the `dst_ptr` pointer obtained by this macro is a local variable. Ensure that it is used within a reasonable scope.
-   Kernel direct invocation projects are not supported for now.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *tiling)
{
    KernelAdd op;

    GET_TILING_DATA_PTR_WITH_STRUCT(AddCustomTilingData, tilingDataPtr, tiling);
 
    if ASCEND_IS_AIV {
        COPY_TILING_WITH_ARRAY(uint64_t, 2, tilingDataPtr->vectorTilingArray, vTilingArrayPtr);        
        op.Init(x, y, z, (*vTilingArrayPtr)[0], (*vTilingArrayPtr)[1]);
        op.Process();
    } else {
        COPY_TILING_WITH_ARRAY(uint64_t, 2, tilingDataPtr->cubeTilingArray, cTilingArrayPtr);
	op.Init(x, y, z, (*cTilingArrayPtr)[0], (*cTilingArrayPtr)[1]);
	op.Process();
    }
}
```

