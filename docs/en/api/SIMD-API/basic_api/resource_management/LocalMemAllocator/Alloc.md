# Alloc<a name="ZH-CN_TOPIC_0000002303937138"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:28:26.789Z pushedAt=2026-10-26T07:41:44.645Z nl=2 -->

## Applicable Products<a name="section73648168211"></a>

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/LocalMemAllocator/Alloc_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tensor.h"`.

Returns the corresponding **LocalTensor** object based on the logical position, data type, and data length specified by the user.

## Prototype<a name="section620mcpsimp"></a>

-   Prototype 1: tileSize is a template parameter

    ```cpp
    // When tileSize is a constant, use this API for better performance.
    template <class DataType, uint32_t tileSize> LocalTensor<DataType> __aicore__ inline Alloc()
    template <TPosition pos, class DataType, uint32_t tileSize> __aicore__ inline LocalTensor<DataType> Alloc()
    ```

-   Prototype 2: tileSize is an API input parameter

    ```cpp
    // Use this API when tileSize is a dynamic parameter.
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize)
    template <TPosition pos, class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize)
    ```

-   Prototype 3: Use this API when TensorTrait is used

    ```cpp
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc()
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description for prototypes 1 and 2

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="17.43%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="82.57%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row18835145716587"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>**pos**</p></td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p18689719202918"><a name="p18689719202918"></a><a name="p18689719202918"></a><a href="../../aux_data_structures//TPosition.md">TPosition</a> position, which must comply with the Hardware physical position specified in <a href="LocalMemAllocator_intro.md">LocalMemAllocator</a> (this parameter can be omitted in C++ Tensor programming scenarios).</p></td>
</tr>
<tr id="row199212475441"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p352033512453"><a name="p352033512453"></a><a name="p352033512453"></a>**DataType**</p></td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p4921114784410"><a name="p4921114784410"></a><a name="p4921114784410"></a>Data type of LocalTensor. Only basic data types are supported; TensorTrait types are not supported.</p></td>
</tr>
<tr id="row187531218114513"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p18753151854519"><a name="p18753151854519"></a><a name="p18753151854519"></a>**tileSize**</p></td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p16753518194519"><a name="p16753518194519"></a><a name="p16753518194519"></a>Number of elements in the LocalTensor. The number must not exceed the remaining memory space at the current physical position.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Prototype 2 parameter description

<a name="table10918948849"></a>
<table><thead align="left"><tr id="row159181048845"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="p89187484413"><a name="p89187484413"></a><a name="p89187484413"></a>Parameter Name</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="p179183480411"><a name="p179183480411"></a><a name="p179183480411"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="p1291894812414"><a name="p1291894812414"></a><a name="p1291894812414"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row14918104812413"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p391811481242"><a name="p391811481242"></a><a name="p391811481242"></a>tileSize</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p1591812481240"><a name="p1591812481240"></a><a name="p1591812481240"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p191810481240"><a name="p191810481240"></a><a name="p191810481240"></a>Number of elements in the LocalTensor. The number must not exceed the remaining memory space at the current physical position.</p>
<p id="p3875183944919"><a name="p3875183944919"></a><a name="p3875183944919"></a><span>The remaining memory space can be calculated as the difference between the maximum physical memory and the current available memory address (the return value of </span><a href="GetCurAddr.md">GetCurAddr</a><span>).</span></p></td>
</tr>
</tbody>
</table>

**Table 3**  Prototype 3 template parameter description

<a name="table645894610463"></a>
<table><thead align="left"><tr id="row12458246134618"><th class="cellrowborder" valign="top" width="27.839999999999996%" id="mcps1.2.3.1.1"><p id="p045817465464"><a name="p045817465464"></a><a name="p045817465464"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="72.16%" id="mcps1.2.3.1.2"><p id="p15458446154614"><a name="p15458446154614"></a><a name="p15458446154614"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row945854624614"><td class="cellrowborder" valign="top" width="27.839999999999996%" headers="mcps1.2.3.1.1 "><p id="p8458184610468"><a name="p8458184610468"></a><a name="p8458184610468"></a>**TensorTraitType**</p></td>
<td class="cellrowborder" valign="top" width="72.16%" headers="mcps1.2.3.1.2 "><p id="p10458194684613"><a name="p10458194684613"></a><a name="p10458194684613"></a>Only the <a href="../../aux_data_structures/TensorTrait/TensorTrait.md">TensorTrait</a> type can be passed in. The data type, logical position, and shape size of **TensorTrait** must match the physical position specified in **LocalMemAllocator** and its remaining space.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the **LocalTensor** object constructed based on the user input.

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT products, memory allocation for the Unified Buffer (UB) is performed on top of static memory, that is, the initial position of dynamic memory is determined based on static memory. For details, see [Memory Hierarchy](../../../../../guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md).
<!-- end id8 -->
- Mixing with static arrays of L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, and BiasTable is not supported.

## Example<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```cpp
template <uint32_t v>
using UIntImm = Std::integral_constant<uint32_t, v>;
...
AscendC::LocalMemAllocator allocator;
// Prototype 1: float type, with 1024 elements in the Tensor. The user can specify the logical position (or leave it unspecified, in which case the Alloc function provides a default value based on the physical position, which does not affect functionality).
auto tensor1 = allocator.Alloc<AscendC::TPosition::VECIN, float, 1024>();
auto tensor1 = allocator.Alloc<float, 1024>();

// Prototype 2: float type, with tileLength elements in the Tensor. The user can specify the logical position (or leave it unspecified, in which case the Alloc function provides a default value based on the physical position, which does not affect functionality).
auto tensor1 = allocator.Alloc<AscendC::TPosition::VECIN, float>(tileLength);

// Prototype 3: The user specifies the logical position VECIN, the data type is float, and the number of elements in the Tensor is 16*16*16.
auto shape = AscendC::MakeShape(UIntImm<16>{}, UIntImm<16>{}, UIntImm<16>{});
auto stride = AscendC::MakeStride(UIntImm<0>{}, UIntImm<0>{}, UIntImm<0>{});
auto layoutMake = AscendC::MakeLayout(shape, stride);
auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
auto tensor3 = allocator.Alloc<decltype(tensorTraitMake)>();
```
