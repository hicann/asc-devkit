# CheckOpResSufficient

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/CheckOpResSufficient_res.md#id1 -->

## 功能说明

根据MC2通信参数，对指定通信算法所需的CCU通信资源进行预校验。该接口只做资源探测，不返回通信资源上下文，也不启动通信服务端。

## 函数原型

```cpp
extern HcclResult __attribute__((visibility("default"))) CheckOpResSufficient(
    HcclComm comm, uint8_t ccType, void* ccArgs)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| comm | 输入 | 已初始化的通信域句柄。 |
| ccType | 输入 | 通信任务类型，取值为HcclCMDType枚举值，当前支持AllGather、AllReduce、ReduceScatter、AlltoAll和AlltoAllV。 |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)创建并通过Mc2SetCc*接口填充的MC2通信参数对象。 |

## 返回值说明

返回HcclResult类型的值，具体取值参考下表。

**表2**  返回值说明

| 返回值 | 描述 |
| --- | --- |
| HCCL_SUCCESS | 资源充足，或当前场景不执行真实资源探测。 |
| HCCL_E_PARA | 参数格式非法。 |
| HCCL_E_PTR | 空指针错误。 |
| HCCL_E_NOT_SUPPORT | 设备、通信任务或通信引擎不支持。 |
| HCCL_E_ALG_NOT_SUPPORTED | 算法未注册。 |
| HCCL_E_RES_NOT_SUFFICIENT | 所需通信资源不足。 |

## 约束说明

-   ccArgs必须由[Mc2GetCcArgs](Mc2GetCcArgs.md)创建。
-   仅当通信域rank数大于1且通信引擎为CCU_SCHED时执行真实资源探测；其他场景直接返回HCCL_SUCCESS。
-   AICPU_TS和CCU_MS路径当前不执行真实资源探测。
-   本接口不替代[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)，资源预检查通过后仍需正式申请通信资源上下文。

## 调用示例

```cpp
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLTOALL);

HcclResult checkRet = CheckOpResSufficient(comm, ccType, ccArgs);
if (checkRet == HCCL_E_RES_NOT_SUFFICIENT) {
    // 资源不足处理，例如回退到其他通信路径或提示用户
} else if (checkRet != HCCL_SUCCESS) {
    // 参数或内部异常处理
}

Mc2Result acquireRet = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
```
