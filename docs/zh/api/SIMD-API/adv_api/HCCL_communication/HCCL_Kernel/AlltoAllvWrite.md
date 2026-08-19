# AlltoAllvWrite

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AlltoAllvWrite_res.md#id1 -->

## 功能说明

集合通信AlltoAllvWrite的任务下发接口，返回该任务的标识handleId给用户。

AlltoAllvWrite的功能为：通信域内的卡互相发送和接收数据，并且定制每张卡给其它卡发送的数据量和从其它卡接收的数据量，以及定制发送和接收的数据在内存中的偏移。结合原型中的参数，描述接口功能，具体为：本卡发送地址偏移为sendOffsets\[i\]字节且大小为sendSizes\[i\]字节的数据给第i张卡，remoteWinOffset表示本卡数据写入对端window的偏移，localDataSize表示各张对端卡写入本卡的数据大小。注意：这里的偏移和数据量均为字节数。

![](../../../../figures/251208103727744.png)

## 函数原型

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAllvWrite(GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| commit | 输入 | bool类型。参数取值如下：<br>true：在调用Prepare接口时，Commit同步通知服务端可以执行该通信任务。<br>false：在调用Prepare接口时，不通知服务端执行该通信任务。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| usrIn | 输入 | 源数据buffer地址。 |
| sendOffsets | 输入 | 待发送的每个分片的偏移，以字节为单位。 |
| sendSizes | 输入 | 待发送的每个分片的数据大小，以字节为单位。 |
| remoteWinOffset | 输入 | 本卡数据写入对端window的偏移，以字节为单位。 |
| localDataSize | 输入 | 各张对端卡写入本卡的数据大小，以字节为单位。 |

## 返回值说明

返回该任务的标识handleId，handleId大于等于0。调用失败时，返回 -1。

## 约束说明

-   调用本接口前确保已调用过[InitV2](InitV2.md)和[SetCcTilingV2](SetCcTilingV2.md)接口。
-   若HCCL对象的[模板参数config](HCCL_template_params.md#hccl-template-params)未指定下发通信任务的核，则该接口只能在AIC核或者AIV核两者之一上调用。若HCCL对象的[模板参数config](HCCL_template_params.md#hccl-template-params)指定了下发通信任务的核，则该接口可以在AIC核和AIV核上同时调用，接口内部根据指定的核的类型，在对应的AIC核、AIV核二者之一下发该通信任务。
-   一个通信域内，所有Prepare接口和InterHcclGroupSync接口的总调用次数不能超过63。
<!-- npu="950" id7 -->
-   对于Ascend 950PR/Ascend 950DT，通信服务端为CCU时，默认仅支持FullMesh算法，并且单次最大通信数据量不能超过256M。
<!-- end id7 -->

## 调用示例

```
extern "C" __global__ __aicore__ void alltoallvwrite_custom(
    GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    REGISTER_TILING_DEFAULT(
        AllToAllVWriteCustomTilingData); // AllToAllVWriteCustomTilingData为对应算子头文件定义的结构体
    GET_TILING_DATA_WITH_STRUCT(AllToAllVWriteCustomTilingData, tilingData, tilingGM);

    auto&& cfg = tilingData.param;
    uint32_t M = cfg.M;
    uint32_t K = cfg.K;
    uint32_t dataType = cfg.dataType;
    uint32_t dataTypeSize = cfg.dataTypeSize;

    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_2);
    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    GM_ADDR context = GetHcclContext<HCCL_GROUP_ID_0>();
    hccl.InitV2(context, &tilingData);
    hccl.SetCcTilingV2(offsetof(AllToAllVCustomV3TilingData, mc2CcTiling));
    uint32_t rankDim = hccl.GetRankDim();
    uint32_t rankId = hccl.GetRankId();

    uint64_t perRankDataSize_ = M * K * dataTypeSize / rankDim;
    GM_ADDR sendSizeGM_ = workspaceGM;
    GM_ADDR sendOffsetGM_ = sendSizeGM_ + rankDim * sizeof(uint64_t) * 2;
    __gm__ uint64_t* sendSizes = reinterpret_cast<__gm__ uint64_t*>(sendSizeGM_);
    __gm__ uint64_t* sendOffsets = reinterpret_cast<__gm__ uint64_t*>(sendOffsetGM_);
    // 当前ccu通信都是双die，所以sendSize和sendOffset需要等分切成die0和die1的数据
    for (uint32_t i = 0U; i < rankDim; i++) { 
        sendSizes[i] = perRankDataSize_ / 2;
        sendSizes[i + rankDim] = perRankDataSize_ - perRankDataSize_ / 2;
        sendOffsets[i] = i * perRankDataSize_;
        sendOffsets[i + rankDim] = i * perRankDataSize_ + sendSizes[i];
    }
    uint64_t remoteWinOffset = rankId * perRankDataSize_;
    uint64_t localDataSize = perRankDataSize_;
    if (TILING_KEY_IS(1000UL)) {
        if ASCEND_IS_AIV {
            AscendC::HcclHandle handleId = -1;
            handleId = hccl.AlltoAllvWrite<true>(xGM, sendOffsetGM_, sendSizeGM_, remoteWinOffset, localDataSize);
            hccl.Wait(handleId);
            AscendC::SyncAll<true>(); // 全AIV核同步，防止0核执行过快，提前调用hccl.Finalize()接口，导致其他核Wait卡死
            hccl.Finalize();
        }
    }
}
```
