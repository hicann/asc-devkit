# DataCacheCleanAndInvalid<a name="ZH-CN_TOPIC_0000001787873306"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:14:06.487Z pushedAt=2026-10-26T07:41:44.044Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

As shown in Figure 1, inside the AI Core, both the Scalar unit and the DMA unit may access the GM:

- The DMA unit reads from and writes to the GM. Data is exchanged between Local Memory such as the Unified Buffer (UB) and the GM through APIs such as DataCopy, so there is no cache coherence issue.
- When the Scalar unit accesses the GM, it first accesses the Data Cache (DCache) in each core. Therefore, a cache coherence issue exists between the DCache and the GM. For details, see [Cache Coherence](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md).

**Figure 1**  DCache memory hierarchy diagram<a name="zh-cn_topic_0000002530021782_fig1161014168448"></a>

![](../../../figures/datacache.png)

The **DataCacheCleanAndInvalid** API is used to resolve the preceding coherence issue. Its functionality can be divided into two parts:

- Clean: Writes the modified data (dirty data) in the cache back to the GM to prevent data loss.
- Invalid:
    - Marks the Cache Lines in the specified address range as "invalid" so that they are removed from the cache.
    - Ensures that the next access to these memory addresses reloads data from GM instead of using potentially stale cached data.

Typical usage scenarios of this API are as follows:

- When reading data from GM that may have been modified externally by other cores, use the DataCacheCleanAndInvalid API to directly access GM and obtain the latest data.
- When a user writes data to GM through the Scalar unit and wants the data to be written out immediately, the DataCacheCleanAndInvalid API is also required.

<!-- npu="950" id15 -->
- For Ascend 950PR & 950DT products, during an atomic operation, if you want to change the saturation mode of subsequent data, you must first use the DataCacheCleanAndInvalid API to immediately write out the existing data in the Cache Line, and then call [SetCtrlSpr](../special_register_access/SetCtrlSpr_ISASI.md) to set the saturation mode of subsequent data.
<!-- end id15 -->

## Prototype<a name="section620mcpsimp"></a>

- Prototype with **dcciDst** configured

  ```cpp
  // Supports ensuring consistency between DCache and GM storage by configuring dcciDst.
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
  // Supports ensuring consistency between DCache and Local Memory storage by configuring dcciDst.
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const LocalTensor<T>& dst)
  ```

- **dcciDst** configuration is not supported; only consistency between **DCache** and **GM** is guaranteed.

    ```cpp
    template <typename T, CacheLine entireType>
    __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
    ```

<!-- npu="310p" id14 -->
> [!NOTE] Note 
> For the AI Core of Atlas inference products, only the prototype without the **dcciDst** parameter is supported.
<!-- end id14 -->


## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| **T** | Data type of **dst**. |
| **entireType** | Operation mode of the instruction:<br>**SINGLE_CACHE_LINE**: Only flushes the Cache Line where the passed address resides. **Note that if the address is not 64B-aligned, only the portion from the passed address to the 64B alignment boundary is operated on.**<br>**ENTIRE_DATA_CACHE**: In this case, the passed address is invalid, and the entire DCache is flushed within the core, which is time-consuming. **Use with caution in performance-sensitive scenarios.** |
| **dcciDst** | Indicates the type of storage with which this API ensures DCache consistency. The type is the **DcciDst** enumeration.<br>&bull; **CACHELINE_ALL**: Same effect as **CACHELINE_OUT**.<br>&bull; **CACHELINE_UB**: Indicates that this API is used to ensure consistency between DCache and UB.<br>&bull; **CACHELINE_OUT**: Indicates that this API is used to ensure consistency between DCache and GM.<br>&bull; **CACHELINE_ATOMIC**: Ensures consistency between DCache and GM during atomic operations.<br><!-- npu="950" id16 -->&ensp;&ensp;- Ascend 950PR & 950DT products: supported.<br><!-- end id16 --><!-- npu="A3" id10 -->&ensp;&ensp;- Atlas A3 products: Not yet supported.<br><!-- end id10 --><!-- npu="910b" id11 -->&ensp;&ensp;- Atlas A2 products: Not yet supported.<br><!-- end id11 --><!-- npu="310b" id12 -->&ensp;&ensp;- Atlas 200I/500 A2 inference products: Not yet supported.<br><!-- end id12 --><!-- npu="310p" id13 -->&ensp;&ensp;- Atlas inference products AI Core: Not yet supported.<!-- end id13 --> |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **dst** | Input | Tensor whose cache needs to be flushed. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When `entireType` is `SINGLE_CACHE_LINE`, `dst` takes effect and `dcciDst` does not take effect.
- When `entireType` is `ENTIRE_DATA_CACHE`, `dst` does not take effect and `dcciDst` takes effect.
- The function prototype with `dst` of the `LocalTensor` type can only be called in the AIV.

## Example<a name="section837496171220"></a>

- **Example 1: SINGLE\_CACHE\_LINE mode (64B alignment scenario)**  
  Assuming mmAddr\_ is 0x40 (64B aligned), a single instruction call flushes the entire Cache Line.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
        global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Because the start address is 64B aligned, the first 8 elements are flushed immediately after the DataCacheCleanAndInvalid instruction is called.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **Example 2: SINGLE\_CACHE\_LINE mode (non-64B alignment scenario)**  
  Assuming mmAddr\_ is 0x20 (not 64B aligned), two instruction calls are required to flush the complete Cache Line.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
        global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Since the start address is not 64B-aligned, calling one instruction only flushes the portion from the start address to the 64B-aligned boundary, i.e., the first 4 elements.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    // Call the DataCacheCleanAndInvalid instruction again to flush the last 4 elements.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global[4]);
    ```

- **<a name="example3_multi_core"></a>Example 3: SINGLE\_CACHE\_LINE mode (multi-core processing scenario)**  
  Assuming mmAddr\_ is 0x40 (64B aligned), random overwrites may occur when multiple cores operate on the same Cache Line. This sample is provided only for illustration to help developers understand the usage restrictions, and is not a normal usage sample.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_);
    global.SetValue(AscendC::GetBlockIdx(), AscendC::GetBlockIdx());
    // In an operator, although multiple cores operate on different addresses, they may fall within the same cache line, causing random data overwrites, which differs from the behavior of a general-purpose CPU.
    // After the DataCacheCleanAndInvalid instruction is invoked, the final result is nondeterministic because the timing of multi-core operations differs; the core that executes later overwrites the result of the core that executed earlier.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **Example 4: ENTIRE\_DATA\_CACHE mode**  
  Assume that mmAddr\_ is 0x20 (not 64B-aligned), and the entire DCache is flushed. This sample is provided only for illustration to help developers understand the usage restrictions and is not a normal usage sample.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
        global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Flush the entire DCache, which results in poor performance.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```
