# WriteGmByPassDCache\(ISASI\)（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/WriteGmByPassDCache_ISASI_res.md#id1 -->

## 功能说明

**该接口已废弃，请使用[WriteGmBypassDCache\(ISASI\)](../scalar_compute/WriteGmBypassDCache_ISASI.md)接口替代。**

头文件路径为：`"basic_api/kernel_operator_scalar_intf.h"`。

不经过DCache向GM地址上写数据。
<!-- npu="A3,910b" id8 -->
> [!CAUTION]注意
> 针对[NPU架构2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，接口能否将value成功写入GM还与目标地址addr有关，具体请参见[约束说明](#约束说明)。
<!-- end id8 -->

使用场景：

- 当多个核写入的数据落在同一条Cache Line内时，经过DCache的读写将以64B为粒度，可能引发多核数据随机覆盖问题（参考[DataCacheCleanAndInvalid调用示例3](../cache_control/DataCacheCleanAndInvalid.md#example3_multi_core)）。使用该接口不经过DCache直接按操作数大小写GM，可避免此问题。
- 经过DCache写GM时可能导致多核间的数据不一致问题（详细原因请参考[Cache写策略与Cache一致性问题](../cache_control/system_cache_overview.md#zh-cn_topic_0000002583420201_section053731716357)），使用该接口不经过DCache直接向GM写数据，可避免此问题。

由于WriteGmByPassDCache接口向GM写数据时不经过DCache，因此开发者需要考虑如下场景：
- 当GM上地址addr已经在DCache缓存并且其对应的Cache Line标记为“脏”（dirty，表示该数据已被修改但尚未写回到GM）时，开发者应该在WriteGmByPassDCache接口之前调用[DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md)接口将addr对应的Cache Line立即写回GM，否则WriteGmByPassDCache接口写入addr的数据后续可能被DCache写回的脏数据覆盖。
- DataCacheCleanAndInvalid接口与WriteGmByPassDCache接口向GM写数据时硬件不能保证两个接口的执行顺序，因此开发者应该在WriteGmByPassDCache接口之前调用[DataSyncBarrier](../sync_control/intra_core_sync/DataSyncBarrier_ISASI.md)接口对这两个接口进行同步，否则WriteGmByPassDCache接口写入addr的数据依然可能被DCache写回的脏数据覆盖。

根据以上的描述，为了简化编程（开发者无需关心addr是否在DCache缓存以及是否被标记为“脏”），建议开发者在使用WriteGmByPassDCache接口时采用如下代码片段：

```cpp
AscendC::GlobalTensor<T> global; // global为addr对应的GlobalTensor
AscendC::DataCacheCleanAndInvalid<T, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
// 保证WriteGmByPassDCache接口向addr写入value之前，DCache中的脏数据已经写回GM。
AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
AscendC::WriteGmByPassDCache<T>(addr, value);
```

## 函数原型

```cpp
template <typename T>
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 操作数的数据类型。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 含义 |
|--------|-----------|------|
| addr | 输入 | 目标GM地址。 |
| value | 输入 | 待写入目标数据。 |

## 数据类型

支持的数据类型为int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。

## 返回值说明

无

## 约束说明

<!-- npu="A3,910b" id9 -->
针对[NPU架构2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，接口是否执行写入取决于目标地址addr在当前128字节对齐区间内的偏移。令$\mathrm{offset} = addr \bmod 128$，当$0 \leq \mathrm{offset} < 32$时，写入生效；当$32 \leq \mathrm{offset} < 128$时，不执行写入，目标地址中的数据保持原值。即仅当目标地址位于每个128字节对齐区间的前32字节时，接口才执行写入。

设接口调用前后的目标地址数据分别为$\mathrm{GM}_{\mathrm{before}}(addr)$和$\mathrm{GM}_{\mathrm{after}}(addr)$，则：

  $$
  \mathrm{GM}_{\mathrm{after}}(addr) =
  \begin{cases}
  value, & 0 \leq \mathrm{offset} < 32 \\
  \mathrm{GM}_{\mathrm{before}}(addr), & 32 \leq \mathrm{offset} < 128
  \end{cases}
  $$
<!-- end id9 -->

## 调用示例

```cpp
if (blockIdx == 0) {
    // 先写入被依赖数据，再通过DataSyncBarrier等待DDR访问完成。
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
    // DataSyncBarrier<DDR>阻塞后续GM写，确保上一条GM写对其他核可见。
    AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
    // 最后写入同步标记，block 1读到该标记后即可安全读取srcGm[1]。
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
}

if (blockIdx == 1) {
    while (true) {
        __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
        // 轮询GM第0个元素，等待block 0写入同步标记。
        T flagValue = AscendC::ReadGmByPassDCache<T>(addr);
        if (flagValue == SYNC_FLAG) {
            // DataSyncBarrier保证同步标记之前的srcGm[1]写入已完成。
            T dataValue = AscendC::ReadGmByPassDCache<T>(addr + 1);
            AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
            return;
        }
    }
}
```

完整样例请参考[DataSyncBarrier样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier)。
