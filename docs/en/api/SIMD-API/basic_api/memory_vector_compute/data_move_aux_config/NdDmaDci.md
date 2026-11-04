# NdDmaDci<a name="ZH-CN_TOPIC_0000002414107834"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:52:14.512Z pushedAt=2026-10-28T03:14:39.504Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Refreshes the NDDMA DataCache. In multi-core scenarios, when multiple AI Cores read the same GM (Global Memory) address, the NDDMA DataCache may cache stale data, causing some cores to read incorrect data. Therefore, before using [DataCopy (GMToUB multidimensional data NDDMA move)](../data_move/DataCopy_GMToUB_NDDMA.md), you need to call the NdDmaDci API to refresh the NDDMA DataCache and ensure that the data in the cache is up to date.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void NdDmaDci()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

This API is called only before using [DataCopy (GMToUB multidimensional data NDDMA move)](../data_move/DataCopy_GMToUB_NDDMA.md) to refresh the NDDMA DataCache.

## Examples<a name="section1227835243314"></a>

For a complete operator example, see [Multidimensional Data Movement Example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma).

Take the 2D Padding scenario as an example. Data with the shape [2, 8] in GM is moved to the Unified Buffer (UB) and padded to [4, 16], with left padding 3, top padding 1, right padding 5, and bottom padding 1, and the padding value is 0. Before calling [DataCopy (GMToUB multidimensional data movement NDDMA)](../data_move/DataCopy_GMToUB_NDDMA.md), call NdDmaDci first to refresh the NDDMA DataCache.

```cpp
// T: Type of the data to be moved.
// xGm: Stores the data moved in by DataCopy.
// xLocal: Stores the data moved out by DataCopy.

// 2D Padding scenario.
// xGmShape: [2, 8]. Move 8 columns and 2 rows of data, with left padding 3, top padding 1, right padding 5, and bottom padding 1. xLocalShape: [4, 16].
AscendC::NdDmaLoopInfo<2> loopInfo{{1, 8}, {1, 16}, {8, 2}, {3, 1}, {5, 1}};
AscendC::NdDmaParams<T, 2> params{loopInfo, 0}; // The padding value is 0.
AscendC::NdDmaDci(); // Refresh the NDDMA DataCache to ensure data consistency in multi-core scenarios.
static constexpr AscendC::NdDmaConfig dmaConfig; // Use the default parameters. This argument can also be omitted.
AscendC::DataCopy<T, 2, dmaConfig>(xLocal, xGm, params);
```

The result is as follows:

```plain
Input data (xGm):
[[ 1. 2. 3. 4. 5. 6. 7. 8.]
[ 9. 10. 11. 12. 13. 14. 15. 16.]]
Data moved to Local (xLocal):
[[ 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.]
[ 0. 0. 0. 1. 2. 3. 4. 5. 6. 7. 8. 0. 0. 0. 0. 0.]
[ 0. 0. 0. 9. 10. 11. 12. 13. 14. 15. 16. 0. 0. 0. 0. 0.]
[ 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.]]
```
