# Xor<a name="ZH-CN_TOPIC_0000001956827101"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:14:18.593Z -->

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

This section describes two APIs for operating on **RegTensor** and **MaskReg**, respectively:

-   Operating on **RegTensor**:

    Performs a bitwise XOR (^) operation on the input data **srcReg0** and **srcReg1** based on **mask**, and writes the result to **dstReg**. The calculation formula is as follows:

    ![](../../../../figures/zh-cn_formulaimage_0000002499005628.png)

-   Operating on **MaskReg**:

    Performs a logical XOR operation on the valid bits of the two input **MaskReg** objects to obtain a new [MaskReg](../register_data_types/MaskReg.md).

## Prototype<a name="section620mcpsimp"></a>

-   For **RegTensor** operations

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Xor(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

-   For **MaskReg** operations

    ```
    __simd_callee__ inline void Xor(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

-   For **RegTensor** operations.

    **Table 1**  Template parameter description.

    <a name="table1878811121758"></a>
    <table><thead align="left"><tr id="row97885121457"><th class="cellrowborder" valign="top" width="18.2%" id="mcps1.2.3.1.1"><p id="p2078817124511"><a name="p2078817124511"></a><a name="p2078817124511"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.8%" id="mcps1.2.3.1.2"><p id="p278811214515"><a name="p278811214515"></a><a name="p278811214515"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row167887129511"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p137887124517"><a name="p137887124517"></a><a name="p137887124517"></a>**T**</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p197882121851"><a name="p197882121851"></a><a name="p197882121851"></a>Data type of the operand.</p>
    <p id="p37882126514"><a name="p37882126514"></a><a name="p37882126514"></a><span id="ph1178812126519"><a name="ph1178812126519"></a><a name="ph1178812126519"></a>Ascend 950PR/Ascend 950DT</span>: Supported data types: bool/uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t.</p>
    </td>
    </tr>
    <tr id="row1278818121756"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p87885121156"><a name="p87885121156"></a><a name="p87885121156"></a>**mode**</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p278817121952"><a name="p278817121952"></a><a name="p278817121952"></a>Selects MERGING mode or ZEROING mode.</p>
    <a name="ul978812125510"></a><a name="ul978812125510"></a><ul id="ul978812125510"><li>**ZEROING**: Elements not filtered by mask are set to zero in dst.</li><li>**MERGING**: Currently not supported.</li></ul>
    </td>
    </tr>
    <tr id="row07893121515"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p27898128518"><a name="p27898128518"></a><a name="p27898128518"></a>U</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p1678910126514"><a name="p1678910126514"></a><a name="p1678910126514"></a>**srcReg0**/**srcReg1**/**dstReg** are of the RegTensor type, for example, RegTensor&lt;uint32_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p>
    </td>
    </tr>
    </tbody>
    </table>

    **Table 2**  Parameter Description

    <a name="table378911128510"></a>
    <table><thead align="left"><tr id="row157893121054"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="p978911212516"><a name="p978911212516"></a><a name="p978911212516"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="p12789141217510"><a name="p12789141217510"></a><a name="p12789141217510"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="p1278919121515"><a name="p1278919121515"></a><a name="p1278919121515"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row278911126511"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p278915124518"><a name="p278915124518"></a><a name="p278915124518"></a>dstReg</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p678910121514"><a name="p678910121514"></a><a name="p678910121514"></a>Output</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1078910122054"><a name="p1078910122054"></a><a name="p1078910122054"></a>Destination operand.</p>
    <p id="p3789151211518"><a name="p3789151211518"></a><a name="p3789151211518"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
    </td>
    </tr>
    <tr id="row0789181219515"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p478915121056"><a name="p478915121056"></a><a name="p478915121056"></a>srcReg0</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p578981217517"><a name="p578981217517"></a><a name="p578981217517"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p87891412752"><a name="p87891412752"></a><a name="p87891412752"></a>Source operand.</p>
    <p id="p5789712252"><a name="p5789712252"></a><a name="p5789712252"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
    <p id="p197891412453"><a name="p197891412453"></a><a name="p197891412453"></a>The data type must be consistent with that of the destination operand.</p>
    </td>
    </tr>
    <tr id="row078911212512"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p18789812955"><a name="p18789812955"></a><a name="p18789812955"></a>srcReg1</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p678914121258"><a name="p678914121258"></a><a name="p678914121258"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p15789612652"><a name="p15789612652"></a><a name="p15789612652"></a>Source operand.</p>
    <p id="p3789191213516"><a name="p3789191213516"></a><a name="p3789191213516"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</p>
    <p id="p107897127515"><a name="p107897127515"></a><a name="p107897127515"></a>The data type must be consistent with that of the destination operand.</p>
    </td>
    </tr>
    <tr id="row107890126513"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p187891122520"><a name="p187891122520"></a><a name="p187891122520"></a>mask</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p27895121358"><a name="p27895121358"></a><a name="p27895121358"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p177898121258"><a name="p177898121258"></a><a name="p177898121258"></a><span id="ph4789201213510"><a name="ph4789201213510"></a><a name="ph4789201213510"></a>Valid indicator for the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Operates on **MaskReg**.

    **Table 3**  Parameter Description

    <a name="table481714378514"></a>
    <table><thead align="left"><tr id="row98171837059"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1181793716511"><a name="p1181793716511"></a><a name="p1181793716511"></a>Parameter</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p208175371155"><a name="p208175371155"></a><a name="p208175371155"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1481753715511"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p781783711518"><a name="p781783711518"></a><a name="p781783711518"></a>**dst**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p198175377514"><a name="p198175377514"></a><a name="p198175377514"></a>Destination operand.</p>
    </td>
    </tr>
    <tr id="row2818103719519"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>**src0**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p>
    </td>
    </tr>
    <tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>**src1**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>Source operand.</p>
    </td>
    </tr>
    <tr id="row3926164202714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1892618426271"><a name="p1892618426271"></a><a name="p1892618426271"></a>**mask**</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>Indicates which bits are valid during computation.</p>
    </td>
    </tr>
    </tbody>
    </table>

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

-   Operation on **RegTensor**

    ```
    template <typename T>
    __simd_vf__ inline void XorVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);       
            AscendC::Reg::Xor(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   Operation on **MaskReg**

    ```
    template <typename T>
    __simd_vf__ inline void XorVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
        AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::MaskReg dst;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < (uint16_t)repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Xor(dst, src0, src1, mask);
            AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
        }
    }
    ```

