# InitBufPool<a name="ZH-CN_TOPIC_0000001912802253"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:08:44.904Z pushedAt=2026-11-18T01:25:31.012Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/TBufPool/InitBufPool_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_tpipe.h"`.

The Tpipe::InitBufPool API partitions an entire block of resources. The entire TbufPool resource can then be further partitioned into smaller blocks through the TBufPool::InitBufPool API.

Non-shared mode and shared mode of resource allocation:
- Non-shared mode: A new resource pool is partitioned from Tpipe.
- Shared mode: The new resource pool partitioned from Tpipe shares the start address and length with other resource pools.

## Prototype<a name="section620mcpsimp"></a>

-   Non-shared mode

    ```cpp
    template <class T>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len)
    ```

-   Shared mode

    ```cpp
    template <class T, class U>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table1550165916920"></a>
<table><thead align="left"><tr id="row115015591391"><th class="cellrowborder" valign="top" width="12.139999999999999%" id="mcps1.2.3.1.1"><p id="p12501159099"><a name="p12501159099"></a><a name="p12501159099"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="87.86%" id="mcps1.2.3.1.2"><p id="p85019592918"><a name="p85019592918"></a><a name="p85019592918"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row1550117591914"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p185019592913"><a name="p185019592913"></a><a name="p185019592913"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12101541625"><a name="p12101541625"></a><a name="p12101541625"></a>Type of the bufPool<span> parameter.</span></p>
</td>
</tr>
<tr id="row758993611212"><td class="cellrowborder" valign="top" width="12.139999999999999%" headers="mcps1.2.3.1.1 "><p id="p115897367213"><a name="p115897367213"></a><a name="p115897367213"></a>U</p>
</td>
<td class="cellrowborder" valign="top" width="87.86%" headers="mcps1.2.3.1.2 "><p id="p12589153615212"><a name="p12589153615212"></a><a name="p12589153615212"></a>Type of the shareBuf parameter.</p>
</td>
</tr>
</tbody>
</table>

**Table 2**  Parameters of the API in non-shared mode

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>bufPool</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p164211556249"><a name="p164211556249"></a><a name="p164211556249"></a>Newly partitioned resource pool of type TBufPool.</p>
</td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1042110572411"><a name="p1042110572411"></a><a name="p1042110572411"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1342115582416"><a name="p1342115582416"></a><a name="p1342115582416"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p154210516247"><a name="p154210516247"></a><a name="p154210516247"></a>Length of the newly partitioned resource pool, in bytes. If it is not 32-byte aligned, it is automatically rounded up to 32-byte alignment.</p>
</td>
</tr>
</tbody>
</table>

**Table 3**  Parameters of the shared mode API

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="12.370000000000001%" id="mcps1.2.4.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="75.27000000000001%" id="mcps1.2.4.1.3"><p id="p17377162715303"><a name="p17377162715303"></a><a name="p17377162715303"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row19377627133012"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p55733283248"><a name="p55733283248"></a><a name="p55733283248"></a>bufPool</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p1357372832410"><a name="p1357372832410"></a><a name="p1357372832410"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p1457322822419"><a name="p1457322822419"></a><a name="p1457322822419"></a>Newly partitioned resource pool of type TBufPool.</p>
</td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1357312818241"><a name="p1357312818241"></a><a name="p1357312818241"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p857319288247"><a name="p857319288247"></a><a name="p857319288247"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p17573122810249"><a name="p17573122810249"></a><a name="p17573122810249"></a>Length of the newly partitioned resource pool, in bytes. If not 32-byte aligned, it is automatically rounded up to 32-byte alignment.</p>
</td>
</tr>
<tr id="row1371133216245"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1417710361241"><a name="p1417710361241"></a><a name="p1417710361241"></a>shareBuf</p>
</td>
<td class="cellrowborder" valign="top" width="12.370000000000001%" headers="mcps1.2.4.1.2 "><p id="p15177113615246"><a name="p15177113615246"></a><a name="p15177113615246"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="75.27000000000001%" headers="mcps1.2.4.1.3 "><p id="p11771236152417"><a name="p11771236152417"></a><a name="p11771236152417"></a>Reused resource pool of type TBufPool. The newly partitioned resource pool and the reused resource pool share the start address and length.</p>
</td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

- The physical memory of the newly divided resource pool and the reused resource pool must be consistent, and the two share the start address and length.
- The input length must be less than or equal to the length of the reused resource pool.
- For other general constraints, see [TBufPool](TBufPool.md).

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

When the data volume is large and memory is limited, all data cannot be moved at once. The computation must be split into multiple stages, each using a portion of the data. Memory address reuse can be achieved through the TBufPool resource pool. In this example, the resource pool tbufPool0 is divided from Tpipe. After tbufPool0 allocates space for src0Gm, the resource pool tbufPool1 is further allocated, and tbufPool1 is specified to be reused with tbufPool2 for the first and second rounds of computation respectively. At this point, tbufPool1 and tbufPool2 share the start address and length.

For the complete sample, see [tbufpool\_management sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/04_memory_management/tbufpool_management).

```cpp
AscendC::TPipe* pipe;
AscendC::TBufPool<AscendC::TPosition::VECCALC> tbufPool0, tbufPool1, tbufPool2;
// srcQue0, srcQue1, and srcQue2 are TQue on VECIN, and dstQue0 and dstQue1 are TQue on VECOUT.
// Divide the resource pool tbufPool0 from Tpipe.
pipe->InitBufPool(tbufPool0, bufSize * 4);
// Allocate space for tbufPool1 through tbufPool0, and specify that tbufPool1 and tbufPool2 reuse the same space.
tbufPool0.InitBufPool(tbufPool1, bufSize * 2);
// Allocate space for srcQue0.
tbufPool0.InitBuffer(srcQue0, 1, bufSize * 2); // Total src0
tbufPool0.InitBufPool(tbufPool2, bufSize * 2, tbufPool1);
// Allocate space for srcQue1 and dstQue0 through tbufPool1.
tbufPool1.InitBuffer(srcQue1, 1, bufSize);
tbufPool1.InitBuffer(dstQue0, 1, bufSize);
CopyIn();
Compute();
CopyOut();
// Switch the resource pool to tbufPool2, and allocate space for srcQue2 and dstQue1 through tbufPool2.
tbufPool1.Reset();
tbufPool2.InitBuffer(srcQue2, 1, bufSize);
tbufPool2.InitBuffer(dstQue1, 1, bufSize);
CopyIn1();
Compute1();
CopyOut1();
tbufPool2.Reset();
tbufPool0.Reset();
pipe->Reset();
```
