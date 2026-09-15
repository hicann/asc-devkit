# InitBuffer<a name="ZH-CN_TOPIC_0000001375937722"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:35:44.212Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Supported
<!-- end id8 -->


## Description<a name="section618mcpsimp"></a>

Used to allocate memory for queues such as **TQue** and for **TBuf**.

## Prototype<a name="section620mcpsimp"></a>

-   Allocate memory for queues such as **TQue**.

    ```
    template <class T>
    __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len)
    
    // Allocate memory for queues such as TQue. Developers can customize the address information, which includes the start address and length.
    template <class T, class U, class V, class... Addrs>
    __aicore__ inline bool InitBuffer(T& que, const Std::tuple<U, V>& addr0, const Addrs&... addrs)
    ```

-   Allocate memory for **TBuf**.

    ```
    template <TPosition bufPos>
    __aicore__ inline bool InitBuffer(TBuf<bufPos>& buf, uint32_t len)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  bool InitBuffer\(T& que, uint8\_t num, uint32\_t len\) Prototype Definition Template Parameter Description

<a name="table634418773417"></a>
<table><thead align="left"><tr id="row1934537133415"><th class="cellrowborder" valign="top" width="12.13%" id="mcps1.2.3.1.1"><p id="p1934597103419"><a name="p1934597103419"></a><a name="p1934597103419"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.87%" id="mcps1.2.3.1.2"><p id="p1034519793416"><a name="p1034519793416"></a><a name="p1034519793416"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row14345472346"><td class="cellrowborder" valign="top" width="12.13%" headers="mcps1.2.3.1.1 "><p id="p149121233183416"><a name="p149121233183416"></a><a name="p149121233183416"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="87.87%" headers="mcps1.2.3.1.2 "><p id="p1034577203412"><a name="p1034577203412"></a><a name="p1034577203412"></a>Queue type. Supported values include <a href="../TQue/TQue.md">TQue</a>, <a href="../TQueBind/TQueBind.md">TQueBind</a>, and <a href="../TSCM/TSCM.md">TSCM</a>.</p></td>
</tr>
</tbody>
</table>

**Table 2**  bool InitBuffer\(T& que, uint8\_t num, uint32\_t len\) Prototype Definition Parameter Description

<a name="table193329316393"></a>
<table><thead align="left"><tr id="row123331131153919"><th class="cellrowborder" valign="top" width="11.940000000000001%" id="mcps1.2.4.1.1"><p id="p8333133153913"><a name="p8333133153913"></a><a name="p8333133153913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.8%" id="mcps1.2.4.1.2"><p id="p518118718459"><a name="p518118718459"></a><a name="p518118718459"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p833353113393"><a name="p833353113393"></a><a name="p833353113393"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row11660173845017"><td class="cellrowborder" valign="top" width="11.940000000000001%" headers="mcps1.2.4.1.1 "><p id="p466053810507"><a name="p466053810507"></a><a name="p466053810507"></a>**que**</p></td>
<td class="cellrowborder" valign="top" width="12.8%" headers="mcps1.2.4.1.2 "><p id="p885774605014"><a name="p885774605014"></a><a name="p885774605014"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p0660153818501"><a name="p0660153818501"></a><a name="p0660153818501"></a>**TQue** and other objects that need to allocate memory.</p></td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="11.940000000000001%" headers="mcps1.2.4.1.1 "><p id="p11399116193313"><a name="p11399116193313"></a><a name="p11399116193313"></a>**num**</p></td>
<td class="cellrowborder" valign="top" width="12.8%" headers="mcps1.2.4.1.2 "><p id="p111819774511"><a name="p111819774511"></a><a name="p111819774511"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p6383173514333"><a name="p6383173514333"></a><a name="p6383173514333"></a>Number of memory blocks to allocate. The double buffer feature is enabled through this parameter: when **num** is set to 1, double buffer is disabled; when **num** is set to 2, double buffer is enabled.</p></td>
</tr>
<tr id="row1430772593316"><td class="cellrowborder" valign="top" width="11.940000000000001%" headers="mcps1.2.4.1.1 "><p id="p530752514330"><a name="p530752514330"></a><a name="p530752514330"></a>**len**</p></td>
<td class="cellrowborder" valign="top" width="12.8%" headers="mcps1.2.4.1.2 "><p id="p530711252335"><a name="p530711252335"></a><a name="p530711252335"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p183081251336"><a name="p183081251336"></a><a name="p183081251336"></a>Size of each memory block, in bytes. When the passed **len** does not meet 32-byte alignment, the API automatically rounds it up to 32-byte alignment internally, and subsequent data movement involves unaligned processing.</p></td>
</tr>
</tbody>
</table>

**Table 3** Template parameter description for the bool InitBuffer(T& que, const Std::tuple<U, V>& addr0, const Addrs&... addrs) prototype definition.

<a name="table115271653105120"></a>
<table><thead align="left"><tr id="row8527185375112"><th class="cellrowborder" valign="top" width="12.13%" id="mcps1.2.3.1.1"><p id="p10527155312514"><a name="p10527155312514"></a><a name="p10527155312514"></a>Parameter Name</p></th>
<th class="cellrowborder" valign="top" width="87.87%" id="mcps1.2.3.1.2"><p id="p1652713531510"><a name="p1652713531510"></a><a name="p1652713531510"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row20527115315514"><td class="cellrowborder" valign="top" width="12.13%" headers="mcps1.2.3.1.1 "><p id="p85273533517"><a name="p85273533517"></a><a name="p85273533517"></a>T</p></td>
<td class="cellrowborder" valign="top" width="87.87%" headers="mcps1.2.3.1.2 "><p id="p352719537516"><a name="p352719537516"></a><a name="p352719537516"></a>Type of the queue. Supported values include <a href="../TQue/TQue.md">TQue</a> and <a href="../TQueBind/TQueBind.md">TQueBind</a>.</p></td>
</tr>
<tr id="row1865712321528"><td class="cellrowborder" valign="top" width="12.13%" headers="mcps1.2.3.1.1 "><p id="p15657123285218"><a name="p15657123285218"></a><a name="p15657123285218"></a>U</p></td>
<td class="cellrowborder" valign="top" width="87.87%" headers="mcps1.2.3.1.2 "><p id="p46571732125213"><a name="p46571732125213"></a><a name="p46571732125213"></a>Type of the start address. The type is an integer.</p></td>
</tr>
<tr id="row1120812368527"><td class="cellrowborder" valign="top" width="12.13%" headers="mcps1.2.3.1.1 "><p id="p1520813695218"><a name="p1520813695218"></a><a name="p1520813695218"></a>V</p></td>
<td class="cellrowborder" valign="top" width="87.87%" headers="mcps1.2.3.1.2 "><p id="p1220853665211"><a name="p1220853665211"></a><a name="p1220853665211"></a>Type of the length. The type is an integer.</p></td>
</tr>
<tr id="row536893945217"><td class="cellrowborder" valign="top" width="12.13%" headers="mcps1.2.3.1.1 "><p id="p16368163965216"><a name="p16368163965216"></a><a name="p16368163965216"></a>Addrs...</p></td>
<td class="cellrowborder" valign="top" width="87.87%" headers="mcps1.2.3.1.2 "><p id="p93681939185212"><a name="p93681939185212"></a><a name="p93681939185212"></a>Address information in tuple form, including the start address and length.</p></td>
</tr>
</tbody>
</table>

**Table 4**  Parameter Description of the bool InitBuffer\(T& que, const Std::tuple<U, V\>& addr0, const Addrs&... addrs\) Prototype

<a name="table2527125319516"></a>
<table><thead align="left"><tr id="row10527553125118"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p95271153205115"><a name="p95271153205115"></a><a name="p95271153205115"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p3527553105113"><a name="p3527553105113"></a><a name="p3527553105113"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p19527205319514"><a name="p19527205319514"></a><a name="p19527205319514"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row1752710537516"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p352720533513"><a name="p352720533513"></a><a name="p352720533513"></a>que</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p16527115316513"><a name="p16527115316513"></a><a name="p16527115316513"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p38012172352"><a name="p38012172352"></a><a name="p38012172352"></a>**TQue** and other objects that need to allocate memory.</p></td>
</tr>
<tr id="row152715534516"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p195271553115111"><a name="p195271553115111"></a><a name="p195271553115111"></a>**addr0**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1527145365116"><a name="p1527145365116"></a><a name="p1527145365116"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p2527115313519"><a name="p2527115313519"></a><a name="p2527115313519"></a>Address information of the memory block to be allocated, structured as the start address and length.</p></td>
</tr>
<tr id="row9528353145116"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p11528753195110"><a name="p11528753195110"></a><a name="p11528753195110"></a>**addrs**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8528135312519"><a name="p8528135312519"></a><a name="p8528135312519"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p17528185311518"><a name="p17528185311518"></a><a name="p17528185311518"></a>List of address information in tuple format. The number of elements in the tuple must be 2, namely the start address and length.</p></td>
</tr>
</tbody>
</table>

**Table 5**  Prototype Definition Template Parameter Description of InitBuffer\(TBuf<bufPos\>& buf, uint32\_t len\)

<a name="table873615294112"></a>
<table><thead align="left"><tr id="row473742114115"><th class="cellrowborder" valign="top" width="12.34%" id="mcps1.2.3.1.1"><p id="p1073720204115"><a name="p1073720204115"></a><a name="p1073720204115"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.66000000000001%" id="mcps1.2.3.1.2"><p id="p37371826412"><a name="p37371826412"></a><a name="p37371826412"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row167376215416"><td class="cellrowborder" valign="top" width="12.34%" headers="mcps1.2.3.1.1 "><p id="p14568152324113"><a name="p14568152324113"></a><a name="p14568152324113"></a>bufPos</p></td>
<td class="cellrowborder" valign="top" width="87.66000000000001%" headers="mcps1.2.3.1.2 "><p id="p188271137124113"><a name="p188271137124113"></a><a name="p188271137124113"></a>Logical position of **TBuf**, of the <a href="../../aux_data_structures/TPosition.md">TPosition</a> type.</p></td>
</tr>
</tbody>
</table>

**Table 6** Parameter description of the InitBuffer\(TBuf<bufPos\>& buf, uint32\_t len\) prototype definition

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.370000000000001%" id="mcps1.2.4.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.27000000000001%" id="mcps1.2.4.1.3"><p id="p17377162715303"><a name="p17377162715303"></a><a name="p17377162715303"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row19377627133012"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p737710279307"><a name="p737710279307"></a><a name="p737710279307"></a>buf</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p13377122733010"><a name="p13377122733010"></a><a name="p13377122733010"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p19377102793016"><a name="p19377102793016"></a><a name="p19377102793016"></a>**TBuf** object that needs to allocate memory.</p></td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p5377527113018"><a name="p5377527113018"></a><a name="p5377527113018"></a>**len**</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p12377122712304"><a name="p12377122712304"></a><a name="p12377122712304"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p6514716314"><a name="p6514716314"></a><a name="p6514716314"></a>Size of the memory allocated for **TBuf**, in bytes. If the passed **len** does not satisfy 32-byte alignment, the API automatically rounds it up to 32-byte alignment internally, and subsequent data transfer involves unaligned processing.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   The memory allocated by **InitBuffer** is automatically released by the destructor when the **TPipe** object is destroyed, and no manual release is required.
-   To reallocate the memory allocated by **InitBuffer**, call [Reset](Reset.md) first and then call the **InitBuffer** API.
-   The total number of buffers used in a kernel cannot exceed 64.
-   Mixing the custom-address **InitBuffer** allocation method with the method that does not specify an address is not recommended, as it may cause memory conflicts.
<!-- npu="950" id1 -->
- For Ascend 950PR/Ascend 950DT, UB memory allocation is performed on top of static memory. That is, the initial position of dynamic memory is based on static memory. For details, see [memory hierarchy](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/memory_hierarchy.md).
<!-- end id1 -->
- Mixing with static arrays such as L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, and BiasTable is not supported currently.

## Return Value<a name="section640mcpsimp"></a>

Returns the result of **Buffer** initialization.

## Example<a name="section642mcpsimp"></a>

```
// Allocate memory for TQue, with 2 memory blocks of 128 bytes each.
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 2> que; // Output data queue management object, with TPosition set to VECOUT.
uint8_t num = 2;
uint32_t len = 128;
pipe.InitBuffer(que, num, len);

// Allocate memory for TQue. The developer customizes the memory address information as [0, 1024], [2048, 4096], and [8192, 12288].
AscendC::TPipe pipe; // Pipe memory management object.
AscendC::TQue<AscendC::TPosition::VECOUT, 1> que; // Output data queue management object, with TPosition set to VECOUT.
auto addr0 = Std::make_tuple(0, 1024);
auto addr1 = Std::make_tuple(2048, 2048);
auto addr2 = Std::make_tuple(8192, 4096);
pipe.InitBuffer(que, addr0, addr1, addr2);

// Allocate memory for TBuf, with a length of 128 bytes.
AscendC::TPipe pipe;
AscendC::TBuf<AscendC::TPosition::A1> buf; // Output data management object, with TPosition set to A1.
uint32_t len = 128;
pipe.InitBuffer(buf, len);
```
