# SetMadType

## 功能说明

设置是否开启HF32模式。**当前版本暂不支持。**

<!-- npu="950" id1 -->
对于Ascend 950PR/Ascend 950DT，本接口可用于设置是否开启HF32模式（**当前版本暂不支持**）、设置是否开启MxMatmul场景。在MxMatmul场景，必须调用该接口并配置为开启MxMatmul场景，从而保证该场景下正确计算并返回Tiling参数。
<!-- end id1 -->

## 函数原型

```
int32_t SetMadType(MatrixMadType madType)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| madType | 输入 | 设置Matmul模式。MatrixMadType类型，定义如下方代码所示，其中参数的含义为：<br><br>MatrixMadType::NORMAL：普通模式，即非HF32模式、非MxMatmul场景。<br><br>MatrixMadType::HF32：开启HF32模式。<br><br>MatrixMadType::MXMODE：开启MxMatmul场景。 |

```
enum class MatrixMadType : int32_t {
    NORMAL = 0,
    HF32 = 1,
    MXMODE = 2,
};
```

## 返回值说明

-1表示设置失败； 0表示设置成功。

## 约束说明

无

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetTraverse(MatrixTraverse::FIRSTM); // 设置遍历方式
tiling.SetMadType(MatrixMadType::MXMODE);   // 开启MxMatmul场景
```
