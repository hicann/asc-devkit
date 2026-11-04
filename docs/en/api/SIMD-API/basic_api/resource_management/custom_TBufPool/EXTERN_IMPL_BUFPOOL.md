# EXTERN\_IMPL\_BUFPOOL Macro<a name="ZH-CN_TOPIC_0000002087922321"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:22:30.160Z pushedAt=2026-10-26T07:41:44.632Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/custom_TBufPool/EXTERN_IMPL_BUFPOOL_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_tpipe.h"`.

Developers can use the [TBufPool class](../TBufPool/TBufPool.md) to manually manage Unified Buffer (UB) and L1 Buffer physical memory.

The memory blocks divided by the TBufPool class are all contiguous. Developers may have some custom memory block allocation requirements, such as non-contiguous memory blocks and memory blocks shared across different TQue instances. In these cases, developers need to implement a custom TBufPool.

To simplify the custom implementation for developers, the EXTERN\_IMPL\_BUFPOOL macro is provided to help users customize TBufPool. When using the custom TBufPool feature, note the following:

-   Before customizing TBufPool, you must initialize the TBufPool memory resource pool through the [TPipe::InitBufPool](../TPipe/InitBufPool.md) API.
-   For a custom TBufPool, developers need to implement the allocation, initialization, and release of TQue/TBuf memory blocks on their own.

For the Reset, Init, GetBufHandle, SetCurAddr, GetCurAddr, SetCurBufSize, and GetCurBufSize APIs defined inside the EXTERN\_IMPL\_BUFPOOL macro, see the descriptions in the subsequent sections. After using this macro, you can use the preceding APIs to implement the custom TBufPool feature.

> [!NOTE] Note 
> The custom TBufPool-related APIs are experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers should pay attention to updates in later versions.

## Prototype<a name="section620mcpsimp"></a>

```cpp
// Omit the specific content of the macro definition.
#define EXTERN_IMPL_BUFPOOL(EXT_BUFPOOL, POSITION, BUFID_SIZE) ...
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  EXTERN\_IMPL\_BUFPOOL Parameter description of the macro prototype definition

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="7.75%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="79.89%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>EXT_BUFPOOL</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p171754287124"><a name="p171754287124"></a><a name="p171754287124"></a>Custom TBufPool class name.</p></td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1042110572411"><a name="p1042110572411"></a><a name="p1042110572411"></a>POSITION</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p1342115582416"><a name="p1342115582416"></a><a name="p1342115582416"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p154210516247"><a name="p154210516247"></a><a name="p154210516247"></a>Custom TBufPool logical position, which can be <span>VECIN, VECOUT, </span>VECCALC, A1<span>, </span>B1, or C1. <span>For details about TPosition, see </span><a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
<tr id="row1460143271116"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p2601143215118"><a name="p2601143215118"></a><a name="p2601143215118"></a>BUFID_SIZE</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p17601123261115"><a name="p17601123261115"></a><a name="p17601123261115"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p0601123201116"><a name="p0601123201116"></a><a name="p0601123201116"></a>Number of buffer blocks allocated by the custom TBufPool. The recommended value does not exceed 16.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section1234017553610"></a>

For a detailed example, see [tbufpool\_management sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/04_memory_management/tbufpool_management).

In the following example, 65536 \* 3 of memory is allocated for **tbufPool0**, and then the **InitBuffer** function of the custom **MyBufPool** is implemented to allocate memory for **TQue** and **Tbuf**.

```cpp
#include "kernel_operator.h"

class MyBufPool {
public:
    __aicore__ inline MyBufPool() {
        Init();
    }

    template<class T> 
    __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len) {
    }

    template<AscendC::TPosition bufPos>
    __aicore__ inline bool InitBuffer(AscendC::TBuf<bufPos>& buf, uint32_t len) {
    }
    
    // MyBufPool, the custom TBufPool class name. Select VECCALC as the logical position for the custom TBufPool.
    // The number of Buffer blocks allocated by the custom TBufPool is 16.
    EXTERN_IMPL_BUFPOOL(MyBufPool, AscendC::TPosition::VECCALC, 16);
};
```
