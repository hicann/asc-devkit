# DataCacheCleanAndInvalid<a name="ZH-CN_TOPIC_0000001787873306"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:02:29.323Z -->

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
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

As shown in Figure 1, inside the AI Core, both the Scalar unit and the DMA unit may access GM:

- The DMA unit reads from and writes to GM. Data is exchanged between Local Memory such as UB and GM through APIs such as **DataCopy**, so there is no cache consistency issue.
- When the Scalar unit accesses GM, it first accesses the Data Cache in each core, so there is a cache consistency issue between the Data Cache and GM.

**Figure 1** DataCache memory hierarchy diagram<a name="zh-cn_topic_0000002530021782_fig1161014168448"></a>  

![](../../../figures/datacache.png)

The **DataCacheCleanAndInvalid** API is used to resolve the consistency issue described above. Its functionality can be divided into two parts:

- Clean: Writes the modified data (dirty data) in the cache back to GM to avoid data loss.
- Invalid:
    - Marks the Cache Lines in the specified address range as "invalid" so that they are removed from the cache.
    - Ensures that the next access to these memory addresses reloads data from GM instead of using possibly stale cached data.

Typical use cases of this API are as follows:

- When reading data from GM that may have been modified externally by other cores, use **DataCacheCleanAndInvalid** to access GM directly and obtain the latest data.
- When a user writes data to GM through the Scalar unit and wants the data to be written out immediately, **DataCacheCleanAndInvalid** is also required.

<!-- npu="950" id15 -->
- For Ascend 950PR/Ascend 950DT, during atomic operations, if you want to change the saturation mode of subsequent data, first use **DataCacheCleanAndInvalid** to write out the existing data in the Cache Line immediately, and then call [SetCtrlSpr](../special_register_access/SetCtrlSpr_ISASI.md) to set the saturation mode of subsequent data.
<!-- end id15 -->

## Prototype<a name="section620mcpsimp"></a>

- Prototype with **dcciDst** configured.

  ```cpp
  // Supports ensuring consistency between DCache and GM storage by configuring dcciDst.
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
  // Supports ensuring consistency between DCache and Local Memory storage by configuring dcciDst.
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const LocalTensor<T>& dst)
  ```

- Prototype without **dcciDst** configured, which only ensures consistency between DCache and GM.

    ```cpp
    template <typename T, CacheLine entireType>
    __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
    ```

<!-- npu="310p" id14 -->
> [!NOTE]Note
> For the AI Core of Atlas inference products, only the prototype without the **dcciDst** parameter is supported.
<!-- end id14 -->


## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of **dst**. |
| entireType | Instruction operation mode:<br>**SINGLE_CACHE_LINE**: Only the Cache Line where the passed address resides is refreshed. **Note that if the address is not 64B-aligned, only the portion from the passed address to the 64B alignment boundary is operated.**<br>**ENTIRE_DATA_CACHE**: The passed address is invalid in this case. The entire DCache is refreshed within the core, but this is time-consuming. **Use with caution in performance-sensitive scenarios.** |
| dcciDst | Indicates the storage with which this API maintains DCache consistency. The type is the **DcciDst** enum class.<br>&bull; **CACHELINE_ALL**: Same effect as **CACHELINE_OUT**.<br>&bull; **CACHELINE_UB**: Indicates that this API maintains consistency between DCache and UB.<br>&bull; **CACHELINE_OUT**: Indicates that this API maintains consistency between DCache and GM.<br>&bull; **CACHELINE_ATOMIC**: Maintains consistency between DCache and GM during atomic operations.<br><!-- npu="950" id16 -->&ensp;&ensp;- Ascend 950PR/Ascend 950DT: Supported.<br><!-- end id16 --><!-- npu="A3" id10 -->&ensp;&ensp;- Atlas A3 training products/Atlas A3 inference products: not yet supported.<br><!-- end id10 --><!-- npu="910b" id11 -->&ensp;&ensp;- Atlas A2 training products/Atlas A2 inference products: not yet supported.<br><!-- end id11 --><!-- npu="310b" id12 -->&ensp;&ensp;- Atlas 200I/500 A2 inference products: not yet supported.<br><!-- end id12 --><!-- npu="310p" id13 -->&ensp;&ensp;- Atlas inference products AI Core: not yet supported.<!-- end id13 --> |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| dst | Input | Tensor whose Cache needs to be refreshed. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

- **Example 1: SINGLE\_CACHE\_LINE mode (64B alignment scenario)**  
  Assuming **mmAddr_** is 0x40 (64B-aligned), a single instruction call refreshes the entire Cache Line.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Because the start address is 64B-aligned, the first eight elements are refreshed immediately after the DataCacheCleanAndInvalid instruction is called.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **Example 2: SINGLE\_CACHE\_LINE mode (non-64B alignment scenario)**  
  Assuming **mmAddr_** is 0x20 (not 64B-aligned), two instruction calls are required to refresh the complete Cache Line.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Because the start address is not 64B-aligned, a single instruction call refreshes only the portion from the start address to the 64B-aligned boundary, that is, the first four elements.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    // Call the DataCacheCleanAndInvalid instruction again to refresh the last four elements.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global[4]);
    ```

- **<a name="example3_multi_core"></a>Example 3: SINGLE\_CACHE\_LINE mode (multi-core processing scenario)**  
  Assuming **mmAddr_** is 0x40 (64B-aligned), random overwrites may occur when multiple cores operate on the same Cache Line. This sample is provided only for illustration to help developers understand the usage restrictions, and is not a normal usage example.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_);
    global.SetValue(AscendC::GetBlockIdx(), AscendC::GetBlockIdx());
    // In the operator, although multi-core operations do not target the same address, they fall within the same Cache Line, which may cause random data overwriting and differs from the behavior of a general-purpose CPU.
    // After the DataCacheCleanAndInvalid instruction is invoked, because the timing of multi-core operations is inconsistent, the final result is nondeterministic, and the core that executes later overwrites the result of the core that executed earlier.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **Example 4: ENTIRE\_DATA\_CACHE mode**  
  Assume that mmAddr\_ is 0x20 (not 64B-aligned), and the entire DCache is flushed. This example is provided only for illustration to help developers understand the usage restrictions and is not a normal usage example.

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // Flush the entire DCache, which delivers poor performance.
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```
