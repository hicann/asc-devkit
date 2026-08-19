# HCCL模板参数

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/HCCL_template_params_res.md#id1 -->

## 功能说明

创建HCCL对象时需要传入模板参数。

## 函数原型

Hccl类定义如下，模板参数说明见[表1 Hccl类模板参数说明](#hccl-template-params)。

```
template <HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU, const auto& config = DEFAULT_CFG>
class Hccl;
```

## 参数说明

**表1**  Hccl类模板参数说明

<a name="hccl-template-params"></a>
| 参数名称 | 描述 |
| --- | --- |
| serverType | 支持的服务端类型。HcclServerType枚举类型，取值如下。<br>HCCL_SERVER_TYPE_AICPU：AI CPU服务端。<br>HCCL_SERVER_TYPE_CCU：CCU服务端。<br>HCCL_SERVER_TYPE_END：预留参数，不支持使用。<!-- npu="950" id7 --><br><br>对于Ascend 950PR/Ascend 950DT，当前仅支持HCCL_SERVER_TYPE_CCU。<!-- end id7 --><!-- npu="A3" id8 --><br><br>对于Atlas A3 训练系列产品/Atlas A3 推理系列产品，当前仅支持HCCL_SERVER_TYPE_AICPU。<!-- end id8 --><!-- npu="910b" id9 --><br><br>对于Atlas A2 训练系列产品/Atlas A2 推理系列产品，当前仅支持HCCL_SERVER_TYPE_AICPU。<!-- end id9 --> |
| config | 用于指定向服务端下发任务的核。HcclServerConfig类型，定义如下代码所示，含义如下。<br><br>type：向服务端下发任务的核的类型。CoreType类型，可取值为：<br>DEFAULT：表示不指定AIC核或者AIV核。<br>ON_AIV：表示指定为AIV核。<br>ON_AIC：表示指定为AIC核。<br><br>blockId：向服务端下发任务的核的ID。<br><br>默认值DEFAULT_CFG = {CoreType::DEFAULT, 0}。 |

```
enum HcclServerType {
    HCCL_SERVER_TYPE_AICPU = 0,
    HCCL_SERVER_TYPE_CCU,
    HCCL_SERVER_TYPE_END // 预留参数，不支持使用
};

struct HcclServerConfig {
    CoreType type;
    int64_t blockId;
};

enum class CoreType : uint8_t { DEFAULT, ON_AIV, ON_AIC };
```

## 返回值说明

无

## 约束说明

无

## 调用示例

通过如下传入模板参数config的方式创建Hccl类对象，指定HCCL客户端仅在AIV的10号核上发送通信消息给服务端，替代通过调用[GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md)接口的方式指定运行的核。

```
static constexpr HcclServerConfig HCCL_CFG = {CoreType::ON_AIV, 10};
// 选择AICPU作为服务端
Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU, HCCL_CFG> hccl;

// 选择CCU作为服务端
Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU, HCCL_CFG> hccl;
```
