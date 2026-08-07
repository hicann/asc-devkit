# aclrtcCompileProg<a name="ZH-CN_TOPIC_0000002433359052"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas 训练系列产品：不支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

编译接口，编译指定的程序。

## 函数原型<a name="section620mcpsimp"></a>

```
aclError aclrtcCompileProg(aclrtcProg prog, int numOptions, const char **options)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|----------|------|
| prog | 输入 | 运行时编译程序的句柄。 |
| numOptions | 输入 | 编译选项数量。 |
| options | 输入 | 编译选项数组，保存具体的编译选项（默认添加-std=c++17）。<br>支持的编译选项可以参考《[毕昇编译器](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/920beta1/compiler/BishengCompiler/atlas_bisheng_10_0001.html)》。 |

## 返回值说明<a name="section640mcpsimp"></a>

aclError为int类型变量，详细说明请参考[RTC错误码](RTC错误码.md)。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

```
aclrtcProg prog;
const char *options[] = {"--npu-arch=dav-2201"};
int numOptions = sizeof(options) / sizeof(options[0]);
aclError result = aclrtcCompileProg(prog, numOptions, options);
```
