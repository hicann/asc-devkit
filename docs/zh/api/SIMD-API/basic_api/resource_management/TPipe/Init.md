# Init<a name="ZH-CN_TOPIC_0000001717287036"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_tpipe.h"`。

用于内存和同步流水事件EventID的初始化。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void Init()
```

## 约束说明<a name="section633mcpsimp"></a>

-   Tpipe对象复用时，要与[Destroy](Destroy.md)接口成对使用，每次复用需要先Destroy释放资源，再调用init重新初始化。
-   Tpipe构造函数内部已调用Init，无需在Tpipe对象初始化后再额外调用。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>


```cpp
// 实例化一个浮点型的自定义算子对象
KernelTPipeInit<float> op;
uint32_t srcSize = 128;

// 构造一个Tpipe对象
AscendC::TPipe tpipe;

// 第一次执行流程
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // 重复使用需要先Destory，再进行Init

// 第二次执行流程：复用算子对象和tpipe管道对象
tpipe.Init();
op.Init(x, z, srcSize, &tpipe);
op.Process();
tpipe.Destroy();  // 销毁tpipe管道资源
```
