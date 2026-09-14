# ExtendCfgInfo<a name="ZH-CN_TOPIC_0000002114097093"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T10:00:07.846Z pushedAt=2026-09-11T01:53:15.091Z -->

## Description<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Extends operator-related parameter configuration to provide more flexible parameter configuration capabilities.

## Prototype<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &OpAICoreConfig::ExtendCfgInfo(const char *key, const char *value) 
```

## Parameters<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

**Table 1** Parameters

| Parameter | Description |
|:-------|:-----|
| key | Configuration item, for example, "aclnnSupport.value". |
| value | Value corresponding to the configuration item key, for example, "aclnnSupport.value", which can be set to "support_aclnn" or "aclnn_only". |

The parameters supported by ExtendCfgInfo are as follows:

**Table 2** Parameters supported by ExtendCfgInfo

| Parameter | Description |
|:-------|:---------|
| aclnnSupport.value | - support_aclnn: In this mode, the operator is executed through model sinking in static shape scenarios, while in dynamic shape scenarios, the fallback function is called on the host side to deliver the operator. If [EnableFallBack](../OpDef/EnableFallBack.md) is called, this mode is used by default.<br>- aclnn_only: In this mode, the operator is delivered in fallback form in both dynamic and static shape scenarios. Users are not advised to use this mode, as it will be deprecated in later versions.<br>For details about delivering operators through fallback, see [Graph Engine Development Guide](https://www.hiascend.com/document/redirect/CannCommunityGraphguide). |

Example of using support_aclnn:

```cpp
// The following is an example of a dynamic shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true)   // Set DynamicShapeSupportFlag to true in dynamic shape scenarios.
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);

// The following is an example of a static shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicCompileStaticFlag(true)  // In static shape scenarios, set DynamicCompileStaticFlag to true.
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

Example of using aclnn_only:

```cpp
// The following is an example of a dynamic shape scenario.
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true) // In dynamic shape scenarios, set DynamicShapeSupportFlag to true.
			 .ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
// The following is an example of a static shape scenario.
OpAICoreConfig aicore_config;              // There is no need to configure DynamicCompileStaticFlag/DynamicShapeSupportFlag. Operators are all delivered in fallback mode, that is, the delivery mode of dynamic shape models.
aicore_config.ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

## Return Value<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

Definition of the **OpAICoreConfig** operator. See [OpAICoreConfig](OpAICoreConfig.md).

## Constraints<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

