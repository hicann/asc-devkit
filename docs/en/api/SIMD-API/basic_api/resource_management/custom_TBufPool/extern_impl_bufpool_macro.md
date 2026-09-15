# EXTERN\_IMPL\_BUFPOOL Macro<a name="ZH-CN_TOPIC_0000002087922321"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:18:15.617Z -->

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

Developers can use the [TBufPool class](../TBufPool/TBufPool.md) to manually manage the physical memory of Unified Buffer and L1 Buffer.

The memory blocks divided by the TBufPool class are all contiguous. Developers may have some custom memory block allocation requirements, such as non-contiguous memory blocks or memory blocks shared among different TQues. In such cases, developers need to implement a custom TBufPool.

To simplify the custom implementation for developers, the **EXTERN_IMPL_BUFPOOL** macro is provided to assist users in customizing TBufPool. When using the custom TBufPool feature, note the following:

-   Before customizing TBufPool, you must initialize the TBufPool memory resource pool through the [TPipe::InitBufPool](../TPipe/InitBufPool.md) API.
-   For a custom TBufPool, developers need to implement the allocation, initialization, and release of TQue/TBuf memory blocks on their own.

For the **Reset**, **Init**, **GetBufHandle**, **SetCurAddr**, **GetCurAddr**, **SetCurBufSize**, and **GetCurBufSize** APIs defined inside the **EXTERN_IMPL_BUFPOOL** macro, see the descriptions in the subsequent sections. After using this macro, you can use the preceding APIs to implement the custom TBufPool feature.

> [!NOTE]Note
> The custom TBufPool-related APIs are experimental APIs and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers should pay attention to updates in later versions.

## Prototype<a name="section620mcpsimp"></a>

```
// The specific content of the macro definition is omitted.
#define EXTERN_IMPL_BUFPOOL(EXT_BUFPOOL, POSITION, BUFID_SIZE) ...
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameter description of the EXTERN\_IMPL\_BUFPOOL macro prototype definition

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="7.75%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="79.89%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>EXT_BUFPOOL</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p171754287124"><a name="p171754287124"></a><a name="p171754287124"></a>Custom **TBufPool** class name.</p></td>
</tr>
<tr id="row03336319398"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1042110572411"><a name="p1042110572411"></a><a name="p1042110572411"></a>POSITION</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p1342115582416"><a name="p1342115582416"></a><a name="p1342115582416"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p154210516247"><a name="p154210516247"></a><a name="p154210516247"></a>Logical position of the custom TBufPool, which can be <span>**VECIN**, **VECOUT**, </span>**VECCALC**, **A1**<span>, </span>**B1**, **C1**.<span>For details about TPosition, see </span><a href="../../aux_data_structures/TPosition.md">TPosition</a>.</p></td>
</tr>
<tr id="row1460143271116"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p2601143215118"><a name="p2601143215118"></a><a name="p2601143215118"></a>**BUFID_SIZE**</p></td>
<td class="cellrowborder" valign="top" width="7.75%" headers="mcps1.2.4.1.2 "><p id="p17601123261115"><a name="p17601123261115"></a><a name="p17601123261115"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="79.89%" headers="mcps1.2.4.1.3 "><p id="p0601123201116"><a name="p0601123201116"></a><a name="p0601123201116"></a>Number of buffer blocks allocated by the custom **TBufPool**. It is recommended not to exceed 16.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Example<a name="section1234017553610"></a>

For a detailed example, see the [tbufpool_management sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/04_memory_management/tbufpool_management).

In the following example, memory of 65536 \* 3 is allocated for **tbufPool0**, and then the **InitBuffer** function of the custom **MyBufPool** is implemented to allocate memory for **TQue** and **Tbuf**.

```
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
    
    // MyBufPool is the custom TBufPool class name. Select VECCALC as the logical position of the custom TBufPool.
    // The number of Buffer blocks allocated by the custom TBufPool is 16.
    EXTERN_IMPL_BUFPOOL(MyBufPool, AscendC::TPosition::VECCALC, 16);
};
```
