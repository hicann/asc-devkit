# CheckOpResSufficient

## 功能说明

根据输入的Tiling信息，对指定的通信算法所需资源进行预校验，返回检查结果。

## 函数原型

```
extern HcclResult __attribute__((visibility("default"))) CheckOpResSufficient(
    HcclComm comm, void* stream, void* mc2Tiling)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| comm | 输入 | 初始化后的通信域指针。<br>HcclComm类型的定义可参见[HcclComm](https://gitcode.com/cann/hcomm/blob/master/docs/zh/api_ref/comm_mgr_c/data_type_definition/HcclComm.md)。 |
| stream | 输入 | stream类型为aclrtStream，用于维护一些异步操作的执行顺序，确保按照应用程序中的代码调用顺序在Device上执行。stream创建等管理接口请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。 |
| mc2Tiling | 输入 | MC2算子的Tiling信息，通过[GetTiling](GetTiling.md)接口组装到算子[TilingData结构体](TilingData_struct.md)中。 |

## 返回值说明

返回HcclResult类型的值，具体取值参考下表。

**表2**  返回值说明

| 返回值 | 描述 |
| --- | --- |
| HCCL_SUCCESS | 资源充足。 |
| HCCL_E_PARA | 参数格式非法，如Tiling版本、数量非法等。 |
| HCCL_E_PTR | 空指针错误。 |
| HCCL_E_NOT_SUPPORT | 通信类型、算法路径等不支持。 |
| HCCL_E_ALG_NOT_SUPPORTED | 算法未注册。 |
| HCCL_E_RES_NOT_SUFFICIENT | 所选通信算法所需的通信资源不足。 |

## 约束说明

-   仅支持CCU通信引擎。
-   仅支持rank数量大于1的场景。

## 调用示例

```
#include "adv_api/hccl/hccl_mc2.h"

extern "C" void NnopbaseGetTilingData(void *executor, void **tilingData, uint64_t *dataLen);

int rankId = 0;
int64_t g_hcclBufferSize = 200;
int g_op_expansion_mode = 6;

int func() {
    ...
    int ret;
    HcclComm comm;
    aclrtStream stream;
    ret = aclrtSetDevice(rankId);
    ret = aclrtCreateStream(&stream);
    ...
    HcclCommConfig config;
    HcclCommConfigInit(&config);
    config.hcclDeterministic = 0;
    config.hcclBufferSize = g_hcclBufferSize;
    config.hcclOpExpansionMode = g_op_expansion_mode;
    strncpy(config.hcclCommName, "testGroup", sizeof(config.hcclCommName));
    std::string rankTableFile = getenv("RANK_TABLE_FILE");
    ...
    ret = HcclCommInitClusterInfoConfig(rankTableFile.c_str(), rankId, &config, &comm);
    ...
    aclOpExecutor *executor = nullptr;
    void *mc2Tiling = nullptr;
    uint64_t tilingLen = 0;
    NnopbaseGetTilingData(executor, &mc2Tiling, &tilingLen);
    ...
    HcclResult checkRet = CheckOpResSufficient(comm, stream, mc2Tiling);
    if (checkRet == HCCL_E_RES_NOT_SUFFICIENT) {
        // 资源不足处理
        ...
    } else {
        // 其他错误，错误处理
        ...
    }
    return 0;
}
```
