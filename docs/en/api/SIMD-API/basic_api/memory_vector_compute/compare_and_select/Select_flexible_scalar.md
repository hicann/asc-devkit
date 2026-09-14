# Select (Flexible Scalar Position)<a name="ZH-CN_TOPIC_0000002171712357"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T08:16:42.285Z -->

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

Given two source operands src0 and src1, elements are selected based on the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is as follows: when a bit of selMask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.

For the tensor high-dimensional splitting computation API, the selection result described above can be further filtered based on the mask parameter. Significant bits are written to the final dst, while invalid bits retain the original dst values. For example, if src0 is \[1,2,3,4,5,6,7,8\], src1 is \[9,10,11,12,13,14,15,16\], selMask is \[0,0,0,0,1,1,1,1\], mask is \[1,1,1,1,0,0,0,0\], and the original dst value is \[-1,-2,-3,-4,-5,-6,-7,-8\], then the result dst\_temp after selection based on the bits of selMask is \[9,10,11,12,5,6,7,8\]. After filtering based on mask, the final output of dst is \[9,10,11,12,-5,-6,-7,-8\].

This selection function supports three modes:

-   Mode 0: Selects elements from two tensors based on selMask. The number of valid data in selMask is limited, depending on the data type of the source operands. In each iteration round, the selection operation is performed based on the valid bit data of selMask. The selMask used in each iteration round has the same value, that is, the valid value of selMask.
-   Mode 1: Selects elements from one tensor and one scalar based on selMask, with no limit on the valid data of selMask. During multi-round iteration, each iteration round consecutively uses different parts of selMask.
-   Mode 2: Selects elements from two tensors based on selMask, with no limit on the valid data of selMask. During multi-round iteration, each iteration round consecutively uses different parts of selMask.

For Mode 1, an API with flexible scalar position is provided.

## Prototype<a name="section620mcpsimp"></a>

-   Computation on the first n data of a tensor

    ```
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint32_t count)
    ```

-   Tensor high-dimensional splitting computation
    -   Bitwise mask mode

        ```
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    -   Continuous mask mode

        ```
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table1183814716169"></a>
<table><thead align="left"><tr id="row0838154741615"><th class="cellrowborder" valign="top" width="16.580000000000002%" id="mcps1.2.3.1.1"><p id="p683818472167"><a name="p683818472167"></a><a name="p683818472167"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.42%" id="mcps1.2.3.1.2"><p id="p208381447161619"><a name="p208381447161619"></a><a name="p208381447161619"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row166091324112514"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p186091247251"><a name="p186091247251"></a><a name="p186091247251"></a>T0</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p186091824132516"><a name="p186091824132516"></a><a name="p186091824132516"></a>Data type of the source operand and destination operand.</p>
<p id="p2094981215419"><a name="p2094981215419"></a><a name="p2094981215419"></a>In particular, for the flexible scalar position API, this is a reserved parameter that is not yet enabled and is retained for future feature extension. When it needs to be specified, pass the default value BinaryDefaultType.</p></td>
</tr>
<tr id="row135451522112520"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p1354612213257"><a name="p1354612213257"></a><a name="p1354612213257"></a>T1</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p14571141782416"><a name="p14571141782416"></a><a name="p14571141782416"></a>Data type of selMask.</p></td>
</tr>
<tr id="row18381947151619"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p683894716163"><a name="p683894716163"></a><a name="p683894716163"></a>isSetMask</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p134861920172013"><a name="p134861920172013"></a><a name="p134861920172013"></a>Reserved parameter. Keep the default value. To use the function of setting the mask outside the API, call the API that does not pass the mask parameter.</p></td>
</tr>
<tr id="row3400166112410"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p7401463247"><a name="p7401463247"></a><a name="p7401463247"></a>selMode</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p840106102420"><a name="p840106102420"></a><a name="p840106102420"></a>Same as the selMode parameter in <a href="#table8955841508">Table 2 Parameters</a>.</p></td>
</tr>
<tr id="row1010412442456"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p914318523429"><a name="p914318523429"></a><a name="p914318523429"></a>config</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p19365134614341"><a name="p19365134614341"></a><a name="p19365134614341"></a>Type: BinaryConfig. This parameter takes effect when the scalar is a single-point element of the LocalTensor type, and is used to specify the position of the single-point element operand. The default value is DEFAULT_BINARY_CONFIG, indicating that the right operand is a scalar.</p>
<a name="screen13143195284213"></a><a name="screen13143195284213"></a><pre class="screen" codetype="Cpp" id="screen13143195284213">struct BinaryConfig {
    int8_t scalarTensorIndex = 1; // Specify the position of the scalar when the scalar is a single-point element of LocalTensor. 0 indicates the left operand, and 1 indicates the right operand.
};
constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};</pre></td>
</tr>
<tr id="row125610473454"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p7144195244214"><a name="p7144195244214"></a><a name="p7144195244214"></a>T2</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p3172440173612"><a name="p3172440173612"></a><a name="p3172440173612"></a>LocalTensor type. The corresponding data type is automatically derived based on the input parameter dst. Developers do not need to configure this parameter, but must ensure that dst meets the data type constraints.</p></td>
</tr>
<tr id="row16559144914459"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p51440526428"><a name="p51440526428"></a><a name="p51440526428"></a>T3</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p151721640153610"><a name="p151721640153610"></a><a name="p151721640153610"></a>LocalTensor type or scalar type. The corresponding data type is automatically derived based on the input parameter src0. Developers do not need to configure this parameter, but must ensure that src0 meets the data type constraints.</p></td>
</tr>
<tr id="row4209185214512"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="p7144352114214"><a name="p7144352114214"></a><a name="p7144352114214"></a>T4</p></td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="p11721940193620"><a name="p11721940193620"></a><a name="p11721940193620"></a>LocalTensor type or scalar type. The corresponding data type is automatically derived based on the input parameter src1. Developers do not need to configure this parameter, but must ensure that src1 meets the data type constraints.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table8955841508"></a>
<table><thead align="left"><tr id="row15956194105014"><th class="cellrowborder" valign="top" width="16.38163816381638%" id="mcps1.2.4.1.1"><p id="p7956144195014"><a name="p7956144195014"></a><a name="p7956144195014"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.24112411241124%" id="mcps1.2.4.1.2"><p id="p1295624145013"><a name="p1295624145013"></a><a name="p1295624145013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="72.37723772377238%" id="mcps1.2.4.1.3"><p id="p16956144145011"><a name="p16956144145011"></a><a name="p16956144145011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5956546509"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p19576531173410"><a name="p19576531173410"></a><a name="p19576531173410"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p16576163119347"><a name="p16576163119347"></a><a name="p16576163119347"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="p785913161268"><a name="p785913161268"></a><a name="p785913161268"></a>Destination operand.</p>
<p id="p5945720195112"><a name="p5945720195112"></a><a name="p5945720195112"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p775563112310"><a name="p775563112310"></a><a name="p775563112310"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p087755215110"><a name="p087755215110"></a><a name="p087755215110"></a><span id="ph1886081495211"><a name="ph1886081495211"></a><a name="ph1886081495211"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64</p></td>
</tr>
<tr id="row59491678111"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p29491174110"><a name="p29491174110"></a><a name="p29491174110"></a>selMask</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p171691412131"><a name="p171691412131"></a><a name="p171691412131"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="p933512419919"><a name="p933512419919"></a><a name="p933512419919"></a>Selection mask.</p>
<p id="p1634988196"><a name="p1634988196"></a><a name="p1634988196"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p369675014511"><a name="p369675014511"></a><a name="p369675014511"></a><span id="ph4282175119455"><a name="ph4282175119455"></a><a name="ph4282175119455"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p962521419203"><a name="p962521419203"></a><a name="p962521419203"></a><span id="ph7168111512201"><a name="ph7168111512201"></a><a name="ph7168111512201"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: uint8_t/uint16_t/uint32_t/uint64_t.</p>
<p id="p448916309617"><a name="p448916309617"></a><a name="p448916309617"></a>Each bit indicates the selection of one element. When the bit of selMask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.</p>
<p id="p1199617464116"><a name="p1199617464116"></a><a name="p1199617464116"></a>When selMode is Mode 0, in each iteration round, the selection operation is performed based on the significant bits of selMask. The selMask used in each iteration round is the same value, that is, the valid value of selMask. When selMode is Mode 1/2, selMask is consumed continuously across multiple iterations.</p>
<a name="ul126291640131014"></a><a name="ul126291640131014"></a><ul id="ul126291640131014"><li>Mode 0: Selects elements from two tensors based on selMask. selMask has a bit count limit. Regardless of the number of iterations, each iteration performs selection based only on the truncated fixed number of bits of selMask. When the data type of the source operand is 8-bit, the first 256 bits of selMask are significant; when the data type of the source operand is 16-bit, the first 128 bits of selMask are significant; when the data type of the source operand is 32-bit, the first 64 bits of selMask are significant; when the data type of the source operand is 64-bit, the first 32 bits of selMask are significant.</li><li>Mode 1: Selects elements from one tensor and one scalar based on selMask. Multiple iterations are supported. The selection method is as follows: based on the configured value of selMask, if the selMask bit value is 1, the value at the same position in src0 is selected; if the selMask bit value is 0, the scalar value is selected. selMask is stored contiguously. When the data type of the source operand is 8-bit, 256-bit selMask data is obtained in one comparison; when the data type of the source operand is 16-bit, 128-bit selMask data is obtained in one comparison; when the data type of the source operand is 32-bit, 64-bit selMask data is obtained in one comparison; when the data type of the source operand is 64-bit, 32-bit selMask data is obtained in one comparison.</li><li>Mode 2: Selects elements from two tensors based on selMask. Multiple iterations are supported. The selection method is as follows: based on the configured value of selMask, if the selMask bit value is 1, the value at the same position in src0 is selected; if the selMask bit value is 0, the value at the same position in src1 is selected. selMask is stored contiguously. When the data type of the source operand is 8-bit, 256-bit selMask data is obtained in one comparison; when the data type of the source operand is 16-bit, 128-bit selMask data is obtained in one comparison; when the data type of the source operand is 32-bit, 64-bit selMask data is obtained in one comparison; when the data type of the source operand is 64-bit, 32-bit selMask data is obtained in one comparison.</li></ul></td>
</tr>
<tr id="row69959512410"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p165761231123417"><a name="p165761231123417"></a><a name="p165761231123417"></a>src0</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p757693163410"><a name="p757693163410"></a><a name="p757693163410"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><div class="p" id="p62581922192218"><a name="p62581922192218"></a><a name="p62581922192218"></a>Source operand in the flexible scalar position API.<a name="ul3585928979"></a><a name="ul3585928979"></a><ul id="ul3585928979"><li>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. The supported TPosition values are VECIN/VECCALC/VECOUT.<p id="p205584233295"><a name="p205584233295"></a><a name="p205584233295"></a><span id="ph2558172315291"><a name="ph2558172315291"></a><a name="ph2558172315291"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p12558323142913"><a name="p12558323142913"></a><a name="p12558323142913"></a><span id="ph7558182319297"><a name="ph7558182319297"></a><a name="ph7558182319297"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64</p>
</li><li>When the type is scalar:<p id="p11558142332918"><a name="p11558142332918"></a><a name="p11558142332918"></a><span id="ph75580236298"><a name="ph75580236298"></a><a name="ph75580236298"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64</p>
</li></ul>
</div>
<p id="p136651831996"><a name="p136651831996"></a><a name="p136651831996"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row4956154125018"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p10576203116349"><a name="p10576203116349"></a><a name="p10576203116349"></a>src1</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p1157693123417"><a name="p1157693123417"></a><a name="p1157693123417"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><div class="p" id="p17783113192218"><a name="p17783113192218"></a><a name="p17783113192218"></a>Source operand in the flexible scalar position API.<a name="ul825610308105"></a><a name="ul825610308105"></a><ul id="ul825610308105"><li>When the type is LocalTensor, it can be used as a vector operand or a scalar single-point element. The supported TPosition values are VECIN/VECCALC/VECOUT.<p id="p29171220133019"><a name="p29171220133019"></a><a name="p29171220133019"></a><span id="ph99171820133010"><a name="ph99171820133010"></a><a name="ph99171820133010"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p149171920183012"><a name="p149171920183012"></a><a name="p149171920183012"></a><span id="ph129172208302"><a name="ph129172208302"></a><a name="ph129172208302"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64</p>
</li><li>When the type is scalar:<p id="p1791719202308"><a name="p1791719202308"></a><a name="p1791719202308"></a><span id="ph89171020193018"><a name="ph89171020193018"></a><a name="ph89171020193018"></a>Ascend 950PR/Ascend 950DT</span>, the supported data types are: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64</p>
</li></ul>
</div>
<p id="p14981134719107"><a name="p14981134719107"></a><a name="p14981134719107"></a>The data type must be consistent with that of the destination operand.</p></td>
</tr>
<tr id="row1495634115010"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p10974181411356"><a name="p10974181411356"></a><a name="p10974181411356"></a>selMode</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p1797491412352"><a name="p1797491412352"></a><a name="p1797491412352"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="p16706144115550"><a name="p16706144115550"></a><a name="p16706144115550"></a>Instruction mode, of the SELMODE type. The values are as follows:</p>
<a name="screen6704150175512"></a><a name="screen6704150175512"></a><pre class="screen" codetype="Cpp" id="screen6704150175512">enum class SELMODE : uint8_t {
    VSEL_CMPMASK_SPR = 0, 
    VSEL_TENSOR_SCALAR_MODE,
    VSEL_TENSOR_TENSOR_MODE,
};</pre>
<a name="ul167871527102517"></a><a name="ul167871527102517"></a><ul id="ul167871527102517"><li>Mode 0: The value is VSEL_CMPMASK_SPR. Elements are selected from two tensors based on selMask. The number of valid data in selMask is limited, <span>specifically depending on the data type of the source operand.</span>In each iteration round, the selection operation is performed based on the significant bit data of selMask. The selMask used in each iteration round is the same value, that is, the valid value of selMask.</li><li>Mode 1: The value is VSEL_TENSOR_SCALAR_MODE. Elements are selected from one tensor and one scalar based on selMask. selMask has no valid data limit. During multi-round iteration, <span>each iteration round consecutively uses</span> different parts of selMask<span>.</span></li><li>Mode 2: The value is VSEL_TENSOR_TENSOR_MODE. Elements are selected from two tensors based on selMask. selMask has no valid data limit. During multi-round iteration, <span>each iteration round consecutively uses</span> different parts of selMask<span>.</span></li></ul></td>
</tr>
<tr id="row769135514428"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p175358462154"><a name="p175358462154"></a><a name="p175358462154"></a>mask/mask[]</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p10535746191515"><a name="p10535746191515"></a><a name="p10535746191515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001530181537_p0554313181312"><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><a name="zh-cn_topic_0000001530181537_p0554313181312"></a><span id="zh-cn_topic_0000001530181537_ph793119540147"><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><a name="zh-cn_topic_0000001530181537_ph793119540147"></a><span id="zh-cn_topic_0000001530181537_ph199431319161515"><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a name="zh-cn_topic_0000001530181537_ph199431319161515"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">mask</a></span> is used to control the elements that participate in the computation within each iteration.</span></p>
<a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><a name="zh-cn_topic_0000001530181537_ul1255411133132"></a><ul id="zh-cn_topic_0000001530181537_ul1255411133132"><li>Bitwise mode: You can control, bit by bit, which elements participate in the computation. A bit value of 1 indicates that the element participates in the computation, and 0 indicates that it does not.<p id="zh-cn_topic_0000001530181537_p121114581013"><a name="zh-cn_topic_0000001530181537_p121114581013"></a><a name="zh-cn_topic_0000001530181537_p121114581013"></a>mask is in array form. The array length and the value range of the array elements are related to the data type of the operand. When the operand is 16-bit, the array length is 2, and mask[0], mask[1] ∈ [0, 2<sup id="zh-cn_topic_0000001530181537_sup1411059101"><a name="zh-cn_topic_0000001530181537_sup1411059101"></a><a name="zh-cn_topic_0000001530181537_sup1411059101"></a>64</sup>-1], and they are not 0 at the same time. When the operand is 32-bit, the array length is 1, and mask[0] ∈ (0, 2<sup id="zh-cn_topic_0000001530181537_sup1711155161017"><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a><a name="zh-cn_topic_0000001530181537_sup1711155161017"></a>64</sup>-1]. When the operand is 64-bit, the array length is 1, and mask[0] ∈ (0, 2<sup id="zh-cn_topic_0000001530181537_sup181195111019"><a name="zh-cn_topic_0000001530181537_sup181195111019"></a><a name="zh-cn_topic_0000001530181537_sup181195111019"></a>32</sup>-1].</p>
<p id="zh-cn_topic_0000001530181537_p711354105"><a name="zh-cn_topic_0000001530181537_p711354105"></a><a name="zh-cn_topic_0000001530181537_p711354105"></a>For example, mask=[8, 0], where 8=0b1000, indicates that only the fourth element participates in the computation.</p>
</li></ul>
<a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><a name="zh-cn_topic_0000001530181537_ul18554121313135"></a><ul id="zh-cn_topic_0000001530181537_ul18554121313135"><li>Continuous mode: Indicates how many consecutive elements at the front participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed within each iteration. When the operand is 16-bit, mask ∈ [1, 128]. When the operand is 32-bit, mask ∈ [1, 64]. When the operand is 64-bit, mask ∈ [1, 32].</li></ul></td>
</tr>
<tr id="row12284152699"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p557663119345"><a name="p557663119345"></a><a name="p557663119345"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p195761631163416"><a name="p195761631163416"></a><a name="p195761631163416"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="p0448621183615"><a name="p0448621183615"></a><a name="p0448621183615"></a>Number of repeated iterations. The vector computing unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and computing all the data. repeatTime indicates the number of iterations.</p>
<p id="p17845145813432"><a name="p17845145813432"></a><a name="p17845145813432"></a>For details about this parameter, see <span id="zh-cn_topic_0000001530181537_ph43961850144019"><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a name="zh-cn_topic_0000001530181537_ph43961850144019"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">High-Dimensional Slicing API</a></span>.</p></td>
</tr>
<tr id="row191624543910"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p1325595674818"><a name="p1325595674818"></a><a name="p1325595674818"></a>repeatParams</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p172551556134814"><a name="p172551556134814"></a><a name="p172551556134814"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001429830437_p12596185919348"><a name="zh-cn_topic_0000001429830437_p12596185919348"></a><a name="zh-cn_topic_0000001429830437_p12596185919348"></a>Parameter that controls the address stride of the operand. It is of the <a href="../../aux_data_structures/BinaryRepeatParams.md">BinaryRepeatParams</a> type, and contains parameters such as the address stride of the same datablock between adjacent iterations of the operand and the address stride of different datablocks within the same iteration of the operand.</p>
<p id="zh-cn_topic_0000001429830437_p1156819418442"><a name="zh-cn_topic_0000001429830437_p1156819418442"></a><a name="zh-cn_topic_0000001429830437_p1156819418442"></a>For details about the address stride parameter between adjacent iterations, see <span id="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph18976413195617"><a name="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph18976413195617"></a><a name="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph18976413195617"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">repeatStride</a></span>. For details about the address stride parameter of DataBlock within the same iteration, see <span id="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph19976131385613"><a name="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph19976131385613"></a><a name="zh-cn_topic_0000001429830437_zh-cn_topic_0000001487959374_ph19976131385613"></a><a href="../SIMD_compute/high_dimension_slicing.md" target="_blank" rel="noopener noreferrer">dataBlockStride</a></span>.</p></td>
</tr>
<tr id="row17825055390"><td class="cellrowborder" valign="top" width="16.38163816381638%" headers="mcps1.2.4.1.1 "><p id="p158803470265"><a name="p158803470265"></a><a name="p158803470265"></a>count</p></td>
<td class="cellrowborder" valign="top" width="11.24112411241124%" headers="mcps1.2.4.1.2 "><p id="p7880174717266"><a name="p7880174717266"></a><a name="p7880174717266"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="72.37723772377238%" headers="mcps1.2.4.1.3 "><p id="p122367392431"><a name="p122367392431"></a><a name="p122367392431"></a>Number of elements involved in the computation.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section198548421851"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   When the flexible scalar position API is called and the source operand is a LocalTensor single-point element, address overlap between the source operand and the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   For Ascend 950PR/Ascend 950DT, the int8\_t/uint8\_t/uint64\_t/int64\_t/complex32/complex64 data type supports only the API for computing the first n data of a tensor.
-   At least one of the left operand and the right operand must be a vector; the case where both the left and right operands are scalars is not supported.
-   When this API passes LocalTensor Single-Point data as a scalar, the idx parameter must be a compile-time constant; when a variable is passed, it must be declared as constexpr.
-   When using the flexible scalar position API in mode 1, the template parameter config must be specified to prevent the API from matching other modes.

## Example<a name="section642mcpsimp"></a>

-   Example of computing the first n data of a Select-tensor (mode 1)

    ```
    // Flexible scalar position, src1Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 1 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local, src1Local[0], AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    
    // Flexible scalar position, src0Local[0] as the scalar.
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local[0], src1Local, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    ```
