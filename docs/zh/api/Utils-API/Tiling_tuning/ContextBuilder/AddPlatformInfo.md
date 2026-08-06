# AddPlatformInfo<a name="ZH-CN_TOPIC_0000002333452901"></a>

## 功能说明<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section36583473819"></a>

设置硬件平台信息便于用户在算子Tiling函数调测中使用。支持以下两种设置方式：

-   **自动获取当前硬件平台信息**：传入空指针，自动获取当前硬件信息并添加到ContextBuilder类中。
-   **指定硬件平台信息**：传入具体的AI处理器型号，添加对应硬件信息至ContextBuilder类中。

若设置失败，会打印报错信息。关于日志配置和查看，请参考[《环境变量参考》](https://www.hiascend.com/document/redirect/CannCommunityEnvRef)中日志相关环境变量。

## 函数原型<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &AddPlatformInfo(const char* customSocVersion)
```

## 参数说明<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section75395119104"></a>

|参数名|输入/输出|描述|
|------------|------------|-----------|
| customSocVersion     | 输入     | AI处理器型号。   |

## 返回值说明<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section25791320141317"></a>

当前ContextBuilder对象。

## 约束说明<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section19165124931511"></a>

AddPlatformInfo调用后需要通过[BuildTilingContext](BuildTilingContext.md)来构建Tiling的上下文，并传递给Tiling函数来使用。

## 调用示例<a name="zh-cn_topic_0000001867409741_zh-cn_topic_0000001389787297_section320753512363"></a>

```
void AddPlatformInfoDemo(......)
{
    auto holder = context_ascendc::ContextBuilder()
	// ... ... // 增加算子输入输出接口的调用
	.AddPlatformInfo("Ascendxxxyy")
	.BuildTilingContext();
    auto tilingContext = holder->GetContext<gert::TilingContext>();
    // ... ...
}
```

