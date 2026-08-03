# DataSyncBarrier\(ISASI\)<a name="ZH-CN_TOPIC_0000001787873298"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
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


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束，用于解决Scalar的内存依赖。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <MemDsbT arg0>
__aicore__ inline void DataSyncBarrier()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| arg0 | 模板参数，表示需要等待的内存位置，类型为MemDsbT，可取值为：<br>&bull; ALL，等待GM和UB的访问指令（不包括其他内存位置，例如L0C Buffer、L1 Buffer）。<br>&bull; DDR，等待GM访问指令。<br>&bull; UB，等待UB访问指令。<br>&bull; SEQ，等待SEQ访问指令。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

等待的内存位置取值为ALL时，只能等待GM和UB的访问指令，不包括其他内存位置，例如L0C Buffer、L1 Buffer。

## 调用示例<a name="section837496171220"></a>

```cpp
    if (blockIdx == 0) {
        // 先写入被依赖数据，再通过DataSyncBarrier等待GM访问完成。
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

完整样例请参考[DataSyncBarrier接口样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier)。
