---
okftype: reference.api
description: "Set the L2 Cache management policy when the scalar unit accesses Global Memory: pass in asc_load_l2_cache_mode to configure the scalar load path, and pass in asc_store_l2_cache_mode to configure the scalar store path. The setting remains in effect until this interface is called again to modify it."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/scalar_atomic.md
    kind: configures
    condition: "the store overload takes effect on scalar atomic operations within its scope"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_datacache_preload.md
    kind: configures
    condition: "the load overload takes effect on this prefetch interface"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_icache_preload.md
    kind: configures
    condition: "the load overload takes effect on this instruction prefetch interface"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: related
    condition: "Input parameter type of the load overload"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md
    kind: related
    condition: "Input parameter type of the store overload"
  - path: docs/zh/api/SIMD-API/c_api/scalar_compute/scalar_load/asc_load_dev.md
    kind: configures
    condition: "The load overload takes effect on this scalar load interface"
  - path: docs/zh/api/SIMD-API/c_api/scalar_compute/scalar_store/asc_store_dev.md
    kind: configures
    condition: "The store overload takes effect on this scalar store interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md
    kind: related
    condition: "The input parameter of this interface carries l2_cache_mode, which is mutually independent of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md
    kind: related
    condition: "The input parameter of this API carries l2_cache_mode and is mutually independent from this API"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: related
    condition: "The input parameter of this API carries l2_cache_mode and is mutually independent from this API"
version: ">=9.2.0"
version_changes: []
---
# asc_set_scalar_cache_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:53:53.920Z pushedAt=2026-11-18T08:15:34.202Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is: `"c_api/cache_ctrl/cache_ctrl.h"`.

Configure the L2 Cache management policy when the scalar unit accesses Global Memory:

- When [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) is passed in, set the scalar load policy.
- When [asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md) is passed in, set the scalar store policy.

After the setting, subsequent GM reads and writes on the scalar path access the L2 Cache according to this policy until this interface is called again to modify it.

## Prototype

```c
// Configure the L2 Cache management policy for the scalar load interface.
__aicore__ inline void asc_set_scalar_cache_mode(asc_load_l2_cache_mode l2_cache_mode)
// Configure the L2 Cache management policy for the scalar store interface.
__aicore__ inline void asc_set_scalar_cache_mode(asc_store_l2_cache_mode l2_cache_mode)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| l2_cache_mode | Input | L2 Cache management policy:<br>&bull; The overload for configuring the L2 Cache management policy of the scalar load interface passes in the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) enumeration value.<br>&bull; The overload for configuring the L2 Cache management policy of the scalar store interface passes in the [asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md) enumeration value. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The two function prototypes of this API act on the scalar load and scalar store paths respectively. The scalar-related operation APIs affected by each are as follows:
    - For the overload that configures the L2 Cache management policy of the scalar load API, the APIs within the scope are:
        - [asc_load_dev](../scalar_compute/scalar_load/asc_load_dev.md)
        - [asc_datacache_preload](asc_datacache_preload.md)
        - [asc_icache_preload](asc_icache_preload.md)
    - For the overload that configures the L2 Cache management policy of the scalar store API, the APIs within the scope are:
        - [asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md)
        - [scalar atomic operations](../atomic/scalar_atomic/scalar_atomic.md)
- This API only affects the default L2 Cache policy of the **scalar memory access path**. It does not change the `l2_cache_mode` input parameter of vector data movement APIs (such as [asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md), [asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md), [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md), etc.). The two are mutually independent. For L2 Cache optimization of large tile data, it is recommended to configure it through the input parameters of vector data movement APIs. For details, see [L2 Cache Mode best practice example](../../../../../../examples/02_simd_c_api/02_features/00_data_movement/00_set_l2_cache_mode).

## Examples

After saving the code as `example.asc`, you can compile and run it using the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (whose corresponding NPU architecture is `dav-3510`) as an example, the compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

- Scenario A: The scalar load uses `NORMAL_FIRST_VICTIM` to read the GM tiling table.<a id="asc_set_scalar_cache_mode_scenario_a"></a>

  Dynamic tiling is stored in GM: `tiling_gm[0]` is `tile_count`, followed by the `length` and `offset` of each tile in sequence. The same tiling region is read repeatedly within the loop, which is suitable for enabling L2 reuse.

    ```cpp
    #include <array>
    #include <cstdint>
    #include <iostream>

    #include "c_api/asc_simd.h"
    #include "acl/acl.h"

    namespace {
    constexpr uint32_t TILE_COUNT = 3;
    constexpr uint32_t TILING_ELEMENTS = 1 + 2 * TILE_COUNT;
    constexpr uint32_t READ_PASSES = 2;

    __global__ __vector__ void ScalarLoadCacheModeKernel(__gm__ int32_t* tiling, __gm__ int32_t* output)
    {
        asc_init();
        const asc_load_l2_cache_mode old_mode = asc_get_scalar_load_cache_mode();
        asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);

        // Repeatedly read the tiling table in GM. The scalar load policy takes effect here.
        const int32_t tile_count = asc_load_dev(tiling);
        int32_t sum = 0;
        for (uint32_t pass = 0; pass < READ_PASSES; ++pass) {
            for (int32_t tile = 0; tile < tile_count; ++tile) {
                const int32_t length = asc_load_dev(tiling + 1 + tile);
                const int32_t offset = asc_load_dev(tiling + 1 + tile_count + tile);
                sum += length + offset;
            }
        }

        asc_set_scalar_cache_mode(old_mode);
        asc_store_dev(output, sum);
        asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    }
    } // namespace

    int main()
    {
        const std::array<int32_t, TILING_ELEMENTS> tiling = {3, 1, 2, 3, 0, 16, 32};
        constexpr int32_t expected = 108;
        int32_t output = 0;
        int32_t* tiling_device = nullptr;
        int32_t* output_device = nullptr;

        aclInit(nullptr);
        aclrtSetDevice(0);
        aclrtMalloc(reinterpret_cast<void**>(&tiling_device), tiling.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
        aclrtMalloc(reinterpret_cast<void**>(&output_device), sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
        aclrtMemcpy(tiling_device, tiling.size() * sizeof(int32_t), tiling.data(), tiling.size() * sizeof(int32_t), ACL_MEMCPY_HOST_TO_DEVICE);
        ScalarLoadCacheModeKernel<<<1, 0>>>(tiling_device, output_device);
        const aclError ret = aclrtSynchronizeDevice();
        aclrtMemcpy(&output, sizeof(int32_t), output_device, sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

        const bool passed = ret == ACL_SUCCESS && output == expected;
        std::cout << (passed ? "[Success] scalar load cache mode passed." : "[Failed] scalar load cache mode failed.") << std::endl;
        aclrtFree(tiling_device);
        aclrtFree(output_device);
        aclrtResetDevice(0);
        aclFinalize();
        return passed ? 0 : 1;
    }
    ```

- Scenario B: The scalar store uses `NOTALLOC_CLEAN` to update the GM shared counter.<a id="asc_set_scalar_cache_mode_scenario_b"></a>

  Multiple cores update the same GM counter through scalar atomic addition. Temporarily disable L2 allocation on the scalar store path to avoid unnecessary Cache Line allocation for control fields.

    ```cpp
    #include <cstdint>
    #include <iostream>

    #include "c_api/asc_simd.h"
    #include "acl/acl.h"

    namespace {
    constexpr uint32_t BLOCK_COUNT = 4;

    __global__ __vector__ void ScalarStoreCacheModeKernel(__gm__ uint32_t* counter)
    {
        asc_init();
        const asc_store_l2_cache_mode old_mode = asc_get_scalar_store_cache_mode();
        asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NOTALLOC_CLEAN);

        // Flush the DCache before the atomic write to GM to prevent dirty data from affecting the shared counter.
        asc_dcci_entire_all();
        asc_atomic_add(counter, 1U);

        asc_set_scalar_cache_mode(old_mode);
        asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    }
    } // namespace

    int main()
    {
        uint32_t counter = 0;
        uint32_t* counter_device = nullptr;

        aclInit(nullptr);
        aclrtSetDevice(0);
        aclrtMalloc(reinterpret_cast<void**>(&counter_device), sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST);
        aclrtMemcpy(counter_device, sizeof(uint32_t), &counter, sizeof(uint32_t), ACL_MEMCPY_HOST_TO_DEVICE);
        ScalarStoreCacheModeKernel<<<BLOCK_COUNT, 0>>>(counter_device);
        const aclError ret = aclrtSynchronizeDevice();
        aclrtMemcpy(&counter, sizeof(uint32_t), counter_device, sizeof(uint32_t), ACL_MEMCPY_DEVICE_TO_HOST);

        const bool passed = ret == ACL_SUCCESS && counter == BLOCK_COUNT;
        std::cout << (passed ? "[Success] scalar store cache mode passed." : "[Failed] scalar store cache mode failed.") << std::endl;
        aclrtFree(counter_device);
        aclrtResetDevice(0);
        aclFinalize();
        return passed ? 0 : 1;
    }
    ```
