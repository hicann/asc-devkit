# asc_set_scalar_cache_mode

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cache_ctrl/cache_ctrl.h"`。

配置标量单元访问Global Memory时的L2 Cache管理策略：

- 传入[asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md)时，设置标量读（load）策略。
- 传入[asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md)时，设置标量写（store）策略。

设置后，后续标量路径上的GM读写将按该策略访问L2 Cache，直至再次调用本接口修改。

## 函数原型

```c
// 配置标量load接口的L2 Cache管理策略
__aicore__ inline void asc_set_scalar_cache_mode(asc_load_l2_cache_mode l2_cache_mode)
// 配置标量store接口的L2 Cache管理策略
__aicore__ inline void asc_set_scalar_cache_mode(asc_store_l2_cache_mode l2_cache_mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| l2_cache_mode | 输入 | L2 Cache管理策略：<br>&bull; 配置标量load接口的L2 Cache管理策略重载传入[asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md)枚举值。<br>&bull; 配置标量store接口的L2 Cache管理策略重载传入[asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md)枚举值。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口的两个函数原型分别作用于标量load与标量store路径，其各自影响的标量相关操作接口如下：
    - 对于配置标量load接口的L2 Cache管理策略重载，作用范围内的接口有：
        - [asc_load_dev](../scalar_compute/scalar_load/asc_load_dev.md)
        - [asc_datacache_preload](asc_datacache_preload.md)
        - [asc_icache_preload](asc_icache_preload.md)
    - 对于配置标量store接口的L2 Cache管理策略重载，作用范围内的接口有：
        - [asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md)
        - [标量原子操作](../atomic/scalar_atomic/scalar_atomic.md)
- 本接口仅作用于**标量访存路径**的L2 Cache默认策略，不会改变向量搬运类接口（如[asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)、[asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md)、[asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md)等）入参中的`l2_cache_mode`，二者相互独立。针对大块tile数据的L2 Cache优化，建议通过向量搬运类接口的入参配置，具体可参见[L2 Cache Mode最佳实践样例](../../../../../../examples/02_simd_c_api/02_features/00_data_movement/00_set_l2_cache_mode)。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

<!-- end id8 -->

- 场景A：标量load使用`NORMAL_FIRST_VICTIM`读取GM tiling表。<a id="asc_set_scalar_cache_mode_scenario_a"></a>

  动态tiling存放在GM上：`tiling_gm[0]`为`tile_count`，后续依次为各tile的`length`与`offset`。循环内会重复读取同一tiling区域，适合启用L2复用。

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

        // 重复读取GM中的tiling表，标量load策略在此生效。
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

- 场景B：标量store使用`NOTALLOC_CLEAN`更新GM共享计数器。<a id="asc_set_scalar_cache_mode_scenario_b"></a>

  多个核通过标量原子加更新同一GM计数器，临时关闭标量写路径的L2分配，避免对控制字段产生不必要的Cache Line分配。

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

        // 原子写GM前刷新DCache，避免脏数据影响共享计数器。
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
