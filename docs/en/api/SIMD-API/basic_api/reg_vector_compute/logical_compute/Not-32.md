# Not<a name="ZH-CN_TOPIC_0000001929668260"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:07:57.290Z -->

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

This section describes two APIs for operating on RegTensor and MaskReg, respectively:

-   Operating on RegTensor:

    Performs a bitwise NOT on each valid data element in the input **srcReg** and writes the result to **dstReg**.

-   Operating on MaskReg:

    Performs a bitwise NOT on each valid bit in the input **src** and writes the result to **dst**.

## Prototype<a name="section620mcpsimp"></a>

-   Operates on **RegTensor**.

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Not(U& dstReg, U& srcReg, MaskReg& mask)
    ```

-   Operates on **MaskReg**.

    ```
    __simd_callee__ inline void Not(MaskReg& dst, MaskReg& src, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

-   Performs the operation on **RegTensor**.

    **Table 1**  Template parameter description

    <a name="table4835205712588"></a>
    <table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.61%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.39%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**T**</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
    <p id="p7373174413584"><a name="p7373174413584"></a><a name="p7373174413584"></a><span id="ph1137394417584"><a name="ph1137394417584"></a><a name="ph1137394417584"></a>Ascend 950PR/Ascend 950DT</span>: Supported data types are int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t/half/float.</p>
    </td>
    </tr>
    <tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p1756419170189"><a name="p1756419170189"></a><a name="p1756419170189"></a>**mode**</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>Selects the **MERGING** mode or the **ZEROING** mode.</p>
    <a name="ul1163765616511"></a><a name="ul1163765616511"></a><ul id="ul1163765616511"><li>**ZEROING**: Elements not filtered by **mask** are set to zero in **dst**.</li><li>**MERGING**: Currently not supported.</li></ul>
    </td>
    </tr>
    <tr id="row2424151625011"><td class="cellrowborder" valign="top" width="18.61%" headers="mcps1.2.3.1.1 "><p id="p916243141912"><a name="p916243141912"></a><a name="p916243141912"></a>U</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.39%" headers="mcps1.2.3.1.2 "><p id="p15901115914145"><a name="p15901115914145"></a><a name="p15901115914145"></a><span id="ph19851723182011"><a name="ph19851723182011"></a><a name="ph19851723182011"></a>RegTensor type of the destination operand, for example, **RegTensor&lt;half&gt;**, which is automatically deduced by the compiler and does not need to be specified by the user.</span></p>
    </td>
    </tr>
    </tbody>
    </table>

    **Table 2**  Parameter Description

    <a name="table147028618289"></a>
    <table><thead align="left"><tr id="row19702156192811"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="p1670217682820"><a name="p1670217682820"></a><a name="p1670217682820"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="p20702464283"><a name="p20702464283"></a><a name="p20702464283"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="p870215616281"><a name="p870215616281"></a><a name="p870215616281"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1970206162814"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p157029620283"><a name="p157029620283"></a><a name="p157029620283"></a>dstReg</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p187024618284"><a name="p187024618284"></a><a name="p187024618284"></a>Output</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p5702164284"><a name="p5702164284"></a><a name="p5702164284"></a>Destination operand.</p>
    <p id="p27023615289"><a name="p27023615289"></a><a name="p27023615289"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
    </td>
    </tr>
    <tr id="row970220672819"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6702469281"><a name="p6702469281"></a><a name="p6702469281"></a>**srcReg**</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p57029614285"><a name="p57029614285"></a><a name="p57029614285"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p177029622816"><a name="p177029622816"></a><a name="p177029622816"></a>Source operand.</p>
    <p id="p137026616286"><a name="p137026616286"></a><a name="p137026616286"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
    <p id="p670212611287"><a name="p670212611287"></a><a name="p670212611287"></a>The data type of the source operand must be consistent with that of the destination operand.</p>
    </td>
    </tr>
    <tr id="row167021569289"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p37029612280"><a name="p37029612280"></a><a name="p37029612280"></a>**mask**</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p1970217602814"><a name="p1970217602814"></a><a name="p1970217602814"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p10702663286"><a name="p10702663286"></a><a name="p10702663286"></a><span id="ph167021767286"><a name="ph167021767286"></a><a name="ph167021767286"></a>Valid indication for the operation on the source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Operation on **MaskReg**

    **Table 3**  Parameters

    <a name="table13822195442813"></a>
    <table><thead align="left"><tr id="row28221954102815"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1482275462812"><a name="p1482275462812"></a><a name="p1482275462812"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p18822554202817"><a name="p18822554202817"></a><a name="p18822554202817"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row13822185410281"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p188221554112813"><a name="p188221554112813"></a><a name="p188221554112813"></a>**dst**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p198221454142819"><a name="p198221454142819"></a><a name="p198221454142819"></a>Destination operand.</p>
    </td>
    </tr>
    <tr id="row12822175452814"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>**src**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p>
    </td>
    </tr>
    <tr id="row3926164202714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1892618426271"><a name="p1892618426271"></a><a name="p1892618426271"></a>**mask**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>Indicates which bits are valid during computation.</p>
    </td>
    </tr>
    </tbody>
    </table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Operate on **RegTensor**.

    ```
    template <typename T>
    __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
     uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Not(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   Operate on **MaskReg**.

    ```
    template <typename T>
    __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
        AscendC::Reg::MaskReg dst;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Not(dst, src, mask);
            AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
        }
    }
    ```

