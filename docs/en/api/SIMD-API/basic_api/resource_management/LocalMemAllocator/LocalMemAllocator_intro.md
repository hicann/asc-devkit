# LocalMemAllocator Introduction<a name="ZH-CN_TOPIC_0000002337856509"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-05T04:03:40.894Z pushedAt=2026-11-18T01:25:30.994Z nl=2 -->

**LocalMemAllocator** is a class used for memory management in [static Tensor programming](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md). Instead of building TPipe/TQue, users directly create **LocalTensor** objects (which can also be constructed directly through the [LocalTensor constructor](../../data_structures/LocalTensor/LocalTensor_constructor.md)) and develop operators, thereby reducing runtime overhead and achieving better performance.

**LocalMemAllocator** is supported only in the Ascend C static Tensor programming mode and cannot be mixed with APIs such as TPipe.

## Header File to Include<a name="zh-cn_topic_0000002213064918_section78885814919"></a>

```cpp
#include "kernel_operator.h"
```

## Prototype<a name="section10580930144614"></a>

```cpp
template<Hardware hard = Hardware::UB>
class LocalMemAllocator {
public:
    __aicore__ inline LocalMemAllocator();
    __aicore__ inline uint32_t GetCurAddr() const;
    template <class DataType, uint32_t tileSize> LocalTensor<DataType> __aicore__ inline Alloc();
    template <TPosition pos, class DataType, uint32_t tileSize> __aicore__ inline LocalTensor<DataType> Alloc();
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize);    
    template <TPosition pos, class DataType> LocalTensor<DataType> __aicore__ inline Alloc(uint32_t tileSize);
    
    template <class DataType> LocalTensor<DataType> __aicore__ inline Alloc();
};
```

## Template Parameters<a name="section116801320102618"></a>

**Table 1** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p2060925573411"><a name="p2060925573411"></a><a name="p2060925573411"></a>hard</p>
</td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p823866165711"><a name="p823866165711"></a><a name="p823866165711"></a><span id="ph46543142425"><a name="ph46543142425"></a><a name="ph46543142425"></a>Indicates the physical location of data. It is a Hardware enumeration type, defined as follows. Valid locations are Unified Buffer (UB), L1, L0A, L0B, L0C, BIAS, and FIXBUF.</span></p>
<a name="screen79541519214"></a><a name="screen79541519214"></a><pre class="screen" codetype="Cpp" id="screen79541519214">enum class Hardware : uint8_t { 
GM,     // Global Memory.
UB,     // UB.
L1,     // L1 Buffer.
L0A,    // L0A Buffer.
L0B,    // L0B Buffer.
L0C,    // L0C Buffer.
BIAS,   // BiasTable Buffer.
FIXBUF, // Fixpipe Buffer.
MAX };</pre>
</td>
</tr>
</tbody>
</table>
