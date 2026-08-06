# ExtendCfgInfo<a name="ZH-CN_TOPIC_0000002114097093"></a>

## 功能说明<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

用于扩展算子相关参数配置，提供更灵活的参数配置能力。

## 函数原型<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpAICoreConfig &OpAICoreConfig::ExtendCfgInfo(const char *key, const char *value) 
```

## 参数说明<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

**表1**  参数说明

| 参数名 | 描述 |
|:-------|:-----|
| key | 配置项，如"aclnnSupport.value"。 |
| value | 配置项key对应的取值，如"aclnnSupport.value"，可以填充"support_aclnn"或者"aclnn_only"。 |

ExtendCfgInfo支持的参数如下表：

**表2**  ExtendCfgInfo支持配置的参数

| 参数名 | 功能介绍 |
|:-------|:---------|
| aclnnSupport.value | - support_aclnn：此模式下，静态Shape场景中该算子通过模型下沉执行，动态Shape场景则在Host侧调用fallback函数下发算子。如果调用了[EnableFallBack](../OpDef/EnableFallBack.md)则默认采用该模式。<br>- aclnn_only：此模式下，动静态Shape场景中该算子均以fallback形式下发。不建议用户使用该模式，后续版本待废弃。<br>关于fallback下发算子的详细介绍请参考[《图引擎开发指南》](https://www.hiascend.com/document/redirect/CannCommunityGraphguide)。 |

support_aclnn使用示例：

```cpp
// 如下为动态Shape场景的示例
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true)   // 动态Shape场景需要设置DynamicShapeSupportFlag为true
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);

// 如下为静态Shape场景的示例
OpAICoreConfig aicore_config;
aicore_config.DynamicCompileStaticFlag(true)  // 静态Shape场景需要设置DynamicCompileStaticFlag为true
             .ExtendCfgInfo("aclnnSupport.value", "support_aclnn");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

aclnn_only使用示例：

```cpp
// 如下为动态Shape场景的示例
OpAICoreConfig aicore_config;
aicore_config.DynamicShapeSupportFlag(true) // 动态Shape场景需要设置DynamicShapeSupportFlag为true
			 .ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
// 如下为静态Shape场景的示例
OpAICoreConfig aicore_config;              // 无需配置DynamicCompileStaticFlag/DynamicShapeSupportFlag，算子均以fallback形式下发，即动态Shape模型的下发方式
aicore_config.ExtendCfgInfo("aclnnSupport.value", "aclnn_only");
this->AICore().AddConfig("ascendxxx", aicore_config);
```

## 返回值说明<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpAICoreConfig算子定义，请参考[OpAICoreConfig](OpAICoreConfig.md)。

## 约束说明<a name="zh-cn_topic_0000001600884192_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

无
