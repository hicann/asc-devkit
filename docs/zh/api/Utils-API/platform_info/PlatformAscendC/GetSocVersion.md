# GetSocVersion<a name="ZH-CN_TOPIC_0000002078653638"></a>

## 功能说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

获取当前硬件平台版本型号。

## 函数原型<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
SocVersion GetSocVersion(void) const
```

## 参数说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

无

## 返回值<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

当前硬件平台版本型号的枚举类。该枚举类和AI处理器型号的对应关系请通过CANN软件安装后文件存储路径下include/tiling/platform/platform\_ascendc.h头文件获取。

AI处理器的型号请通过如下方式获取：

<!-- npu="910b,910,310p,310b" id1 -->
-   针对如下产品：在安装AI处理器的服务器执行`npu-smi info`命令进行查询，获取**Name**信息。实际配置值为AscendName，例如**Name**取值为xxxyy，实际配置值为Ascendxxxyy。

    <!-- npu="910b" id2 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id2 -->

    <!-- npu="310b" id3 -->
    Atlas 200I/500 A2 推理产品
    <!-- end id3 -->

    <!-- npu="310p" id4 -->
    Atlas 推理系列产品
    <!-- end id4 -->

    <!-- npu="910" id5 -->
    Atlas 训练系列产品
    <!-- end id5 -->
<!-- end id1 -->

<!-- npu="A3" id6 -->
-   针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，在安装AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取**Chip Name**和**NPU Name**信息，实际配置值为`Chip Name_NPU Name`。例如**Chip Name**取值为Ascendxxx，**NPU Name**取值为1234，实际配置值为Ascendxxx_1234。其中：
    -   `id`：设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。
    -   `chip_id`：芯片id，通过`npu-smi info -m`命令查出的Chip ID即为芯片id，命令中表示为`<chip_id>`。
<!-- end id6 -->

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，在安装AI处理器的服务器执行`npu-smi info -t board -i <id>`命令进行查询，获取**Chip Name**和**NPU Name**信息，实际配置值为`Chip Name_NPU Name`。例如**Chip Name**取值为Ascendxxx，**NPU Name**取值为1234，实际配置值为Ascendxxx_1234。

    其中，`id`为设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。
<!-- end id7 -->

## 约束说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

为了满足算子二进制复用，使用Atlas A3 训练系列产品/Atlas A3 推理系列产品时，调用GetSocVersion接口返回的枚举值与Atlas A2 训练系列产品/Atlas A2 推理系列产品相同；如果需要区分，可使用接口aclrtGetSocName接口。

## 调用示例<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

### 自定义算子工程

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto socVersion = ascendcPlatform.GetSocVersion();
    // 根据所获得的版本型号自行设计Tiling策略
    // ASCENDXXX请替换为实际的版本型号
    if (socVersion == platform_ascendc::SocVersion::ASCENDXXX) {
        // ...
    }
    return ret;
}
```

### Kernel直调

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto socVersion = ascendcPlatform->GetSocVersion();
    // ASCENDXXX请替换为实际的版本型号
    if (socVersion == platform_ascendc::SocVersion::ASCENDXXX) {
        // ...
    }
}
```
