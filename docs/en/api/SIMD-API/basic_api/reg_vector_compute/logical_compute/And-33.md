# And<a name="ZH-CN_TOPIC_0000001956986921"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T07:19:19.837Z -->

## Product Support<a name="section1550532418810"></a>

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


## Function Description<a name="section618mcpsimp"></a>

This section describes two APIs for operating on RegTensor and MaskReg respectively:

-   Operating on RegTensor:

    Performs a bitwise AND (&) operation on the input data srcReg0 and srcReg1 according to mask, and writes the result to dstReg. The calculation formula is as follows:

    ![](../../../../figures/zh-cn_formulaimage_0000002499005626.png)

-   Operating on MaskReg:

    Performs a logical AND operation on the valid bits of the two input MaskReg to obtain a new [MaskReg](../register_data_types/MaskReg.md).

## Function Prototype<a name="section620mcpsimp"></a>

-   Operation on RegTensor

    ```
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void And(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

-   Operation on MaskReg

    ```
    __simd_callee__ inline void And(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
    ```

## Parameter Description<a name="section622mcpsimp"></a>

-   Operation on RegTensor

    **Table 1**  Template parameter description

    <a name="table172551105016"></a>
    <table><thead align="left"><tr id="row152514120505"><th class="cellrowborder" valign="top" width="18.2%" id="mcps1.2.3.1.1"><p id="p1325101195013"><a name="p1325101195013"></a><a name="p1325101195013"></a>Parameter Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.8%" id="mcps1.2.3.1.2"><p id="p12520118504"><a name="p12520118504"></a><a name="p12520118504"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row92541205014"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p11250110504"><a name="p11250110504"></a><a name="p11250110504"></a>T</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p625141195017"><a name="p625141195017"></a><a name="p625141195017"></a>Data type of the operand.</p>
    <p id="p1225115501"><a name="p1225115501"></a><a name="p1225115501"></a><span id="ph11257125017"><a name="ph11257125017"></a><a name="ph11257125017"></a>Ascend 950PR/Ascend 950DT</span>, supported data types: bool/uint8_t/int8_t/uint16_t/int16_t/uint32_t/int32_t/uint64_t/int64_t</p>
    </td>
    </tr>
    <tr id="row12514135013"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p52512195012"><a name="p52512195012"></a><a name="p52512195012"></a>mode</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p22511112504"><a name="p22511112504"></a><a name="p22511112504"></a>Selects the MERGING mode or ZEROING mode.</p>
    <a name="ul10251514504"></a><a name="ul10251514504"></a><ul id="ul10251514504"><li>ZEROING, elements not filtered by mask are set to zero in dst.</li><li>MERGING, currently not supported.</li></ul>
    </td>
    </tr>
    <tr id="row152591105013"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.3.1.1 "><p id="p72531125019"><a name="p72531125019"></a><a name="p72531125019"></a>U</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.8%" headers="mcps1.2.3.1.2 "><p id="p725171135010"><a name="p725171135010"></a><a name="p725171135010"></a>srcReg0/srcReg1/dstReg are of the RegTensor type, for example, RegTensor&lt;uint32_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user.</p>
    </td>
    </tr>
    </tbody>
    </table>

    **Table 2**  Parameter description

    <a name="table14261514502"></a>
    <table><thead align="left"><tr id="row3262116508"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="p6267118507"><a name="p6267118507"></a><a name="p6267118507"></a>Parameter name</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="p2026191195013"><a name="p2026191195013"></a><a name="p2026191195013"></a>Input/Output</p>
    </th>
    <th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="p126619509"><a name="p126619509"></a><a name="p126619509"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row82617110505"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p426161165010"><a name="p426161165010"></a><a name="p426161165010"></a>dstReg</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p1826171205012"><a name="p1826171205012"></a><a name="p1826171205012"></a>Output</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p20261145015"><a name="p20261145015"></a><a name="p20261145015"></a>Destination operand.</p>
    <p id="p22619115503"><a name="p22619115503"></a><a name="p22619115503"></a><span id="ph2261411508"><a name="ph2261411508"></a><a name="ph2261411508"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
    </td>
    </tr>
    <tr id="row626161145015"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p152610185010"><a name="p152610185010"></a><a name="p152610185010"></a>srcReg0</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p122611118506"><a name="p122611118506"></a><a name="p122611118506"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p16264155017"><a name="p16264155017"></a><a name="p16264155017"></a>Source operand.</p>
    <p id="p626151115013"><a name="p626151115013"></a><a name="p626151115013"></a><span id="ph18263113507"><a name="ph18263113507"></a><a name="ph18263113507"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
    <p id="p132651105015"><a name="p132651105015"></a><a name="p132651105015"></a>The data types of the two source operands must be consistent with that of the destination operand.</p>
    </td>
    </tr>
    <tr id="row42610115502"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p2269110507"><a name="p2269110507"></a><a name="p2269110507"></a>srcReg1</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p7261815500"><a name="p7261815500"></a><a name="p7261815500"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p1026171135019"><a name="p1026171135019"></a><a name="p1026171135019"></a>Source operand.</p>
    <p id="p926116509"><a name="p926116509"></a><a name="p926116509"></a><span id="ph32691135015"><a name="ph32691135015"></a><a name="ph32691135015"></a>The type is <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p>
    <p id="p1261916506"><a name="p1261916506"></a><a name="p1261916506"></a>The data types of the two source operands must be consistent with that of the destination operand.</p>
    </td>
    </tr>
    <tr id="row5268195011"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p12641145013"><a name="p12641145013"></a><a name="p12641145013"></a>mask</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p126171175011"><a name="p126171175011"></a><a name="p126171175011"></a>Input</p>
    </td>
    <td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11261112501"><a name="p11261112501"></a><a name="p11261112501"></a>Indicates the validity of the operation on the source operand elements. For details, see <a href="../register_data_types/MaskReg.md">3.3.4.1.2-MaskReg</a>.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Operation on MaskReg

    <a name="table17445957175011"></a>
    <table><thead align="left"><tr id="row134451857195011"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p16445135775019"><a name="p16445135775019"></a><a name="p16445135775019"></a>Parameter Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p1744511579501"><a name="p1744511579501"></a><a name="p1744511579501"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row74451857145019"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1944512571507"><a name="p1944512571507"></a><a name="p1944512571507"></a>dst</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1744565718504"><a name="p1744565718504"></a><a name="p1744565718504"></a>Destination operand.</p>
    </td>
    </tr>
    <tr id="row144451857175020"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p59747391278"><a name="p59747391278"></a><a name="p59747391278"></a>src0</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p53355414286"><a name="p53355414286"></a><a name="p53355414286"></a>Source operand.</p>
    </td>
    </tr>
    <tr id="row2521428183011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p58511219123111"><a name="p58511219123111"></a><a name="p58511219123111"></a>src1</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p175262810308"><a name="p175262810308"></a><a name="p175262810308"></a>Source operand.</p>
    </td>
    </tr>
    <tr id="row3926164202714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1892618426271"><a name="p1892618426271"></a><a name="p1892618426271"></a>mask</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p3926174212712"><a name="p3926174212712"></a><a name="p3926174212712"></a>Indicates which bits are valid during computation.</p>
    </td>
    </tr>
    </tbody>
    </table>

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Calling Example<a name="section642mcpsimp"></a>

-   Operation on RegTensor

    ```
    template <typename T>
    __simd_vf__ inline void AndVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);        
            AscendC::Reg::And(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

-   Operation on MaskReg

    ```
    template <typename T>
    __simd_vf__ inline void AndVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
        AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::MaskReg dst;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::And(dst, src0, src1, mask);
            AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
        }
    }
    ```

