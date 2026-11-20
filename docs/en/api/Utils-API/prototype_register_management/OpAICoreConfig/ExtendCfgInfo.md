# ExtendCfgInfo<a name="ZH-CN_TOPIC_0000002114097093"></a>

<!-- md-trans-meta sourceCommit=78baa767400d3af08b5a397cacdcc8b46362de90 translatedAt=2026-11-10T10:42:44.555Z pushedAt=2026-11-13T08:46:49.306Z nl=2 -->

## Description<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Used to extend operator-related parameter configuration, providing more flexible parameter configuration capabilities.

## Prototype<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &OpAICoreConfig::ExtendCfgInfo(const char *key, const char *value) 
```

## Parameters<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

**Table 1**  Parameters

| Parameter | Description |
|:-------|:-----|
| key | Configuration item, for example, "aclnnSupport.value". |
| value | Value corresponding to the configuration item key, for example, "aclnnSupport.value", which can be set to "support_aclnn" or "aclnn_only". |

The parameters supported by ExtendCfgInfo are as follows:

**Table 2** Parameters supported by ExtendCfgInfo

| Parameter | Description |
|:-------|:---------|
| aclnnSupport.value | - support_aclnn: In this mode, the operator is executed through model sinking in static shape scenarios, while in dynamic shape scenarios, the fallback function is called on the host side to dispatch the operator. This mode is used by default if [EnableFallBack](../OpDef/EnableFallBack.md) is called.<br>- aclnn_only: In this mode, the operator is dispatched in fallback form in both dynamic and static shape scenarios. You are not advised to use this mode, as it will be deprecated in later versions.<br>For details about dispatching operators through fallback, see [*Graph Development*](https://gitcode.com/cann/ge/blob/master/docs/zh/user_guides/graph_dev/README.md). |

Example of using support_aclnn:

```cpp
// The following is an example of a dynamic shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true)   // Set DynamicShapeSupportFlag to true in dynamic shape scenarios.
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);

// The following is an example of a static shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicCompileStaticFlag(true)  // Set DynamicCompileStaticFlag to true in static shape scenarios.
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

Example of using aclnn_only:

```cpp
// The following is an example of the dynamic shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true) // In the dynamic shape scenario, set DynamicShapeSupportFlag to true.
			 .ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
// The following is an example of the static shape scenario.
OpAICoreConfig aicore_config;              // There is no need to configure DynamicCompileStaticFlag/DynamicShapeSupportFlag. Operators are delivered in fallback mode, which is the delivery mode of dynamic shape models.
aicore_config.ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

## Return Value<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpAICoreConfig class. For details, see [OpAICoreConfig](OpAICoreConfig.md).

## Constraints<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None
