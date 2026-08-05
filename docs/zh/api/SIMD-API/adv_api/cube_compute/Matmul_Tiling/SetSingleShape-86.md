# SetSingleShape

## 功能说明

设置Matmul单核计算的形状singleMIn，singleNIn，singleKIn，单位为元素。

## 函数原型

```
int32_t SetSingleShape(int32_t singleMIn = -1, int32_t singleNIn = -1, int32_t singleKIn = -1)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| singleMIn | 输入 | 设置的singleMIn大小，单位为元素，默认值为-1。-1表示不设置指定的singleMIn，该值由tiling函数自行计算。 |
| singleNIn | 输入 | 设置的singleNIn大小，单位为元素，默认值为-1。-1表示不设置指定的singleNIn，该值由tiling函数自行计算。 |
| singleKIn | 输入 | 设置的singleKIn大小，单位为元素，默认值为-1。-1表示不设置指定的singleKIn，该值由tiling函数自行计算。 |

## 返回值说明

-1表示设置失败；0表示设置成功。

## 约束说明

-   在MxMatmul场景中，如果A与B矩阵的位置同时为GM，对singleKIn没有特殊限制，在这种情况下，若scaleA和scaleB的K方向大小（即Ceil\(singleKIn, 32\)）为奇数，用户需自行在scaleA和scaleB的K方向补0至偶数。例如，当singleKIn为30时，Ceil\(singleKIn, 32\)为1，用户需要自行在scaleA和scaleB的K方向补0，使K方向为偶数。对于其它A、B矩阵逻辑位置的组合情况，即A与B矩阵的位置不同时为GM，singleKIn以32个元素向上对齐后的数值必须是32的偶数倍。
-   在MxMatmul场景中，当输入数据类型为fp4x2\_e2m1\_t、fp4x2\_e1m2\_t时，内轴必须为偶数。

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);       // 设置Matmul单次计算的形状
tiling.SetSingleShape(1024, 1024, 1024); // 设置单核计算的形状
tiling.SetOrgShape(1024, 1024, 1024);
```
