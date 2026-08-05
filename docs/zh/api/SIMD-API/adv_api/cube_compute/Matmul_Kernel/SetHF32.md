# SetHF32

## 产品支持情况

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明

在纯Cube模式（只有矩阵计算）下，设置是否开启HF32（矩阵乘计算时可采用的数据类型）模式。开启后，在矩阵乘计算时，float32数据类型会转换为hf32数据类型，可提升计算性能，但同时也会带来精度损失。

## 函数原型

```
__aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| enableHF32 | 输入 | 配置是否开启HF32模式，默认值false(不开启)。 |
| transMode | 输入 | 配置在开启HF32模式时，float转换为hf32时所采用的ROUND模式。默认值0。<br><br>0：就近舍入，距离相等时向偶数进位。<br><br>1：就近舍入，距离相等时向远离0方向进位。 |

## 返回值说明

无

## 约束说明

本接口仅支持在纯Cube模式下调用。

## 调用示例

```
//纯Cube模式
#define ASCENDC_CUBE_ONLY
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);    //  A/B/C/BIAS类型是float
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.SetHF32(true);
mm.IterateAll(gm_c);
mm.SetHF32(false);
mm.End();
```
