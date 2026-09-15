# TensorTrait Overview<a name="ZH-CN_TOPIC_0000002337196216"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:02:43.812Z -->

The **TensorTrait** data structure is a base template class that describes Tensor-related information, including the Tensor data type, logical position, and **Layout** memory layout. With template metaprogramming techniques, this class performs computation and code generation at compile time, thereby reducing runtime overhead.

## Header Files to Include<a name="zh-cn_topic_0000002213064918_section78885814919"></a>

```cpp
#include "kernel_operator_tensor_trait.h"
```

## Prototype<a name="section10580930144614"></a>

```cpp
template <typename T, TPosition pos = TPosition::GM, typename LayoutType = Layout<Shape<>, Stride<>>>
struct TensorTrait {
    using LiteType = T;
    using LiteLayoutType = LayoutType;
    static constexpr const TPosition tPos = pos; // Reserved for future feature extension.
public:
    __aicore__ inline TensorTrait(const LayoutType& t = {});

    __aicore__ inline LayoutType& GetLayout();
    __aicore__ inline const LayoutType& GetLayout() const;

    __aicore__ inline void SetLayout(const LayoutType& t);

};
```

## Template Parameters<a name="section116801320102618"></a>

**Table 1** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>T</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p1466165112529"><a name="p1466165112529"></a><a name="p1466165112529"></a>Only the following base data types are supported: **int4b_t**, **uint8_t**, **int8_t**, **int16_t**, **uint16_t**, **bfloat16_t**, **int32_t**, **uint32_t**, **int64_t**, **uint64_t**, **float**, and **half**.</p>
<p id="p9673541185614"><a name="p9673541185614"></a><a name="p9673541185614"></a><span>Inside the **TensorTrait** structure, the </span>using<span> keyword defines a type alias </span>**LiteType**<span>, which is consistent with the type of template parameter **T**.</span></p>
<p id="p17381434135715"><a name="p17381434135715"></a><a name="p17381434135715"></a><span>defined through **TensorTrait**</span>**LocalTensor**/**GlobalTensor** do not contain <a href="../ShapeInfo.md">ShapeInfo</a> information.</p>
<p id="p18609195511344"><a name="p18609195511344"></a><a name="p18609195511344"></a>For example, the Tensor without **ShapeInfo** information corresponding to **LocalTensor**&lt;**float**&gt; is **LocalTensor**&lt;**TensorTrait**&lt;**float**&gt;&gt;.</p></td>
</tr>
<tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>pos</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p1401735165413"><a name="p1401735165413"></a><a name="p1401735165413"></a>Logical position where data is stored, of the <a href="../TPosition.md">Tposition</a> type, defaulting to **TPosition::GM**.</p></td>
</tr>
<tr id="row1076563718543"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p167661637135419"><a name="p167661637135419"></a><a name="p167661637135419"></a>LayoutType</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p1776623725413"><a name="p1776623725413"></a><a name="p1776623725413"></a><a href="../Layout/Layout.md">Layout</a> data type, which defaults to an empty type, that is, Layout&lt;Shape&lt;&gt;, Stride&lt;&gt;&gt;.</p>
<p id="p183241815132114"><a name="p183241815132114"></a><a name="p183241815132114"></a>The input data type LayoutType must satisfy the <a href="../Layout/layout_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511">Constraints</a>.</p></td>
</tr>
</tbody>
</table>

## Member Functions<a name="zh-cn_topic_0000002213064918_section1173524710"></a>

```cpp
__aicore__ inline TensorTrait(const LayoutType& t = {})
__aicore__ inline LayoutType& GetLayout()
__aicore__ inline const LayoutType& GetLayout() const
__aicore__ inline void SetLayout(const LayoutType& t)
```

## Related APIs<a name="section104554349817"></a>

```cpp
// Constructor of the TensorTrait structure.
template <typename T, TPosition pos, typename LayoutType>
__aicore__ inline constexpr auto MakeTensorTrait(const LayoutType& t)

// Prototype definition of is_tensorTrait.
template <typename T> struct is_tensorTrait
```

## Constraints<a name="section1253618294449"></a>

-   The same API does not support simultaneously passing **GlobalTensor**/**LocalTensor** of the **TensorTrait** type and **GlobalTensor**/**LocalTensor** of the non-**TensorTrait** type.
-   Copy construction and assignment operators are not supported between **GlobalTensor**/**LocalTensor** of the non-**TensorTrait** type and those of the **TensorTrait** type.
-   The **TensorTrait** feature currently supports only the following APIs:

    > [!NOTE]Note
    >-   When used with APIs, configuring the **pos** and **LayoutType** template parameters of the **TensorTrait** structure is currently not supported. You need to use the constructor to construct **TensorTrait**, and **pos** and **LayoutType** can retain their default values.
    >-   The **DataCopy** slice data movement API requires **ShapeInfo** information and does not support **GlobalTensor**/**LocalTensor** of the **TensorTrait** type.

    **Table 2**  APIs supported by the TensorTrait feature

    <a name="table5536122919441"></a>
    <table><thead align="left"><tr id="row9536132974410"><th class="cellrowborder" valign="top" width="36.69%" id="mcps1.2.3.1.1"><p id="p053720299442"><a name="p053720299442"></a><a name="p053720299442"></a>API Category</p>
    </th>
    <th class="cellrowborder" valign="top" width="63.31%" id="mcps1.2.3.1.2"><p id="p15537112974412"><a name="p15537112974412"></a><a name="p15537112974412"></a>API Name</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row2053714299447"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p145371529164420"><a name="p145371529164420"></a><a name="p145371529164420"></a>Base API &gt; Resource Management &gt; TQue/TQueBind</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p65371629164414"><a name="p65371629164414"></a><a name="p65371629164414"></a>AllocTensor, FreeTensor, EnQue, DeQue</p>
    </td>
    </tr>
    <tr id="row10537182994411"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p10537152994413"><a name="p10537152994413"></a><a name="p10537152994413"></a>Base API&gt;Vector Computation&gt;Basic Arithmetic</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p1353752917440"><a name="p1353752917440"></a><a name="p1353752917440"></a>Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, Add, Sub, Mul, Div, Max, Min, Adds, Muls, Maxs, Mins, VectorPadding, BilinearInterpolation, Prelu, Mull, LeakyRelu</p>
    </td>
    </tr>
    <tr id="row753710292448"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p953782918440"><a name="p953782918440"></a><a name="p953782918440"></a>Base API&gt;Vector Computation&gt;Logical Computation</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p1453712918442"><a name="p1453712918442"></a><a name="p1453712918442"></a>And, Or<span id="ph14188188114718"><a name="ph14188188114718"></a><a name="ph14188188114718"></a>, ShiftRight, ShiftLeft</span></p>
    </td>
    </tr>
    <tr id="row1353742918445"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p1653742954413"><a name="p1653742954413"></a><a name="p1653742954413"></a>Base API&gt;Vector Computation&gt;Composite Computation</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p1453742917447"><a name="p1453742917447"></a><a name="p1453742917447"></a>CastDequant, AddRelu, AddDeqRelu, SubRelu, MulAddDst, FusedMulAdd, MulAddRelu, AddReluCast, ExpSub, AbsSub, SubReluCast, MulCast</p>
    </td>
    </tr>
    <tr id="row3537129184413"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p15537329144411"><a name="p15537329144411"></a><a name="p15537329144411"></a>Base API&gt;Data Movement</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p753742915441"><a name="p753742915441"></a><a name="p753742915441"></a>DataCopy, Copy</p>
    </td>
    </tr>
    <tr id="row1153772954413"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p35371729114419"><a name="p35371729114419"></a><a name="p35371729114419"></a>Base API&gt;Cube Computation</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p15538192954414"><a name="p15538192954414"></a><a name="p15538192954414"></a>Fill, LoadData, LoadDataWithTranspose, SetAippFunctions, LoadImageToLocal, LoadUnzipIndex, LoadDataUnzip, LoadDataWithSparse, Mmad, MmadWithSparse, BroadCastVecToMM, Gemm, Fixpipe</p>
    </td>
    </tr>
    <tr id="row61987635612"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p519919617562"><a name="p519919617562"></a><a name="p519919617562"></a>Base API &gt; Vector Computation &gt; Comparison and Selection</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p171999635617"><a name="p171999635617"></a><a name="p171999635617"></a>Compare, GetCmpMask, SetCmpMask, Select, GatherMask</p>
    </td>
    </tr>
    <tr id="row17369134075319"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p19751786543"><a name="p19751786543"></a><a name="p19751786543"></a>Base API &gt; Vector Computation &gt; Type Conversion</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p13370194016534"><a name="p13370194016534"></a><a name="p13370194016534"></a>Cast, Truncate</p>
    </td>
    </tr>
    <tr id="row2480185119548"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p1748015518541"><a name="p1748015518541"></a><a name="p1748015518541"></a>Base API &gt; Vector Computation &gt; Reduction Computation</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p948010513543"><a name="p948010513543"></a><a name="p948010513543"></a>ReduceMax, ReduceMin, ReduceSum, ReduceDataBlock, ReduceRepeat, ReducePairElem</p>
    </td>
    </tr>
    <tr id="row19583512577"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p45841411577"><a name="p45841411577"></a><a name="p45841411577"></a>Base API &gt; Vector Computation &gt; Data Conversion</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p135844110574"><a name="p135844110574"></a><a name="p135844110574"></a>Transpose, TransDataTo5HD</p>
    </td>
    </tr>
    <tr id="row5272114215616"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p1272134245615"><a name="p1272134245615"></a><a name="p1272134245615"></a>Base API &gt; Vector Computation &gt; Data Filling</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p122721642185615"><a name="p122721642185615"></a><a name="p122721642185615"></a>Brcb, Duplicate (only APIs without the scalar parameter are supported).</p>
    </td>
    </tr>
    <tr id="row2866124135816"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p786712415810"><a name="p786712415810"></a><a name="p786712415810"></a>Base API > Vector Computation > Discrete and Aggregation.</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p286711411582"><a name="p286711411582"></a><a name="p286711411582"></a>Gather, Gatherb, Scatter.</p>
    </td>
    </tr>
    <tr id="row673710289580"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p773717281588"><a name="p773717281588"></a><a name="p773717281588"></a>Base API > Vector Computation > Data Rearrangement (ISASI).</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p1473732895810"><a name="p1473732895810"></a><a name="p1473732895810"></a>Interleave, DeInterleave.</p>
    </td>
    </tr>
    <tr id="row49061051165820"><td class="cellrowborder" valign="top" width="36.69%" headers="mcps1.2.3.1.1 "><p id="p490625113583"><a name="p490625113583"></a><a name="p490625113583"></a>Base API > Vector Computation > Sorting and Combination (ISASI).</p>
    </td>
    <td class="cellrowborder" valign="top" width="63.31%" headers="mcps1.2.3.1.2 "><p id="p490655195819"><a name="p490655195819"></a><a name="p490655195819"></a>ProposalConcat, ProposalExtract, RpSort16, MrgSort4, Sort32.</p>
    </td>
    </tr>
    </tbody>
    </table>
