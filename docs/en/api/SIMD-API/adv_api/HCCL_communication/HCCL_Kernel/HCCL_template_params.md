# HCCL Template Parameters

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:52:48.313Z pushedAt=2026-10-27T11:44:04.456Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/HCCL_template_params_res.md#id1 -->

## Description

Template parameters must be passed in when creating an **HCCL** object.

## Prototype

The **Hccl** class is defined as follows. For template parameter descriptions, see [Table 1 Hccl class template parameter descriptions](#hccl-template-params).

```
template <HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU, const auto& config = DEFAULT_CFG>
class Hccl;
```

## Parameters

**Table 1** Hccl class template parameters

<a name="hccl-template-params"></a>

| Parameter | Description |
| --- | --- |
| serverType | Supported server type. **HcclServerType** enumeration type, with the following values.<br>**HCCL_SERVER_TYPE_AICPU**: AI CPU server.<br>**HCCL_SERVER_TYPE_CCU**: CCU server.<br>**HCCL_SERVER_TYPE_END**: Reserved parameter. Not supported for use.<!-- npu="950" id7 --><br><br>For Ascend 950PR&950DT products, only **HCCL_SERVER_TYPE_CCU** is currently supported.<!-- end id7 --><!-- npu="A3" id8 --><br><br>For Atlas A3 products, only **HCCL_SERVER_TYPE_AICPU** is currently supported.<!-- end id8 --><!-- npu="910b" id9 --><br><br>For Atlas A2 products, only **HCCL_SERVER_TYPE_AICPU** is currently supported.<!-- end id9 --> |
| config | Used to specify the core that delivers tasks to the server. **HcclServerConfig** type, defined as shown in the following code, with the following meanings.<br><br>**type**: Type of the core that delivers tasks to the server. **CoreType** type, with the following possible values:<br>**DEFAULT**: Indicates that neither the AIC core nor the AIV core is specified.<br>**ON_AIV**: Indicates that the AIV core is specified.<br>**ON_AIC**: Indicates that the AIC core is specified.<br><br>**blockId**: ID of the core that delivers tasks to the server.<br><br>Default value DEFAULT_CFG = {CoreType::DEFAULT, 0}. |

```
enum HcclServerType {
    HCCL_SERVER_TYPE_AICPU = 0,
    HCCL_SERVER_TYPE_CCU,
    HCCL_SERVER_TYPE_END // Reserved parameter. Not supported for use.
};

struct HcclServerConfig {
    CoreType type;
    int64_t blockId;
};

enum class CoreType : uint8_t { DEFAULT, ON_AIV, ON_AIC };
```

## Return Value

None

## Constraints

None

## Examples

Create an **Hccl** class object by passing the template parameter **config** as follows, specifying that the HCCL client sends communication messages to the server only on core 10 of the AIV, instead of specifying the core to run on by calling the [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) API.

```
static constexpr HcclServerConfig HCCL_CFG = {CoreType::ON_AIV, 10};
// Select AICPU as the server.
Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU, HCCL_CFG> hccl;

// Select CCU as the server.
Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU, HCCL_CFG> hccl;
```
