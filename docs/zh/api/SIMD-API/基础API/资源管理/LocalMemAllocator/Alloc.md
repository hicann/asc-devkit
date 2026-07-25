# Alloc<a name="ZH-CN_TOPIC_0000002303937138"></a>

## 产品支持情况<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_tensor.h"`。

根据用户指定的逻辑位置、数据类型、数据长度返回对应的LocalTensor对象。

## 函数原型<a name="section620mcpsimp"></a>

-   原型1：tileSize为模板参数

    ```cpp
    // 当tileSize为常量时，建议使用此接口，以获得更优的性能
    template <class DataType, uint32_t tileSize> LocalTensor<DataType> __aicore__ inline Alloc()
    template <TPosition pos, class DataType, uint32_t tileSize> __aicore__ inline LocalTensor<DataType> Alloc()
    ```

-   原型2：tileSize为接口入参

    ```cpp
    // 当tileSize为动态参数时使用此接口
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize)
    template <TPosition pos, class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize)
    ```

-   原型3：使用TensorTrait时使用此接口

    ```cpp
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc()
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  原型1和原型2模板参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="17.43%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="82.57%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row18835145716587"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>pos</p>
</td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p18689719202918"><a name="p18689719202918"></a><a name="p18689719202918"></a><a href="../../辅助数据结构//TPosition.md">TPosition</a>位置，需要符合<a href="LocalMemAllocator简介.md">LocalMemAllocator</a>中指定的Hardware物理位置（C++ Tensor编程场景下，此参数可以省略）。</p>
</td>
</tr>
<tr id="row199212475441"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p352033512453"><a name="p352033512453"></a><a name="p352033512453"></a>DataType</p>
</td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p4921114784410"><a name="p4921114784410"></a><a name="p4921114784410"></a>LocalTensor的数据类型，只支持基础数据类型，不支持TensorTrait类型。</p>
</td>
</tr>
<tr id="row187531218114513"><td class="cellrowborder" valign="top" width="17.43%" headers="mcps1.2.3.1.1 "><p id="p18753151854519"><a name="p18753151854519"></a><a name="p18753151854519"></a>tileSize</p>
</td>
<td class="cellrowborder" valign="top" width="82.57%" headers="mcps1.2.3.1.2 "><p id="p16753518194519"><a name="p16753518194519"></a><a name="p16753518194519"></a>LocalTensor的元素个数，其数量不应超过当前物理位置剩余的内存空间。</p>
</td>
</tr>
</tbody>
</table>

**表2**  原型2参数说明

<a name="table10918948849"></a>
<table><thead align="left"><tr id="row159181048845"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="p89187484413"><a name="p89187484413"></a><a name="p89187484413"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="p179183480411"><a name="p179183480411"></a><a name="p179183480411"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="p1291894812414"><a name="p1291894812414"></a><a name="p1291894812414"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row14918104812413"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p391811481242"><a name="p391811481242"></a><a name="p391811481242"></a>tileSize</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p1591812481240"><a name="p1591812481240"></a><a name="p1591812481240"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p191810481240"><a name="p191810481240"></a><a name="p191810481240"></a>LocalTensor的元素个数，其数量不应超过当前物理位置剩余的内存空间。</p>
<p id="p3875183944919"><a name="p3875183944919"></a><a name="p3875183944919"></a><span>剩余的内存空间可以通过物理内存最大值与当前可用内存地址（</span><a href="GetCurAddr-72.md">GetCurAddr</a><span>返回值）的差值来计算。</span></p>
</td>
</tr>
</tbody>
</table>

**表3**  原型3模板参数说明

<a name="table645894610463"></a>
<table><thead align="left"><tr id="row12458246134618"><th class="cellrowborder" valign="top" width="27.839999999999996%" id="mcps1.2.3.1.1"><p id="p045817465464"><a name="p045817465464"></a><a name="p045817465464"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="72.16%" id="mcps1.2.3.1.2"><p id="p15458446154614"><a name="p15458446154614"></a><a name="p15458446154614"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row945854624614"><td class="cellrowborder" valign="top" width="27.839999999999996%" headers="mcps1.2.3.1.1 "><p id="p8458184610468"><a name="p8458184610468"></a><a name="p8458184610468"></a>TensorTraitType</p>
</td>
<td class="cellrowborder" valign="top" width="72.16%" headers="mcps1.2.3.1.2 "><p id="p10458194684613"><a name="p10458194684613"></a><a name="p10458194684613"></a>只支持传入<a href="../../辅助数据结构/TensorTrait/TensorTrait.md">TensorTrait</a>类型，TensorTrait的数据类型/逻辑位置/Shape大小需要匹配LocalMemAllocator中指定的物理位置及其剩余空间。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

根据用户输入构造的LocalTensor对象。

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，对UB的内存分配，会基于静态内存的基础上进行分配，即动态内存初始位置，参考[内存层级](../../../../../guide/编程指南/高级编程/高级AI-Core编程模型/SIMD与SIMT混合编程/内存层级.md)。
<!-- end id8 -->
- 当前暂不支持与L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, BiasTable静态数组混用。

## 调用示例<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```cpp
template <uint32_t v>
using UIntImm = Std::integral_constant<uint32_t, v>;
...
AscendC::LocalMemAllocator allocator;
// 原型1：float类型，Tensor中有1024个元素，用户可以指定逻辑位置(或者不指定，由Alloc函数根据物理位置给出默认值，不影响功能)
auto tensor1 = allocator.Alloc<AscendC::TPosition::VECIN, float, 1024>();
auto tensor1 = allocator.Alloc<float, 1024>();

// 原型2：float类型，Tensor中有tileLength个元素，用户可以指定逻辑位置(或者不指定，由Alloc函数根据物理位置给出默认值，不影响功能)
auto tensor1 = allocator.Alloc<AscendC::TPosition::VECIN, float>(tileLength);

// 原型3：用户指定逻辑位置VECIN，数据类型为float，Tensor中元素个数为16*16*16
auto shape = AscendC::MakeShape(UIntImm<16>{}, UIntImm<16>{}, UIntImm<16>{});
auto stride = AscendC::MakeStride(UIntImm<0>{}, UIntImm<0>{}, UIntImm<0>{});
auto layoutMake = AscendC::MakeLayout(shape, stride);
auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
auto tensor3 = allocator.Alloc<decltype(tensorTraitMake)>();
```
