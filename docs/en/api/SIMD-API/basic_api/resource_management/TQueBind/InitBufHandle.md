# InitBufHandle<a name="ZH-CN_TOPIC_0000002087866029"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:52:46.404Z -->

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
- Atlas 200I/500 A2 inference products: Supported
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

Performs memory allocation for the memory blocks of **TQue** and **TBuf** objects, including setting the memory block size and the address to which it points.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void InitBufHandle(T* bufPool, uint32_t index, TBufHandle bufhandle, uint32_t curPoolAddr, uint32_t len)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>T</p></td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a><span>Data type of bufPool.</span></p></td>
</tr>
</tbody>
</table>

**Table 2**  Parameter description

<a name="table181221135162517"></a>
<table><thead align="left"><tr id="row151221135112520"><th class="cellrowborder" valign="top" width="12.471247124712471%" id="mcps1.2.4.1.1"><p id="p1353754532512"><a name="p1353754532512"></a><a name="p1353754532512"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1253774516259"><a name="p1253774516259"></a><a name="p1253774516259"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="74.93749374937492%" id="mcps1.2.4.1.3"><p id="p1653710452259"><a name="p1653710452259"></a><a name="p1653710452259"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row12122235102511"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1537164502512"><a name="p1537164502512"></a><a name="p1537164502512"></a>bufPool</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1653714592515"><a name="p1653714592515"></a><a name="p1653714592515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p182176191392"><a name="p182176191392"></a><a name="p182176191392"></a>User-defined TBufPool object.</p></td>
</tr>
<tr id="row385524382216"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p485616439222"><a name="p485616439222"></a><a name="p485616439222"></a>index</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p1285694392215"><a name="p1285694392215"></a><a name="p1285694392215"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p6856143142219"><a name="p6856143142219"></a><a name="p6856143142219"></a>Offset subscript value of the memory block to be set. The first block is 0, the second block is 1, and so on.</p></td>
</tr>
<tr id="row1283033172318"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p828433352317"><a name="p828433352317"></a><a name="p828433352317"></a>bufhandle</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p928433315235"><a name="p928433315235"></a><a name="p928433315235"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p1828443313236"><a name="p1828443313236"></a><a name="p1828443313236"></a>Pointer to the memory block to be set, of the type **TBufHandle** (actually **uint8_t***).</p></td>
</tr>
<tr id="row942716115246"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p34281216244"><a name="p34281216244"></a><a name="p34281216244"></a>**curPoolAddr**</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p144283119247"><a name="p144283119247"></a><a name="p144283119247"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p2428171142416"><a name="p2428171142416"></a><a name="p2428171142416"></a>Address of the memory block to be set.</p></td>
</tr>
<tr id="row16611172462412"><td class="cellrowborder" valign="top" width="12.471247124712471%" headers="mcps1.2.4.1.1 "><p id="p1861242432412"><a name="p1861242432412"></a><a name="p1861242432412"></a>**len**</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p196121224152414"><a name="p196121224152414"></a><a name="p196121224152414"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="74.93749374937492%" headers="mcps1.2.4.1.3 "><p id="p156121124132420"><a name="p156121124132420"></a><a name="p156121124132420"></a>Size of the memory block to be set, in bytes.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   **TQue** and **TBuf** classes inherit from the **TQueBind** class; therefore, **TQue** and **TBuf** objects can also use this API.
-   Currently, this API is only provided for initializing the memory blocks of **TQue** and **TBuf** in [custom TBufPool](../custom_TBufPool/custom_tbufpool.md).

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

For the complete example, see [Example](../custom_TBufPool/extern_impl_bufpool_macro.md#section1234017553610).

```
// Assume the custom tbufpool class is MyBufPool.
// InitBuffer function for initializing TQue inside the custom tbufpool class:
template<class T> 
__aicore__ inline bool MyBufPool::InitBuffer(T& que, uint8_t num, uint32_t len)
{
   ...
   // Initialize the memory block of TQue.
   uint32_t curPoolAddr  = 0;  // Start address of the memory block.
   auto bufhandle = xxx; // Specific memory block. This variable can be obtained from the custom tbufpool.
   srcQue0.InitStartBufHandle(bufhandle , num, len);
   for (uint8_t i = 0; i < num; i++) {
      que.InitBufHandle(this, i, bufhandle , curPoolAddr + i * len, len);
   }
   ...
}

// InitBuffer function for initializing TBuf inside the custom tbufpool class:
template<class T> 
__aicore__ inline bool MyBufPool::InitBuffer(TBuf<bufPos>& buf, uint32_t len)
{
   ...
   // Initialize the memory block of TBuf.
   uint32_t curPoolAddr  = 0;  // Start address of the memory block.
   auto bufhandle = xxx; // The specific memory block. This variable can be obtained from the custom tbufpool.
   srcBuf1.InitStartBufHandle(bufhandle, 1, len);
   srcBuf1.InitBufHandle(this, 0, bufhandle , curPoolAddr, len);
   ...
}
AscendC::TPipe pipe;
AscendC::TQue<TPosition::VECIN, 1> srcQue0;
AscendC::TBuf<TPosition::VECIN> srcBuf1;
MyBufPool tbufPool;
pipe.InitBufPool(tbufPool, 1024 * 2);
tbufPool.InitBuffer(srcQue0, 1, 1024);
tbufPool.InitBuffer(srcBuf1, 1024);
```

