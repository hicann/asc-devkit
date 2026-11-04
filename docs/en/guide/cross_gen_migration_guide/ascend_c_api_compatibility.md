# Ascend C API Compatibility Policy<a name="ZH-CN_TOPIC_0000002470508256"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T10:52:24.828Z pushedAt=2026-11-03T07:05:13.248Z nl=2 -->

This compatibility statement applies only to the compatibility migration guidance for Ascend C operator development. For the overall compatibility policy, see [table 1](#table18109729593). The compatibility scope does not include compiler built-in APIs, Ascend C internal implementation APIs, and the like. The compatibility described in this document falls into two categories: functional compatibility, which includes data type compatibility, API prototype compatibility, and constant compatibility; and performance compatibility, which means that for the same amount of data, the time consumed to execute an API on the new architecture is no longer than that on the old architecture.

If developers want to run an Ascend C program originally developed on [NPU architecture version 2201](../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) under [NPU architecture version 3510](../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), they need to recompile and run it on the 3510 architecture, and may need to adjust the code according to the migration guide.

**Figure 1**  Ascend C API hierarchy<a name="fig1912418519815"></a>  
![](../figures/api_hierarchy.png "Ascend C API hierarchy")


**Table 1**  Ascend C API compatibility policy

<a name="table18109729593"></a>
<table><thead align="left"><tr id="row810952912919"><th class="cellrowborder" valign="top" width="18.01%" id="mcps1.2.3.1.1"><p id="p141091429991"><a name="p141091429991"></a><a name="p141091429991"></a>API Level</p></th>
<th class="cellrowborder" valign="top" width="81.99%" id="mcps1.2.3.1.2"><p id="p0109162915915"><a name="p0109162915915"></a><a name="p0109162915915"></a>Compatibility Policy</p></th>
</tr>
</thead>
<tbody><tr id="row171096295916"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p101091029492"><a name="p101091029492"></a><a name="p101091029492"></a>Advanced API</p></td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p16109829497"><a name="p16109829497"></a><a name="p16109829497"></a>Advanced APIs guarantee compatibility within the same domain, while extended features of a specific domain do not guarantee compatibility.</p></td>
</tr>
<tr id="row1110915291898"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p31097291995"><a name="p31097291995"></a><a name="p31097291995"></a>Basic API</p></td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p31097296914"><a name="p31097296914"></a><a name="p31097296914"></a>Basic APIs are divided into compatible basic APIs and ISASI basic APIs. Compatible APIs are compatible across all architectures. ISASI APIs are architecture-related APIs and do not guarantee compatibility across architecture versions, such as the CUBE-side computation APIs LoadData and Mmad.</p></td>
</tr>
<tr id="row81091291898"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p161091129692"><a name="p161091129692"></a><a name="p161091129692"></a>Language Extension Layer C API</p></td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p2109929795"><a name="p2109929795"></a><a name="p2109929795"></a>SIMD & SIMT C APIs can be classified into compatibility APIs and architecture-specific APIs. Currently, SIMD C APIs support only architecture-specific APIs and do not yet support cross-generation compatibility.</p></td>
</tr>
<tr id="row510912292918"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p171096296911"><a name="p171096296911"></a><a name="p171096296911"></a>Compiler Built-In API</p></td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p1610919291397"><a name="p1610919291397"></a><a name="p1610919291397"></a>Compatibility is not guaranteed.</p></td>
</tr>
</tbody>
</table>
