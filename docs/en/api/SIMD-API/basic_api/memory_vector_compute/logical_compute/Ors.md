# Ors<a name="ZH-CN_TOPIC_0000002132550008"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:43:47.743Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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


## Description<a name="section618mcpsimp"></a>

Performs a bitwise OR operation between each element in a vector and a scalar. Both scenarios where the scalar is placed before or after the vector are supported. The scalar input supports configuring a LocalTensor single-point element. The calculation formula is as follows, where idx indicates the position coefficient of the LocalTensor single-point element.

![](../../../../figures/zh-cn_formulaimage_0000002392198093.png)

## Prototype<a name="section620mcpsimp"></a>

-   Computation on the first n data of a Tensor

    ```
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

-   Tensor high-dimensional splitting computation
    -   Bitwise mask mode

        ```
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   Continuous mask mode

        ```
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="13.91%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.09%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><span id="ph1756763773511"><a name="ph1756763773511"></a><a name="ph1756763773511"></a>Reserved parameter, not enabled yet. It is reserved for future function extension. When it needs to be specified, pass the default value BinaryDefaultType.</span></p></td>
</tr>
<tr id="zh-cn_topic_0000001429830437_row18835145716587"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p1383515717581"><a name="zh-cn_topic_0000001429830437_p1383515717581"></a><a name="zh-cn_topic_0000001429830437_p1383515717581"></a>isSetMask</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="p86011457203610"><a name="p86011457203610"></a><a name="p86011457203610"></a>Whether to set the mask mode and mask value inside the API.</p>
<a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><a name="zh-cn_topic_0000001429830437_ul1163765616511"></a><ul id="zh-cn_topic_0000001429830437_ul1163765616511"><li>true: set inside the API.<p id="p1715792133310"><a name="p1715792133310"></a><a name="p1715792133310"></a>The Tensor high-dimensional splitting computation API/Tensor first-n-data computation API internally uses the <span id="ph9187131991312"><a name="ph9187131991312"></a><a name="ph9187131991312"></a><a href="https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/library_api/basic_api/quick_reference/how_to_use_mask_operation_api.md" target="_blank" rel="noopener noreferrer">Normal mode/Counter mode</a></span> of the mask. In general, keep the default value of isSetMask, which means that the mask mode and mask value are set inside the API based on the mask/count parameters passed by the developer.</p>
</li><li>false: set outside the API.<a name="ul687683044913"></a><a name="ul687683044913"></a><ul id="ul687683044913"><li>For the Tensor high-dimensional splitting computation API, in some performance-critical scenarios, the developer needs to use <a href="../mask_operations/SetMaskNorm.md">SetMaskNorm</a>/<a href="../mask_operations/SetMaskCount.md">SetMaskCount</a> to set the mask mode, and use <a href="../mask_operations/SetVectorMask.md">SetVectorMask</a> to set the mask value. The mask value in the API input parameters is set to the placeholder MASK_PLACEHOLDER, which is used only as a placeholder and has no actual meaning.</li><li>For the Tensor first-n-data computation API, in some performance-critical scenarios, the developer needs to use <a href="../mask_operations/SetMaskCount.md">SetMaskCount</a> to set the mask mode to Counter mode, and use <a href="../mask_operations/SetVectorMask.md">SetVectorMask</a> to set the mask value. The count in the input parameters of this API does not take effect, and it is recommended to set it to 1.</li></ul>
</li></ul>
<p id="p56771501927"><a name="p56771501927"></a><a name="p56771501927"></a>For the following models, the isSetMask parameter in the Tensor first-n-data computation API does not take effect, and the default value can be retained.</p>
<a name="ul197761202315"></a><a name="ul197761202315"></a><ul id="ul197761202315"><li>For <span id="ph0483189202"><a name="ph0483189202"></a><a name="ph0483189202"></a>Ascend 950PR/Ascend 950DT</span></li></ul></td>
</tr>
<tr id="row199319230407"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="p10993523144019"><a name="p10993523144019"></a><a name="p10993523144019"></a>config</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="p14393347152310"><a name="p14393347152310"></a><a name="p14393347152310"></a>The type is BinaryConfig. It takes effect when the scalar is a LocalTensor single-point element type, and is used to specify the position of the single-point element operand. The default value is DEFAULT_BINARY_CONFIG, which indicates that the right operand is a scalar.</p>
<a name="screen19993123154018"></a><a name="screen19993123154018"></a><pre class="screen" codetype="Cpp" id="screen19993123154018">struct BinaryConfig {
    int8_t scalarTensorIndex = 1; // Used to specify the position of the scalar when the scalar is a LocalTensor single-point element. 0 indicates the left operand, and 1 indicates the right operand.
};
constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};</pre></td>
</tr>
<tr id="row149932232402"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="p12993623134014"><a name="p12993623134014"></a><a name="p12993623134014"></a>U</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="p3172440173612"><a name="p3172440173612"></a><a name="p3172440173612"></a>The LocalTensor type. The corresponding data type is automatically inferred based on the input parameter dst. The developer does not need to configure this parameter, but only needs to ensure that dst meets the data type constraints.</p></td>
</tr>
<tr id="row199482334014"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="p19945234402"><a name="p19945234402"></a><a name="p19945234402"></a>S</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="p151721640153610"><a name="p151721640153610"></a><a name="p151721640153610"></a>The LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src0. The developer does not need to configure this parameter, but only needs to ensure that src0 meets the data type constraints.</p></td>
</tr>
<tr id="row11994923144020"><td class="cellrowborder" valign="top" width="13.91%" headers="mcps1.2.3.1.1 "><p id="p18994323124018"><a name="p18994323124018"></a><a name="p18994323124018"></a>V</p></td>
<td class="cellrowborder" valign="top" width="86.09%" headers="mcps1.2.3.1.2 "><p id="p11721940193620"><a name="p11721940193620"></a><a name="p11721940193620"></a>The LocalTensor type or scalar type. The corresponding data type is automatically inferred based on the input parameter src1. The developer does not need to configure this parameter, but only needs to ensure that src1 meets the data type constraints.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table5422161464"></a>
<table><thead align="left"><tr id="row34231615465"><th class="cellrowborder" valign="top" width="14.510000000000002%" id="mcps1.2.4.1.1"><p id="p342101613466"><a name="p342101613466"></a><a name="p342101613466"></a><strong id="b1942151674616"><a name="b1942151674616"></a><a name="b1942151674616"></a>Parameter</strong></p></th>
<th class="cellrowborder" valign="top" width="6.619999999999999%" id="mcps1.2.4.1.2"><p id="p1042416174613"><a name="p1042416174613"></a><a name="p1042416174613"></a><strong id="b10421916184615"><a name="b10421916184615"></a><a name="b10421916184615"></a>Type</strong></p></th>
<th class="cellrowborder" valign="top" width="78.86999999999999%" id="mcps1.2.4.1.3"><p id="p743171610468"><a name="p743171610468"></a><a name="p743171610468"></a><strong id="b14314167465"><a name="b14314167465"></a><a name="b14314167465"></a>Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="row154371614467"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p16431016144613"><a name="p16431016144613"></a><a name="p16431016144613"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p194315165469"><a name="p194315165469"></a><a name="p194315165469"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="p1143101611469"><a name="p1143101611469"></a><a name="p1143101611469"></a>Destination operand.</p>
<p id="p64341624613"><a name="p64341624613"></a><a name="p64341624613"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p24312165461"><a name="p24312165461"></a><a name="p24312165461"></a><span id="ph5430168461"><a name="ph5430168461"></a><a name="ph5430168461"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p4432165467"><a name="p4432165467"></a><a name="p4432165467"></a><span id="ph1943116154619"><a name="ph1943116154619"></a><a name="ph1943116154619"></a>On Ascend 950PR/Ascend 950DT</span>, the supported data types are int16_t/uint16_t/int64_t/uint64_t</p></td>
</tr>
<tr id="row1043201654617"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p144371610462"><a name="p144371610462"></a><a name="p144371610462"></a>src0/src1</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p19432016194610"><a name="p19432016194610"></a><a name="p19432016194610"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="p88022033103718"><a name="p88022033103718"></a><a name="p88022033103718"></a>Source operand.</p>
<a name="ul1343573724314"></a><a name="ul1343573724314"></a><ul id="ul1343573724314"><li>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. The supported TPosition values are VECIN/VECCALC/VECOUT.<p id="p9802163310371"><a name="p9802163310371"></a><a name="p9802163310371"></a><span id="ph12803633203712"><a name="ph12803633203712"></a><a name="ph12803633203712"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p2803143314373"><a name="p2803143314373"></a><a name="p2803143314373"></a><span id="ph28036339376"><a name="ph28036339376"></a><a name="ph28036339376"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int16_t/uint16_t/int64_t/uint64_t</p>
</li><li>When the type is scalar:<p id="p157482021194415"><a name="p157482021194415"></a><a name="p157482021194415"></a><span id="ph1180353323718"><a name="ph1180353323718"></a><a name="ph1180353323718"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int16_t/uint16_t/int64_t/uint64_t</p>
</li></ul>
<p id="p380313383720"><a name="p380313383720"></a><a name="p380313383720"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row16431916134614"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p94321694619"><a name="p94321694619"></a><a name="p94321694619"></a>count</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p84317161468"><a name="p84317161468"></a><a name="p84317161468"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="p044121644612"><a name="p044121644612"></a><a name="p044121644612"></a>Number of elements involved in the computation.</p></td>
</tr>
<tr id="row6441216104619"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p2554141321313"><a name="p2554141321313"></a><a name="p2554141321313"></a>mask/mask[]</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p344101654616"><a name="p344101654616"></a><a name="p344101654616"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p0554313181312"><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><span id="zh-cn_topic_0000001530181537_ph793119540147"><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><span id="zh-cn_topic_0000001530181537_ph199431319161515"><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">mask</a></span> is used to control the elements involved in the computation within each iteration.</span></p>
<a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><ul id="zh-cn_topic_0000001530181537_ul1255411133132"><li>Bit-by-bit mode: controls which elements participate in the computation on a per-bit basis. A bit value of 1 indicates that the element participates in the computation, and 0 indicates that it does not.<p id="zh-cn_topic_0000001530181537_p121114581013"><a name="zh-cn_topic_0000001530181537_p121114581013"></a><a name="zh-cn_topic_0000001530181537_p121114581013"></a>mask is in array form. The array length and the value range of the array elements are related to the data type of the operand. When the operand is 16-bit, the array length is 2, and mask[0], mask[1]∈[0, 2<sup id="zh-cn_topic_0000001530181537_sup1411059101"><a name="zh-cn_topic_0000001530181537_sup1411059101"></a><a name="zh-cn_topic_0000001530181537_sup1411059101"></a>64</sup>-1], and they are not both 0. When the operand is 32-bit, the array length is 1, and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup1711155161017"><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a>64</sup>-1]. When the operand is 64-bit, the array length is 1, and mask[0]∈(0, 2<sup id="zh-cn_topic_0000001530181537_sup181195111019"><a name="zh-cn_topic_0000001530181537_sup181195111019"></a><a name="zh-cn_topic_0000001530181537_sup181195111019"></a>32</sup>-1].</p>
<p id="zh-cn_topic_0000001530181537_p711354105"><a name="zh-cn_topic_0000001530181537_p711354105"></a><a name="zh-cn_topic_0000001530181537_p711354105"></a>For example, mask=[8, 0], where 8=0b1000, indicates that only the 4th element participates in the computation.</p>
</li></ul>
<a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><ul id="zh-cn_topic_0000001530181537_ul18554121313135"><li>Continuous mode: indicates how many consecutive elements at the front participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed within each iteration. When the operand is 16-bit, mask∈[1, 128]. When the operand is 32-bit, mask∈[1, 64]. When the operand is 64-bit, mask∈[1, 32].</li></ul></td>
</tr>
<tr id="row944616174620"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p1444181611461"><a name="p1444181611461"></a><a name="p1444181611461"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p14416164461"><a name="p14416164461"></a><a name="p14416164461"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="p10444162462"><a name="p10444162462"></a><a name="p10444162462"></a>Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.</p>
<p id="p19449164465"><a name="p19449164465"></a><a name="p19449164465"></a>For a detailed description of this parameter, see <span id="zh-cn_topic_0000001530181537_ph43961850144019"><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">high-dimensional splitting API</a></span>.</p></td>
</tr>
<tr id="row13442161464"><td class="cellrowborder" valign="top" width="14.510000000000002%" headers="mcps1.2.4.1.1 "><p id="p104461613461"><a name="p104461613461"></a><a name="p104461613461"></a>repeatParams</p></td>
<td class="cellrowborder" valign="top" width="6.619999999999999%" headers="mcps1.2.4.1.2 "><p id="p6442165469"><a name="p6442165469"></a><a name="p6442165469"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="78.86999999999999%" headers="mcps1.2.4.1.3 "><p id="p14414161469"><a name="p14414161469"></a><a name="p14414161469"></a>Element operation control structure information. For details, see <a href="../../aux_data_structures/UnaryRepeatParams.md">UnaryRepeatParams</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   When using the Tensor high-dimensional splitting computation API, to save address space, developers can define one Tensor for both the source operand and the destination operand (that is, address overlap). The related constraints are as follows:
    -   For a single repeat (repeatTime=1), 100% complete overlap is required between the source operand and the destination operand. Partial overlap is not supported.
    -   For multiple repeats (repeatTime\>1), when a dependency exists between the operand and the destination operand, that is, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration, address overlap is not supported.
    -   In the scenario where the source operand is a LocalTensor single-point element, address overlap between the source operand and the destination operand is not supported.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   For Ascend 950PR/Ascend 950DT, uint64\_t/int64\_t data type supports only the Tensor first-n-data computation API.
-   At least one of the left operand and the right operand must be a vector. Currently, having both the left and right operands as scalars is not supported.
-   When this API passes LocalTensor single-point data as a scalar, the idx parameter must be a constant known at compile time. When a variable is passed, it must be declared as constexpr.

## Example<a name="section642mcpsimp"></a>

-   Tensor high-dimensional splitting computation example - continuous mask mode

    ```
    // dstLocal: output Tensor
    // src0Local: input Tensor
    // src1Local: input Tensor
    
    uint64_t mask = 128;
    // repeatTime = 4. Each iteration processes 128 numbers, so 4 iterations are required to compute 512 numbers.
    // dstBlkStride, srcBlkStride = 1. Within each iteration, the address interval of the data in src0 involved in the computation is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8. The address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar at the end
    AscendC::Ors(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });
    
    // Example with the scalar at the beginning
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Tensor high-dimensional splitting computation example - bitwise mask mode

    ```
    // dstLocal: output Tensor
    // src0Local: input Tensor
    // src1Local: input Tensor
    
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 numbers are processed in a single iteration, and 4 iterations are required to compute 512 numbers.
    // dstBlkStride, srcBlkStride = 1, the address interval of the data in src0 involved in computation within each iteration is 1 datablock, indicating that data is read and written continuously within a single iteration.
    // dstRepStride, srcRepStride = 8, the address interval between adjacent iterations is 8 datablocks, indicating that data is read and written continuously between adjacent iterations.
    // Example with the scalar at the end
    AscendC::Ors(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});
    
    // Example with the scalar at the beginning
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Example of computing the first n data of a Tensor

    ```
    // Example with the scalar at the end
    AscendC::Ors(dstLocal, src0Local, src1Local[0], 512);
    
    // Example with the scalar at the beginning
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

The result is as follows:

```
// Scalar at the end, src1Local[0] as the scalar
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [1]
Output data dstLocal: [1 2 3 ... 512]

// Scalar at the beginning, src0Local[0] as the scalar
Input data src0Local: [1]
Input data src1Local: [1 2 3 ... 512]
Output data dstLocal: [1 2 3 ... 512]
```
