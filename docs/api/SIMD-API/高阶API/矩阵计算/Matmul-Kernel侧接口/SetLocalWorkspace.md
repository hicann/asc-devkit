# SetLocalWorkspace

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

对于某些场景Matmul内部需要额外占用VECCALC空间，如果用户希望在算子中复用这个额外占用的VECCALC空间，则该空间需要用户预留，并申请好LocalTensor，将其起始物理地址传入给Matmul。具体需要申请的VECCALC临时空间大小由tiling接口[MatmulGetTmpBufSize](../获取Matmul计算所需空间/MatmulGetTmpBufSize.md)给出，满足以下几个条件之一就需要使用该接口传入UB临时空间：

-   C矩阵Position为TPosition::GM；
-   C矩阵CubeFormat为CubeFormat::ND；
-   A矩阵或者B矩阵CubeFormat为CubeFormat::ND；
-   存在Bias且Bias的Position不是VECCALC。

请在[Iterate](Iterate.md)或者[IterateAll](IterateAll.md)之前调用该接口。

获取到的UB临时空间大小以字节为单位。

## 函数原型

```
__aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tmpBuffer | 输入 | 临时空间，由用户申请并管理，TPosition为VECCALC。 |

## 返回值说明

无

## 约束说明

当开启MixDualMaster（双主模式）场景时，即模板参数[enableMixDualMaster](MatmulConfig.md#matmulconfig-params)设置为true，不支持使用该接口。

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetLocalWorkspace(mmFormatUb);    //设置临时VECCALC空间
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
