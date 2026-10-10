# 基于语言扩展层C API编程<a name="ZH-CN_TOPIC_0000002509743869"></a>

基于语言扩展层C API编程时，通过提供纯C风格的接口，符合C语言算子开发习惯，提供与业界类似编程体验。本节主要介绍C API编程范式，通过内存管理、同步控制、计算及搬运接口相关的介绍，使开发者更好地理解和使用C API进行编程。

## 内存管理<a name="section2092632955315"></a>

C API通过C风格的地址限定符描述不同层级内存，并且可以通过指针直接操作内存地址，从而精准控制数据存放位置。不同存储单元的地址限定符介绍如下：

**表1**  不同存储单元的地址限定符

<a name="table16278354141117"></a>
<table><thead align="left"><tr id="row1827835418116"><th class="cellrowborder" valign="top" width="22.33%" id="mcps1.2.4.1.1"><p id="p1927845481114"><a name="p1927845481114"></a><a name="p1927845481114"></a>存储单元</p>
</th>
<th class="cellrowborder" valign="top" width="13.98%" id="mcps1.2.4.1.2"><p id="p15278165413113"><a name="p15278165413113"></a><a name="p15278165413113"></a>地址限定符</p>
</th>
<th class="cellrowborder" valign="top" width="63.690000000000005%" id="mcps1.2.4.1.3"><p id="p71391214011"><a name="p71391214011"></a><a name="p71391214011"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row5989518428"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p12786547114"><a name="p12786547114"></a><a name="p12786547114"></a><span id="ph64037249135"><a name="ph64037249135"></a><a name="ph64037249135"></a>Global Memory</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p112781554101116"><a name="p112781554101116"></a><a name="p112781554101116"></a>__gm__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p39451758171917"><a name="p39451758171917"></a><a name="p39451758171917"></a>表示被修饰的变量位于Global Memory地址空间。</p>
</td>
</tr>
<tr id="row1427810549118"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p152784546112"><a name="p152784546112"></a><a name="p152784546112"></a><span id="ph0136142113115"><a name="ph0136142113115"></a><a name="ph0136142113115"></a>Unified Buffer</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1984262913010"><a name="p1984262913010"></a><a name="p1984262913010"></a>__ubuf__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p101461561148"><a name="p101461561148"></a><a name="p101461561148"></a>表示被修饰的变量位于Unified Buffer地址空间。</p>
</td>
</tr>
<tr id="row15278135441120"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p14278954181110"><a name="p14278954181110"></a><a name="p14278954181110"></a><span id="ph188413550332"><a name="ph188413550332"></a><a name="ph188413550332"></a>L1 Buffer</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1809351902"><a name="p1809351902"></a><a name="p1809351902"></a>__cbuf__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p089110415513"><a name="p089110415513"></a><a name="p089110415513"></a>表示被修饰的变量位于L1 Buffer地址空间。</p>
</td>
</tr>
<tr id="row1911115484402"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p12111548144020"><a name="p12111548144020"></a><a name="p12111548144020"></a><span id="ph15972183811415"><a name="ph15972183811415"></a><a name="ph15972183811415"></a>L0A Buffer</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1411164812407"><a name="p1411164812407"></a><a name="p1411164812407"></a>__ca__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p187636912512"><a name="p187636912512"></a><a name="p187636912512"></a>表示被修饰的变量位于L0A Buffer地址空间。</p>
</td>
</tr>
<tr id="row173638244210"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p8363724125"><a name="p8363724125"></a><a name="p8363724125"></a><span id="ph56746330210"><a name="ph56746330210"></a><a name="ph56746330210"></a>L0B Buffer</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p953114271829"><a name="p953114271829"></a><a name="p953114271829"></a>__cb__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p844515343514"><a name="p844515343514"></a><a name="p844515343514"></a>表示被修饰的变量位于L0B Buffer地址空间。</p>
</td>
</tr>
<tr id="row7837101412434"><td class="cellrowborder" valign="top" width="22.33%" headers="mcps1.2.4.1.1 "><p id="p158371714124319"><a name="p158371714124319"></a><a name="p158371714124319"></a><span id="ph17632034174314"><a name="ph17632034174314"></a><a name="ph17632034174314"></a>L0C Buffer</span></p>
</td>
<td class="cellrowborder" valign="top" width="13.98%" headers="mcps1.2.4.1.2 "><p id="p1994214412"><a name="p1994214412"></a><a name="p1994214412"></a>__cc__</p>
</td>
<td class="cellrowborder" valign="top" width="63.690000000000005%" headers="mcps1.2.4.1.3 "><p id="p1685917341053"><a name="p1685917341053"></a><a name="p1685917341053"></a>表示被修饰的变量位于L0C Buffer地址空间。</p>
</td>
</tr>
</tbody>
</table>

地址空间限定符可以在数组或指针变量声明中使用，用于指定对象分配的区域。同一个类型上不允许使用多个地址空间限定符。

基于C API编程时，开发者需要自行通过显式的内存管理来控制内存，不同层级的内存申请介绍如下：

-   全局内存（Global Memory）：一般通过Device侧接口aclrtMalloc接口分配传入，需要增加对应地址限定符使用。
-   内部存储（包含Unified Buffer、L1 Buffer等）：由用户自行申请空间，通过地址限定符关键字在Kernel内声明。无自动垃圾回收机制，需开发者严格控制生命周期。以申请UB空间为例：

```
// 在数组变量声明中使用地址空间限定符
// total_length指参与计算的数据长度
constexpr uint64_t total_length = 256;
__ubuf__ float xLocal[ total_length ];
__ubuf__ float yLocal[ total_length ];
__ubuf__ float zLocal[ total_length ];

// 在指针变量声明中使用地址空间限定符
uint64_t offset = 0;                                   // 首先为src0申请内存，从0开始。
__ubuf__ half* src0 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // 获取src0的地址，通过__ubuf__关键字指定该地址指向UB内存。
```

## 同步控制<a name="section6735112217720"></a>

NPU内部有不同的计算单元，在计算前往往需要把计算数据搬运到计算单元上。不同计算单元上的计算过程、数据搬运过程可划分为不同的流水线。如下表所示：

**表2**  指令流水类型和相关说明

<a name="table122172503915"></a>
<table><thead align="left"><tr id="row82178506918"><th class="cellrowborder" valign="top" width="23.03%" id="mcps1.2.3.1.1"><p id="p1821725011911"><a name="p1821725011911"></a><a name="p1821725011911"></a>流水类型</p>
</th>
<th class="cellrowborder" valign="top" width="76.97%" id="mcps1.2.3.1.2"><p id="p8217145014917"><a name="p8217145014917"></a><a name="p8217145014917"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row621713504920"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p202175501295"><a name="p202175501295"></a><a name="p202175501295"></a>PIPE_S</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p5217650394"><a name="p5217650394"></a><a name="p5217650394"></a>标量流水线</p>
</td>
</tr>
<tr id="row721710502092"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p82170501916"><a name="p82170501916"></a><a name="p82170501916"></a>PIPE_V</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1921716505917"><a name="p1921716505917"></a><a name="p1921716505917"></a>矢量计算流水及部分硬件架构下的L0C Buffer-&gt;UB数据搬运流水</p>
</td>
</tr>
<tr id="row1883183712359"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p138319375357"><a name="p138319375357"></a><a name="p138319375357"></a>PIPE_M</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p7832377352"><a name="p7832377352"></a><a name="p7832377352"></a>矩阵计算流水</p>
</td>
</tr>
<tr id="row148311377357"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p1583537163515"><a name="p1583537163515"></a><a name="p1583537163515"></a>PIPE_MTE1</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p168333753515"><a name="p168333753515"></a><a name="p168333753515"></a>L1 Buffer -&gt;L0A Buffer、L1 Buffer-&gt;L0B Buffer数据搬运流水</p>
</td>
</tr>
<tr id="row1721716501098"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p112181650399"><a name="p112181650399"></a><a name="p112181650399"></a>PIPE_MTE2</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1821811501298"><a name="p1821811501298"></a><a name="p1821811501298"></a>GM-&gt;L1 Buffer、GM-&gt;UB等数据搬运流水</p>
</td>
</tr>
<tr id="row112183501492"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p921865017916"><a name="p921865017916"></a><a name="p921865017916"></a>PIPE_MTE3</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p1221865012914"><a name="p1221865012914"></a><a name="p1221865012914"></a>UB-&gt;GM等数据搬运流水</p>
</td>
</tr>
<tr id="row142181150993"><td class="cellrowborder" valign="top" width="23.03%" headers="mcps1.2.3.1.1 "><p id="p132180506918"><a name="p132180506918"></a><a name="p132180506918"></a>PIPE_FIX</p>
</td>
<td class="cellrowborder" valign="top" width="76.97%" headers="mcps1.2.3.1.2 "><p id="p11218050091"><a name="p11218050091"></a><a name="p11218050091"></a>L0C Buffer-&gt;GM、L0C Buffer -&gt;L1等数据搬运流水</p>
</td>
</tr>
</tbody>
</table>

在调用C API提供的搬运或者计算类API编写算子时，需要根据流水线之间的数据依赖关系插入对应的同步事件。C API提供了两种不同的同步控制接口，同步控制粒度由浅到深，帮助开发者精准适配硬件架构，挖掘异构计算的性能极限。

第一种：和静态Tensor编程方式一致的同步接口，主要通过asc\_sync\_notify/asc\_sync\_wait接口来精细化管理，需要手动管理事件的类型和事件ID，还需要考虑正向同步（循环内依赖）与反向同步（循环间依赖）。极致性能场景推荐使用此方式。使用示例如下：

```
// 本片段仅用于说明数据搬运、矢量计算、同步操作间的关系。各接口的完整参数及上下文请参考下文中的编程示例。
asc_copy_gm2ub(); // GM->UB的搬运流水
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_add(); // 矢量计算流水
asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_copy_ub2gm(); // UB->GM的搬运流水
```

第二种：不感知流水类型的同步接口，将asc\_sync接口添加在对应流水类型的指令后面来实现。使用这类同步接口时，不需要考虑指令流水类型，接口内部会自动管理所有指令流水的同步，简化同步指令。性能不敏感场景下，可以使用此方式。使用示例如下：

```
// 本片段仅用于说明数据搬运、矢量计算、同步操作间的关系。各接口的完整参数及上下文请参考下文中的编程示例。
asc_copy_gm2ub();// GM->UB的搬运流水
asc_sync(); // 全同步 无需考虑后面的指令流水
asc_add(); // 矢量计算流水
asc_sync(); // 全同步 无需考虑后面的指令流水
asc_copy_ub2gm(); // UB->GM的搬运流水
```

另外，C API还提供了一组包含同步能力的搬运及计算接口，开发者无需显式手动管理同步，同步操作隐藏在相应的接口中。性能不敏感场景下，推荐使用此方式。使用示例如下：

```
// 本片段仅用于说明数据搬运、矢量计算、同步操作间的关系。各接口的完整参数及上下文请参考下文中的编程示例。
asc_copy_gm2ub_sync(); // GM->UB的搬运流水同时包含了和后面的任意指令流水的同步
asc_add_sync(); // 矢量计算流水同时包含了和后面的任意指令流水的同步
asc_copy_ub2gm_sync(); // UB->GM的搬运流水同时包含了和后面的任意指令流水的同步
```

## 编程示例<a name="section1825793122916"></a>

内存管理与精细化同步完整示例：

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
        asc_copy_gm2ub(xLocal, xGm + i * tileLength, 1, len_burst, 0, 0);
        asc_copy_gm2ub(yLocal, yGm + i * tileLength, 1, len_burst, 0, 0);

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

        asc_copy_ub2gm(zGm + i * tileLength, zLocal, 1, len_burst, 0, 0);

        if (i != (C_API_TILE_NUM-1)) {
            asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
        }
    }
}
```

内存管理与不感知流水类型的同步管理完整示例如下：

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

内存管理与使用带同步能力的接口完整示例如下：

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

内存管理、Reg矢量计算与精细化同步完整示例：

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

