# C API Programming Based on the Language Extension Layer<a name="ZH-CN_TOPIC_0000002509743869"></a>

<!-- md-trans-meta sourceCommit=ee4e394ae45a91991ceeae2ecca994e89d3374e6 translatedAt=2026-08-20T11:10:52.217Z pushedAt=2026-08-29T10:01:39.096Z -->

When programming with the C API based on the language extension layer, pure C-style APIs are provided, which conform to the C-language operator development habits and deliver a programming experience similar to that in the industry. This section mainly describes the C API programming paradigm. Through the introduction of memory management, synchronization control, and computation and data movement APIs, it helps developers better understand and use the C API for programming.

## Memory Management<a name="section2092632955315"></a>

C API uses C-style address qualifiers to describe memory at different levels, and allows direct manipulation of memory addresses through pointers, thereby precisely controlling data placement. The address qualifiers of different storage units are described as follows:

**Table 1** Address qualifiers of different storage units

<a name="table16278354141117"></a>

<table><thead align="left"><tr id="row1827835418116"><th class="cellrowborder" valign="top" width="22.33%" id="mcps1.2.4.1.1"><p id="p1927845481114"><a name="p1927845481114"></a><a name="p1927845481114"></a>Storage Unit</p></th>
<th class="cellrowborder" valign="top" width="13.98%" id="mcps1.2.4.1.2"><p id="p15278165413113"><a name="p15278165413113"></a><a name="p15278165413113"></a>Address Qualifier</p></th>
<th class="cellrowborder" valign="top" width="63.690000000000005%" id="mcps1.2.4.1.3"><p id="p71391214011"><a name="p71391214011"></a><a name="p71391214011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5989518428"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p12786547114"><a name="p12786547114"></a><a name="p12786547114"></a><span id="ph64037249135"><a name="ph64037249135"></a><a name="ph64037249135"></a>Global Memory</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p112781554101116"><a name="p112781554101116"></a><a name="p112781554101116"></a>__gm__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p39451758171917"><a name="p39451758171917"></a><a name="p39451758171917"></a>Indicates that the qualified variable is located in the Global Memory address space.</p></td>
</tr>
<tr id="row1427810549118"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p152784546112"><a name="p152784546112"></a><a name="p152784546112"></a><span id="ph0136142113115"><a name="ph0136142113115"></a><a name="ph0136142113115"></a>Unified Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1984262913010"><a name="p1984262913010"></a><a name="p1984262913010"></a>__ubuf__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p101461561148"><a name="p101461561148"></a><a name="p101461561148"></a>Indicates that the qualified variable is located in the Unified Buffer address space.</p></td>
</tr>
<tr id="row15278135441120"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p14278954181110"><a name="p14278954181110"></a><a name="p14278954181110"></a><span id="ph188413550332"><a name="ph188413550332"></a><a name="ph188413550332"></a>L1 Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1809351902"><a name="p1809351902"></a><a name="p1809351902"></a>__cbuf__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p089110415513"><a name="p089110415513"></a><a name="p089110415513"></a>Indicates that the qualified variable is located in the L1 Buffer address space.</p></td>
</tr>
<tr id="row1911115484402"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p12111548144020"><a name="p12111548144020"></a><a name="p12111548144020"></a><span id="ph15972183811415"><a name="ph15972183811415"></a><a name="ph15972183811415"></a>L0A Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1411164812407"><a name="p1411164812407"></a><a name="p1411164812407"></a>__ca__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p187636912512"><a name="p187636912512"></a><a name="p187636912512"></a>Indicates that the qualified variable is located in the L0A Buffer address space.</p></td>
</tr>
<tr id="row173638244210"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p8363724125"><a name="p8363724125"></a><a name="p8363724125"></a><span id="ph56746330210"><a name="ph56746330210"></a><a name="ph56746330210"></a>L0B Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p953114271829"><a name="p953114271829"></a><a name="p953114271829"></a>__cb__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p844515343514"><a name="p844515343514"></a><a name="p844515343514"></a>Indicates that the qualified variable is located in the L0B Buffer address space.</p></td>
</tr>
<tr id="row7837101412434"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p158371714124319"><a name="p158371714124319"></a><a name="p158371714124319"></a><span id="ph17632034174314"><a name="ph17632034174314"></a><a name="ph17632034174314"></a>L0C Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1994214412"><a name="p1994214412"></a><a name="p1994214412"></a>__cc__</p></td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p1685917341053"><a name="p1685917341053"></a><a name="p1685917341053"></a>Indicates that the qualified variable is located in the L0C Buffer address space.</p></td>
</tr>
</tbody>
</table>

Address space qualifiers can be used in array or pointer variable declarations to specify the region where the object is allocated. Multiple address space qualifiers are not allowed on the same type.

When programming based on the C API, developers need to explicitly manage memory on their own. The methods for applying for memory at different levels are described as follows:

-   Global Memory: Generally allocated and passed in through the device-side API **aclrtMalloc**, and the corresponding address qualifier must be added for use.

-   Internal Memory (including Unified Buffer and L1 Buffer): The space is allocated by the user and declared inside the kernel through address qualifier keywords. There is no automatic garbage collection mechanism, so developers must strictly control the lifecycle. The following uses allocating UB space as an example:

```
// Use the address space qualifier in the array variable declaration.
// total_length indicates the length of the data involved in the computation.
constexpr uint64_t total_length = 256;
__ubuf__ float xLocal[ total_length ];
__ubuf__ float yLocal[ total_length ];
__ubuf__ float zLocal[ total_length ];

// Use the address space qualifier in the pointer variable declaration.
uint64_t offset = 0;                                   // Allocate memory for src0 first, starting from 0.
__ubuf__ half* src0 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // Obtain the address of src0, and use the __ubuf__ keyword to specify that the address points to UB memory.
```

## Synchronization Control<a name="section6735112217720"></a>

The NPU contains different compute units. Before computation, the data to be computed often needs to be moved to the compute units. The computation processes and data movement processes on different compute units can be divided into different pipelines, as described in the following table.

**Table 2** Instruction pipeline types and descriptions

<a name="table122172503915"></a>

<table><thead align="left"><tr id="row82178506918"><th class="cellrowborder" valign="top" width="23.03%" id="mcps1.2.3.1.1"><p id="p1821725011911"><a name="p1821725011911"></a><a name="p1821725011911"></a>Pipeline Type</p></th>
<th class="cellrowborder" valign="top" width="76.97%" id="mcps1.2.3.1.2"><p id="p8217145014917"><a name="p8217145014917"></a><a name="p8217145014917"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row621713504920"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p202175501295"><a name="p202175501295"></a><a name="p202175501295"></a>PIPE_S</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p5217650394"><a name="p5217650394"></a><a name="p5217650394"></a>Scalar pipeline</p></td>
</tr>
<tr id="row721710502092"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p82170501916"><a name="p82170501916"></a><a name="p82170501916"></a>PIPE_V</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1921716505917"><a name="p1921716505917"></a><a name="p1921716505917"></a>Vector computation pipeline and the L0C Buffer-&gt;UB data movement pipeline on some hardware architectures</p></td>
</tr>
<tr id="row1883183712359"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p138319375357"><a name="p138319375357"></a><a name="p138319375357"></a>PIPE_M</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p7832377352"><a name="p7832377352"></a><a name="p7832377352"></a>Matrix computation pipeline</p></td>
</tr>
<tr id="row148311377357"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p1583537163515"><a name="p1583537163515"></a><a name="p1583537163515"></a>PIPE_MTE1</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p168333753515"><a name="p168333753515"></a><a name="p168333753515"></a>L1 Buffer -&gt;L0A Buffer, L1 Buffer-&gt;L0B Buffer data movement pipeline</p></td>
</tr>
<tr id="row1721716501098"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p112181650399"><a name="p112181650399"></a><a name="p112181650399"></a>PIPE_MTE2</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1821811501298"><a name="p1821811501298"></a><a name="p1821811501298"></a>Data movement pipelines such as GM->L1 Buffer and GM->UB</p></td>
</tr>
<tr id="row112183501492"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p921865017916"><a name="p921865017916"></a><a name="p921865017916"></a>PIPE_MTE3</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1221865012914"><a name="p1221865012914"></a><a name="p1221865012914"></a>Data movement pipelines such as UB->GM</p></td>
</tr>
<tr id="row142181150993"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p132180506918"><a name="p132180506918"></a><a name="p132180506918"></a>PIPE_FIX</p></td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p11218050091"><a name="p11218050091"></a><a name="p11218050091"></a>Data movement pipelines such as L0C Buffer->GM and L0C Buffer->L1</p></td>
</tr>
</tbody>
</table>

When writing operators by calling the data movement or computation APIs provided by C API, you need to insert corresponding synchronization events based on the data dependencies between pipelines. C API provides two different synchronization control APIs, with synchronization control granularity ranging from coarse to fine, helping developers precisely adapt to the hardware architecture and tap into the performance limits of heterogeneous computation.

First type: synchronization APIs consistent with the static tensor programming approach, which are mainly managed in a fine-grained manner through the **asc_sync_notify**/**asc_sync_wait** APIs. You need to manually manage the event type and event ID, and also consider forward synchronization (intra-loop dependency) and backward synchronization (inter-loop dependency). This approach is recommended for ultimate performance scenarios. The usage example is as follows:

```
// This snippet is only used to illustrate the relationship among data movement, vector computation, and synchronization operations. For the complete parameters and context of each API, see the programming examples below.
asc_copy_gm2ub(); // GM->UB data movement pipeline
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_add(); // Vector computation pipeline
asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_copy_ub2gm(); // UB->GM data movement pipeline
```

Second type: synchronization APIs that do not require awareness of the pipeline type. They are implemented by adding the **asc_sync** API after the instruction of the corresponding pipeline type. When using this type of synchronization API, you do not need to consider the instruction pipeline type. The API internally manages the synchronization of all instruction pipelines automatically, simplifying synchronization instructions. This method can be used in performance-insensitive scenarios. The usage example is as follows:

```
// This snippet is only used to illustrate the relationship among data movement, vector computation, and synchronization operations. For the complete parameters and context of each API, see the programming examples below.
asc_copy_gm2ub();// GM->UB data movement pipeline
asc_sync(); // Full synchronization; no need to consider the subsequent instruction pipeline
asc_add(); // Vector computation pipeline
asc_sync(); // Full synchronization, no need to consider the subsequent instruction pipeline
asc_copy_ub2gm(); // UB->GM data movement pipeline
```

In addition, C API provides a set of data movement and computation APIs with the built-in synchronization capabilities. Developers do not need to explicitly manage synchronization, because the synchronization operations are hidden in the corresponding APIs. This method is recommended for performance-insensitive scenarios. The usage example is as follows:

```
// This snippet is only used to illustrate the relationship among data movement, vector computation, and synchronization operations. For the complete parameters and context of each API, see the programming examples below.
asc_copy_gm2ub_sync(); // The GM->UB data movement pipeline also includes synchronization with any subsequent instruction pipeline.
asc_add_sync(); // The vector computation pipeline also includes synchronization with any subsequent instruction pipeline.
asc_copy_ub2gm_sync(); // The UB->GM data movement pipeline also includes synchronization with any subsequent instruction pipeline.
```

## Programming Example<a name="section1825793122916"></a>

Complete example of memory management and fine-grained synchronization:

```
#include <cstdint>
#include "c_api/asc_simd.h"

constexpr uint32_t C_API_ONE_BLOCK_SIZE = 32;
constexpr uint32_t C_API_ONE_REPEAT_BYTE_SIZE = 256;
constexpr uint32_t C_API_TOTAL_LENGTH = 16384;
constexpr uint32_t C_API_TILE_NUM = 8;
constexpr uint32_t C_API_TILE_LENGTH = 256;

__vector__ __global__ __aicore__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    asc_init();

    uint32_t blockLength = C_API_TOTAL_LENGTH / asc_get_block_num();
    uint32_t tileLength = blockLength / C_API_TILE_NUM;

    __gm__ float* xGm = x + asc_get_block_idx() * blockLength;
    __gm__ float* yGm = y + asc_get_block_idx() * blockLength;
    __gm__ float* zGm = z + asc_get_block_idx() * blockLength;

    __ubuf__ float xLocal[C_API_TILE_LENGTH];
    __ubuf__ float yLocal[C_API_TILE_LENGTH];
    __ubuf__ float zLocal[C_API_TILE_LENGTH];

    uint16_t len_burst = tileLength;
    for (uint32_t i = 0; i < C_API_TILE_NUM; i++) {
        if (i != 0) {
            asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
        }

        len_burst = tileLength * sizeof(float) / C_API_ONE_BLOCK_SIZE;
        asc_copy_gm2ub(xLocal, xGm + i * tileLength, 0, 1, len_burst, 0, 0);    
        asc_copy_gm2ub(yLocal, yGm + i * tileLength, 0, 1, len_burst, 0, 0);

        asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

        if (i != 0) {
            asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
        }

        asc_add(zLocal, xLocal, yLocal, tileLength * sizeof(float) / C_API_ONE_REPEAT_BYTE_SIZE, 1, 1, 1, 8, 8, 8);

        if (i != (C_API_TILE_NUM-1)) {
            asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0);
        }

        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

        asc_copy_ub2gm(zGm + i * tileLength, zLocal, 0, 1, len_burst, 0, 0);

        if (i != (C_API_TILE_NUM-1)) {
            asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
        }
    }
}
```

The following is a complete example of memory management and synchronization management that is unaware of the pipeline type:

```
#include <cstdint>
#include "c_api/asc_simd.h"

constexpr uint32_t TILE_LENGTH = 2048;
constexpr uint32_t NUM_BLOCKS = 8;

__vector__ __global__ __aicore__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    asc_init();

    uint32_t blockLength = NUM_BLOCKS * TILE_LENGTH / asc_get_block_num();

    __gm__ float* xGm = x + asc_get_block_idx() * blockLength;
    __gm__ float* yGm = y + asc_get_block_idx() * blockLength;
    __gm__ float* zGm = z + asc_get_block_idx() * blockLength;

    __ubuf__ float xLocal[TILE_LENGTH];
    __ubuf__ float yLocal[TILE_LENGTH];
    __ubuf__ float zLocal[TILE_LENGTH];

    asc_copy_gm2ub((__ubuf__ void*)xLocal, (__gm__ void*)xGm, blockLength * sizeof(float));
    asc_copy_gm2ub((__ubuf__ void*)yLocal, (__gm__ void*)yGm, blockLength * sizeof(float));
    asc_sync();

    asc_add(zLocal, xLocal, yLocal, blockLength);
    asc_sync();

    asc_copy_ub2gm((__gm__ void*)zGm, (__ubuf__ void*)zLocal, blockLength * sizeof(float));
    asc_sync();
}
```

The following is a complete example of memory management and using APIs with the synchronization capabilities:

```
#include <cstdint>
#include "c_api/asc_simd.h"

constexpr uint32_t TILE_LENGTH = 2048;
constexpr uint32_t NUM_BLOCKS = 8;

__vector__ __global__ __aicore__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    asc_init();

    __ubuf__ float xLocal[TILE_LENGTH];
    __ubuf__ float yLocal[TILE_LENGTH];
    __ubuf__ float zLocal[TILE_LENGTH];

    uint32_t blockLength = TILE_LENGTH * NUM_BLOCKS / asc_get_block_num();

    asc_copy_gm2ub_sync((__ubuf__ void*)xLocal, (__gm__ void*)(x + asc_get_block_idx() * blockLength), blockLength * sizeof(float));
    asc_copy_gm2ub_sync((__ubuf__ void*)yLocal, (__gm__ void*)(y + asc_get_block_idx() * blockLength), blockLength * sizeof(float));

    asc_add_sync(zLocal, xLocal, yLocal, blockLength);

    asc_copy_ub2gm_sync((__gm__ void*)(z + asc_get_block_idx() * blockLength), (__ubuf__ void*)zLocal, blockLength * sizeof(float));
}

```

The following is a complete example of memory management, Reg vector computation, and fine-grained synchronization:

```
#include <cstdint>
#include "c_api/asc_simd.h"

constexpr uint32_t TILE_LENGTH = 2048;
constexpr uint32_t NUM_BLOCKS = 8;
constexpr uint32_t BLK_NUM = 1;
constexpr uint32_t MASK = 32;

__simd_vf__ inline void AddVF(uint16_t rep, uint16_t one_rep_size, uint32_t blockLength, __ubuf__ float* xLocal, __ubuf__ float* yLocal, __ubuf__ float* zLocal)
{
    vector_bool vmask;
    vector_float reg_src0;
    vector_float reg_src1;
    vector_float reg_dst;
    uint32_t remaining = blockLength;
    for (uint16_t i = 0; i < rep; ++i) {
        vmask = asc_update_mask_b32(remaining);
        asc_loadalign(reg_src0, xLocal + i * one_rep_size);
        asc_loadalign(reg_src1, yLocal + i * one_rep_size);    
        asc_add(reg_dst, reg_src0, reg_src1, vmask);
        asc_storealign(zLocal + i * one_rep_size, reg_dst, vmask);
    }
}

__vector__ __global__ __aicore__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    asc_init();

    uint32_t blockLength = TILE_LENGTH * NUM_BLOCKS / asc_get_block_num();

    __gm__ float* xGm = x + get_block_idx() * blockLength;
    __gm__ float* yGm = y + get_block_idx() * blockLength;
    __gm__ float* zGm = z + get_block_idx() * blockLength;

    __ubuf__ float xLocal[TILE_LENGTH];
    __ubuf__ float yLocal[TILE_LENGTH];
    __ubuf__ float zLocal[TILE_LENGTH];

    const uint8_t cacheMode0 = static_cast<uint8_t>(((uint64_t)xGm) >> 60);
    const uint8_t cacheMode1 = static_cast<uint8_t>(((uint64_t)yGm) >> 60);
    const uint8_t cacheMode2 = static_cast<uint8_t>(((uint64_t)zGm) >> 60);
    uint32_t burstLength = blockLength * 32;
    uint64_t srcStride = burstLength;
    uint32_t dstStride = (burstLength + 31) / 32 * 32;

    asc_copy_gm2ub_align((__ubuf__ float*)xLocal, xGm, BLK_NUM, burstLength, 0, 0, true, cacheMode0, srcStride, dstStride);
    asc_copy_gm2ub_align((__ubuf__ float*)yLocal, yGm, BLK_NUM, burstLength, 0, 0, true, cacheMode1, srcStride, dstStride);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint16_t mask_bit_size = 256;
    uint16_t one_rep_size = mask_bit_size / sizeof(float);
    uint16_t rep = (blockLength + one_rep_size - 1) / one_rep_size;
    asc_vf_call<AddVF>(rep, one_rep_size, blockLength, (__ubuf__ float*)xLocal, (__ubuf__ float*)yLocal, (__ubuf__ float*)zLocal );
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(zGm, (__ubuf__ float*)zLocal, BLK_NUM, burstLength, cacheMode2, srcStride, dstStride);
}
```