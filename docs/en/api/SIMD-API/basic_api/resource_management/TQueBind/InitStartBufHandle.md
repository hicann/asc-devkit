# InitStartBufHandle<a name="ZH-CN_TOPIC_0000002053982944"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:54:13.857Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Sets the start memory block pointer, the number of memory blocks, and the size of each memory block for **TQue**/**TBuf**.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline void InitStartBufHandle(TBufHandle startBufhandle, uint8_t num, uint32_t len)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>startBufhandle</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p165371945142512"><a name="p165371945142512"></a><a name="p165371945142512"></a>Starting memory block pointer of TQue/TBuf, with the data type TBufHandle (actually uint8_t*).</p></td>
</tr>
<tr id="row385524382216"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p485616439222"><a name="p485616439222"></a><a name="p485616439222"></a>num</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1285694392215"><a name="p1285694392215"></a><a name="p1285694392215"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p6856143142219"><a name="p6856143142219"></a><a name="p6856143142219"></a>Number of memory blocks to allocate.</p></td>
</tr>
<tr id="row1283033172318"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p828433352317"><a name="p828433352317"></a><a name="p828433352317"></a>len</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p928433315235"><a name="p928433315235"></a><a name="p928433315235"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p1828443313236"><a name="p1828443313236"></a><a name="p1828443313236"></a>Size of each memory block, in bytes.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   **TQue** and **TBuf** inherit from the **TQueBind** class, so **TQue** and **TBuf** objects can also use this API.

-   This API is currently provided only for [custom TBufPool](../custom_TBufPool/custom TBufPool.md) to initialize the memory blocks of **TQue** and **TBuf**.
-   When this API is called by a **TBuf** object, the input parameter **num** must be 1.

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

For the complete example, see [Example](../custom_TBufPool/extern_impl_bufpool_macro.md#section1234017553610).

```
// Assume the custom tbufpool class is MyBufPool.
// InitBuffer function for TQue initialization inside the custom tbufpool class:
template<class T> 
__aicore__ inline bool MyBufPool::InitBuffer(T& que, uint8_t num, uint32_t len)
{
   ...
   // Initialize the memory blocks of TQue.
   uint32_t curPoolAddr  = 0;  // Start address of the memory block.
   auto bufhandle = xxx; // Specific memory block. This variable can be obtained from the custom tbufpool.
   srcQue0.InitStartBufHandle(bufhandle, num, len);
   for (uint8_t i = 0; i < num; i++) {
      que.InitBufHandle(this, i, bufhandle , curPoolAddr + i * len, len);
   }
   ...
}

// InitBuffer function for TBuf initialization inside the custom tbufpool class:
template<class T> 
__aicore__ inline bool MyBufPool::InitBuffer(AscendC::TBuf<bufPos>& buf, uint32_t len)
{
   ...
   // Initialize the memory blocks of TBuf.
   uint32_t curPoolAddr  = 0;  // Start address of the memory block.
   auto bufhandle = xxx; // A specific memory block. This variable can be obtained from a custom tbufpool.
   srcBuf1.InitStartBufHandle(bufhandle, 1, len);
   srcBuf1.InitBufHandle(this, 0, bufhandle , curPoolAddr, len);
   ...
}
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECIN, 1> srcQue0;
AscendC::TBuf<AscendC::TPosition::VECIN> srcBuf1;
MyBufPool tbufPool;
pipe.InitBufPool(tbufPool, 1024 * 2);
tbufPool.InitBuffer(srcQue0, 1, 1024);
tbufPool.InitBuffer(srcBuf1, 1024);
```

