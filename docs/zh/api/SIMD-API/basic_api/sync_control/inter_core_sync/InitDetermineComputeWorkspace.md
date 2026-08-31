# InitDetermineComputeWorkspace<a name="ZH-CN_TOPIC_0000001723825540"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/InitDetermineComputeWorkspace_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_determine_compute_sync_intf.h"`。

InitDetermineComputeWorkspace是基于核间顺序执行的确定性计算的初始化配置接口，能够初始化GM共享内存的值，完成初始化后才可以调用[WaitPreBlock](WaitPreBlock.md)和[NotifyNextBlock](NotifyNextBlock.md)。以上三个接口共同完成基于核间顺序执行的确定性计算，确定性计算的具体含义请参考[确定性计算](../../atomic_operations/key_features.md)。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void InitDetermineComputeWorkspace(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| :--- | :--- | :--- |
| gmWorkspace | 输入 | 临时空间，初始化核间同步的共享内存，类型为GlobalTensor。 |
| ubWorkspace | 输入 | 临时空间，用于操作gmWorkspace，类型为LocalTensor。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- gmWorkspace申请的空间最少要求为：GetBlockNum()*32Bytes，ubWorkspace申请的空间最少要求为：GetBlockNum()*32+32Bytes。
- 与IBSet/SyncAll不同，该接口输入参数gmWorkspace缓存的值不需要初始化为0。
- 使用该接口进行多核控制时，算子调用时指定的逻辑AI Core核数numBlocks必须保证不大于实际运行该算子的AI处理器核数，否则框架进行多轮调度时会插入异常同步，导致核函数（Kernel）“卡死”现象。

## 调用示例<a name="section177231425115410"></a>

```cpp
    // 初始化GM共享内存的同步状态，必须在核函数（Kernel）开始时首先调用。
    AscendC::InitDetermineComputeWorkspace(gmWorkspace, ubWorkspace);

    for(int64_t i = 0; i < tileNum; i++) {
        AscendC::LocalTensor<T> srcLocal = que.AllocTensor<T>();
        AscendC::DataCopy(srcLocal, srcGlobal[i * tileCount], tileCount);

        // 等待前序核（blockIdx-1）完成操作。
        AscendC::WaitPreBlock(gmWorkspace, ubWorkspace);
        
        // 开启原子累加。
        AscendC::SetAtomicAdd<T>();
        AscendC::DataCopy(dstGlobal[i * tileCount], srcLocal, tileCount);
        AscendC::DisableDmaAtomic();
        
        // 通知后序核（blockIdx+1）当前核已完成。
        AscendC::NotifyNextBlock(gmWorkspace, ubWorkspace);
        que.FreeTensor(srcLocal);
    }
```

完整样例请参考[sequential\_block\_sync示例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/sequential_block_sync)。
