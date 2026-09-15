# InitBufPool<a name="ZH-CN_TOPIC_0000001912722601"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:34:52.507Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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

Initializes the **TBufPool** memory resource pool. This API applies to scenarios where memory resources are limited and you want to manually specify UB/L1 memory resource reuse. After initialization, this API divides a sub-resource pool from the overall memory resources. The divided sub-resource pool **TBufPool** provides the following methods for resource management:

-   The overloaded **TPipe::InitBufPool** API specifies reuse with other **TBufPool** sub-resource pools.
-   The **TBufPool::[InitBufPool](../TBufPool/InitBufPool-55.md)** API further divides the sub-resource pool.
-   The **TBufPool::[InitBuffer](../TBufPool/InitBuffer-56.md)** API allocates buffers.

For details about **TBufPool** and the resource division diagram, see [TBufPool](../TBufPool/TBufPool.md).

## Prototype<a name="section620mcpsimp"></a>

```
template <class T>
__aicore__ inline bool InitBufPool(T& bufPool, uint32_t len)
template <class T, class U>
__aicore__ inline bool InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="18.47%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="81.53%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="18.47%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="81.53%" headers="mcps1.2.3.1.2 "><p id="p1334183893115"><a name="p1334183893115"></a><a name="p1334183893115"></a>Type of **bufPool**.</p></td>
</tr>
<tr id="row18835145716587"><td class="cellrowborder" valign="top" width="18.47%" headers="mcps1.2.3.1.1 "><p id="p2023982513308"><a name="p2023982513308"></a><a name="p2023982513308"></a>**U**</p></td>
<td class="cellrowborder" valign="top" width="81.53%" headers="mcps1.2.3.1.2 "><p id="p1386393113012"><a name="p1386393113012"></a><a name="p1386393113012"></a>Type of **shareBuf**.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.370000000000001%" id="mcps1.2.4.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.27000000000001%" id="mcps1.2.4.1.3"><p id="p17377162715303"><a name="p17377162715303"></a><a name="p17377162715303"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row19377627133012"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p55733283248"><a name="p55733283248"></a><a name="p55733283248"></a>**bufPool**</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p1357372832410"><a name="p1357372832410"></a><a name="p1357372832410"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p1457322822419"><a name="p1457322822419"></a><a name="p1457322822419"></a>Newly allocated resource pool, of the type **TBufPool**.</p></td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1357312818241"><a name="p1357312818241"></a><a name="p1357312818241"></a>**len**</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p857319288247"><a name="p857319288247"></a><a name="p857319288247"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p17573122810249"><a name="p17573122810249"></a><a name="p17573122810249"></a>Length of the newly allocated resource pool, in bytes. If it is not 32-byte aligned, it is automatically padded to 32-byte alignment.</p></td>
</tr>
<tr id="row1371133216245"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1417710361241"><a name="p1417710361241"></a><a name="p1417710361241"></a>shareBuf</p></td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p15177113615246"><a name="p15177113615246"></a><a name="p15177113615246"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p11771236152417"><a name="p11771236152417"></a><a name="p11771236152417"></a>Reused resource pool of the **TBufPool** type. The newly divided resource pool shares the start address and length with the reused resource pool.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   The hardware attributes of the newly divided resource pool must be consistent with those of the reused resource pool, and the two share the same start address and length.
-   The input length must be less than or equal to the length of the reused resource pool.
-   For other general constraints, see [TBufPool](../TBufPool/TBufPool.md).

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

Because physical memory is limited, memory reuse can be specified to resolve resource insufficiency in scenarios where there is no data dependency during computation or where data dependencies are serial. For a complete operator sample, see [tbufpool\_management sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/04_memory_management/tbufpool_management).

```
// Declare a pointer to a TPipe object.
AscendC::TPipe* pipe;
// Define two sub-resource pool objects, tbufPool1 and tbufPool2.
AscendC::TBufPool<AscendC::TPosition::VECCALC> tbufPool1, tbufPool2;
// Initialize the first sub-resource pool, tbufPool1.
pipe->InitBufPool(tbufPool1, bufSize * 3);
// Initialize the second sub-resource pool, tbufPool2, and specify that tbufPool2 reuses the start address and length of tbufPool1.
pipe->InitBufPool(tbufPool2, bufSize * 3, tbufPool1);

// The computation is serial with no data overwriting, implementing memory reuse and automatic synchronization.
tbufPool1.InitBuffer(srcQue0, 1, bufSize);
tbufPool1.InitBuffer(srcQue1, 1, bufSize);
tbufPool1.InitBuffer(dstQue0, 1, bufSize);
CopyIn();
Compute(); 
CopyOut();
tbufPool1.Reset();
tbufPool2.InitBuffer(srcQue2, 1, bufSize);
tbufPool2.InitBuffer(srcQue3, 1, bufSize);
tbufPool2.InitBuffer(dstQue1, 1, bufSize);
CopyIn1();
Compute1();
CopyOut1();
tbufPool2.Reset();
```

