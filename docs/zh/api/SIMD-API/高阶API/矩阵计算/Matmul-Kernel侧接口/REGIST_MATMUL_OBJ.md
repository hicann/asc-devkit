# REGIST\_MATMUL\_OBJ

## 功能说明

初始化Matmul对象。

## 函数原型

```
REGIST_MATMUL_OBJ(tpipe, workspace, ...)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tpipe | 输入 | Tpipe对象。 |
| workspace | 输入 | 系统workspace指针。 |
| ... | 输入 | 可变参数，传入Matmul对象和与之对应的Tiling结构，要求Tiling结构的数据类型为[TCubeTiling结构](../Matmul-Tiling类/TCubeTiling结构体.md#table1563162142915)。<br><br>Tiling参数可以通过Host侧[GetTiling](../Matmul-Tiling类/GetTiling.md)接口获取，并传递到kernel侧使用。 |

## 返回值说明

无

## 约束说明

-   在分离模式中，本接口必须在[InitBuffer](../../../基础API/资源管理/TPipe/InitBuffer.md)接口前调用。
-   在程序中，最多支持定义4个Matmul对象。
-   在Matmul的内部实现中使用了[CrossCoreSetFlag](../../../基础API/同步控制/核间同步/CrossCoreSetFlag(ISASI).md)进行核间同步控制，因此不建议开发者同时使用CrossCoreSetFlag和Matmul，以避免核间同步标记flagId冲突的风险。Matmul高阶API内部占用的flagId范围与定义的Matmul对象数目相关。假设定义了N个Matmul对象，则Matmul高阶API内部占用的flagId范围为\[0, 2 \* N - 1\]。Matmul最多支持定义4个对象，此时flagId占用范围为\[0, 7\]。
-   当代码中只有一个Matmul对象时，本接口可以不传入tiling参数，通过[Init](Init-85.md)接口单独传入tiling参数。
-   当代码中有多个Matmul对象时，必须满足Matmul对象与其tiling参数一一对应，依次传入，具体方式请参考调用示例。
-   在分离模式中，调用本接口后，AIC核不会主动执行接口，仅在AIV核执行到下述接口后，才会触发AIC核的执行，其他接口则不会在AIC核上执行。
    -   [Matmul Kernel侧接口](Matmul-Kernel侧接口.md)。
    -   [DataCopy(GMToL1连续数据搬运)](../../../基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1连续数据搬运）.md)、[DataCopy(GMToL1高维切分数据搬运)](../../../基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1高维切分数据搬运）.md)、[DataCopy(GMToL1随路转换-ND2NZ搬运)](../../../基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)。

## 调用示例

```
Tpipe pipe;
// 推荐：初始化单个matmul对象，传入tiling参数
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// 推荐：初始化多个matmul对象，传入对应的tiling参数
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1, mm1tiling, mm2, mm2tiling, mm3, mm3tiling, mm4, mm4tiling);
// 初始化单个matmul对象，未传入tiling参数。注意，该场景下需要使用Init接口单独传入tiling参数。这种方式将matmul对象的初始化和tiling的设置分离，比如，Tiling可变的场景，可通过这种方式多次对Tiling进行重新设置
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.Init(&tiling);
```
