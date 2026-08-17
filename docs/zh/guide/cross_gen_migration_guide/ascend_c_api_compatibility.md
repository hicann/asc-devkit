# Ascend C API兼容性策略<a name="ZH-CN_TOPIC_0000002470508256"></a>

本兼容性说明仅适用于Ascend C算子开发的兼容性迁移指导。总体兼容性策略见[表1](#table18109729593)，兼容性范围不包含编译器BuiltIn API、Ascend C内部实现接口等。文档中涉及的兼容性分为两类：一是功能兼容，包括数据类型兼容、接口原型兼容和常量兼容；二是性能兼容，指对于同等数据量，新架构上执行API耗时不高于旧架构。

若开发者希望在[NPU架构版本3510](../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)下运行原本在[NPU架构版本2201](../编程指南/语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)上开发的Ascend C程序，需在3510架构上重新编译并运行，并可能需要根据迁移指导进行代码调整。

**图1**  Ascend C API层次结构<a name="fig1912418519815"></a>  
![](../figures/api_hierarchy.png "Ascend-C-API层次结构")


**表1**  Ascend C API兼容策略

<a name="table18109729593"></a>
<table><thead align="left"><tr id="row810952912919"><th class="cellrowborder" valign="top" width="18.01%" id="mcps1.2.3.1.1"><p id="p141091429991"><a name="p141091429991"></a><a name="p141091429991"></a>API层级</p>
</th>
<th class="cellrowborder" valign="top" width="81.99%" id="mcps1.2.3.1.2"><p id="p0109162915915"><a name="p0109162915915"></a><a name="p0109162915915"></a>兼容策略</p>
</th>
</tr>
</thead>
<tbody><tr id="row171096295916"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p101091029492"><a name="p101091029492"></a><a name="p101091029492"></a>高阶API</p>
</td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p16109829497"><a name="p16109829497"></a><a name="p16109829497"></a>高阶API在同领域内可保证兼容，特定领域扩展特性不保证兼容。</p>
</td>
</tr>
<tr id="row1110915291898"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p31097291995"><a name="p31097291995"></a><a name="p31097291995"></a>基础API</p>
</td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p31097296914"><a name="p31097296914"></a><a name="p31097296914"></a>基础API分为可兼容的基础API和ISASI基础API；兼容的API在所有架构上均能兼容；ISASI API为体系架构相关的API，不保证跨架构版本的兼容性，例如CUBE侧的计算接口LoadData、Mmad等。</p>
</td>
</tr>
<tr id="row81091291898"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p161091129692"><a name="p161091129692"></a><a name="p161091129692"></a>语言扩展层C API</p>
</td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p2109929795"><a name="p2109929795"></a><a name="p2109929795"></a>SIMD & SIMT C API可分为兼容性API和体系架构相关API；当前SIMD的C API仅支持体系架构相关，暂未支持跨代兼容。</p>
</td>
</tr>
<tr id="row510912292918"><td class="cellrowborder" valign="top" width="18.01%" headers="mcps1.2.3.1.1 "><p id="p171096296911"><a name="p171096296911"></a><a name="p171096296911"></a>编译器BuiltIn API</p>
</td>
<td class="cellrowborder" valign="top" width="81.99%" headers="mcps1.2.3.1.2 "><p id="p1610919291397"><a name="p1610919291397"></a><a name="p1610919291397"></a>不保证兼容。</p>
</td>
</tr>
</tbody>
</table>
