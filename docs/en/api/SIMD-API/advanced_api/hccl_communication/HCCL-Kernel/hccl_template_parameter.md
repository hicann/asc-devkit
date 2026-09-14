# HCCL Template Parameters

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:20:00.951Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Template parameters must be passed when creating an **HCCL** object.

## Prototype

The **Hccl** class is defined as follows. For template parameter descriptions, see [Table 1 Hccl class template parameter description](#hccl-template-params).

```
template <HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU, const auto &config = DEFAULT_CFG>
class Hccl;
```

## Parameters

<a id="hccl-template-params"></a>
**Table 1**  Hccl class template parameter description
| Parameter Name | Description |
| --- | --- |
| serverType | Supported server type. **HcclServerType** enumeration type, with the following values.<br>**HCCL_SERVER_TYPE_AICPU**: AI CPU server.<br>**HCCL_SERVER_TYPE_CCU**: CCU server.<br>**HCCL_SERVER_TYPE_END**: Reserved parameter, not supported.<br><br>For Ascend 950PR/Ascend 950DT, currently only supports **HCCL_SERVER_TYPE_CCU**.<br><br>For Atlas A3 training products/Atlas A3 inference products, currently only supports **HCCL_SERVER_TYPE_AICPU**.<br><br>For Atlas A2 training products/Atlas A2 inference products, currently only supports **HCCL_SERVER_TYPE_AICPU**. |
| config | Used to specify the core that delivers tasks to the server. **HcclServerConfig** type, defined in the following code, with the following meanings.<br><br>**type**: Type of the core that delivers tasks to the server. **CoreType** type, with the following possible values:<br>**DEFAULT**: Indicates that neither the AIC core nor the AIV core is specified.<br>**ON_AIV**: Indicates that the AIV core is specified.<br>**ON_AIC**: Indicates that the AIC core is specified.<br><br>**blockId**: ID of the core that delivers tasks to the server.<br><br>Default value **DEFAULT_CFG** = {**CoreType::DEFAULT**, 0}. |

```
enum HcclServerType {
    HCCL_SERVER_TYPE_AICPU = 0,
    HCCL_SERVER_TYPE_CCU,
    HCCL_SERVER_TYPE_END  // Reserved parameter, not supported.
};

struct HcclServerConfig {
    CoreType type;  
    int64_t blockId; 
};

enum class CoreType: uint8_t {
    DEFAULT,  
    ON_AIV,   
    ON_AIC   
};
```

## Return Value

None

## Constraints

None

## Examples

Create an **Hccl** class object by passing the template parameter **config** as follows to specify that the HCCL client sends communication messages to the server only on core 10 of the AIV, instead of specifying the core to run on by calling the [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) API.

```
static constexpr HcclServerConfig HCCL_CFG = {CoreType::ON_AIV, 10};
// Select AICPU as the server.
Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU, HCCL_CFG> hccl;

// Select CCU as the server.
Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU, HCCL_CFG> hccl;
```
