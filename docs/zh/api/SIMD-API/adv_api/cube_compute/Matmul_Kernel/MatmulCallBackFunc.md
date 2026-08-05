# MatmulCallBackFunc

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
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->

## 功能说明

模板参数MatmulCallBackFunc支持用户定制化Matmul的A矩阵、B矩阵及C矩阵的搬入搬出功能，如非连续搬入或针对搬出设置不同的数据片段间隔等。具体方式为：用户根据实际需要，实现一个或多个自定义的搬运函数，定义Matmul对象时，通过模板参数MatmulCallBackFunc，传入实现的搬运函数的函数指针，传入的函数指针会替换Matmul流程中默认的搬运函数。

MatmulCallBackFunc中包含3个可由用户自定义的回调函数接口，即用户可配置3个函数指针。3个函数指针分别为C矩阵从L0C Buffer（CO1）拷贝到GM，A矩阵从GM拷贝到L1 Buffer（A1），B矩阵从GM拷贝到L1 Buffer（B1）的回调函数指针。3个函数指针的位置固定，不使用自定义搬运函数的函数指针位置需要设为空指针。各个功能回调函数接口定义及参数释义见[表1 MatmulCallBackFunc回调函数接口及参数说明](#table10989848113111)。每个回调函数实现矩阵搬运中单个基本块（A矩阵基本块baseM \* baseK、B矩阵基本块baseK \* baseN、C矩阵基本块baseM \* baseN）的搬运策略，无法对整块内存空间进行管理。Matmul默认的搬运函数实现单核上单个基本块的搬运，搬运的基本块大小是固定的，在完整的Matmul计算过程中，多次调用搬运函数，对连续排布的基本块按顺序逐个搬运，以搬入A矩阵的过程为例，示意图如下。

**图1**  Matmul默认搬入A矩阵示意图  
![](../../../../figures/Matmul默认搬入A矩阵示意图.png "Matmul默认搬入A矩阵示意图")

**表1**  MatmulCallBackFunc回调函数接口及参数说明

<a name="table10989848113111"></a>
| 回调函数功能 | 回调函数接口 | 参数说明 |
| --- | --- | --- |
| 可自定义设置不同的搬出数据片段数目等参数，实现将Matmul计算结果从L0C Buffer（CO1）搬出到GM的功能 | void DataCopyOut(const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr) | gm：输出的GM地址。<br><br>co1Local: L0C Buffer（CO1）上的计算结果。<br><br>dataCopyOutParams：Matmul定义的DataCopyOutParams结构体指针，具体定义如下方代码所示，供用户参考使用。<br><br>tilingPtr: 用户使用[SetUserDefInfo](SetUserDefInfo.md)设置的tiling参数地址。<br><br>dataPtr: 用户使用[SetSelfDefineData](SetSelfDefineData.md)设置的计算数据地址。 |
| 可自定义左矩阵搬入首地址、搬运块位置、搬运块大小，实现左矩阵从GM搬入L1的功能 | void CopyA1(const LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr) | aMatrix: 目标L1Buffer地址。<br><br>gm：左矩阵GM首地址。<br><br>row、col：搬运块在M、K方向的索引，即在M、K方向上搬运块的序号，序号从0开始。<br><br>useM、useK：搬运块M、K方向大小，单位为元素个数。通过row、col和useM、useK计算出该搬运块左上角在左矩阵中的地址偏移。<br><br>tilingPtr: 用户使用[SetUserDefInfo](SetUserDefInfo.md)设置的tiling参数地址。<br><br>dataPtr: 用户使用[SetSelfDefineData](SetSelfDefineData.md)设置的计算数据地址。 |
| 可自定义右矩阵搬入首地址、搬运块位置、搬运块大小，实现右矩阵从GM搬入L1的功能 | void CopyB1(const LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr) | bMatrix: 目标L1Buffer地址。<br><br>gm：右矩阵GM首地址。<br><br>row、col：搬运块在K、N方向的索引，即在K、N方向上搬运块的序号，序号从0开始。<br><br>useK、useN：搬运块K、N方向大小，单位为元素个数。通过row、col和useK、useN计算出该搬运块左上角在右矩阵中的地址偏移。<br><br>tilingPtr: 用户使用[SetUserDefInfo](SetUserDefInfo.md)设置的tiling参数地址。<br><br>dataPtr: 用户使用[SetSelfDefineData](SetSelfDefineData.md)设置的计算数据地址。 |

```
struct DataCopyOutParams {
    uint16_t cBurstNum;         // 传输数据片段数目
    uint16_t burstLen;          // 连续传输数据片段长度
    uint16_t srcStride;         // 源tensor相邻连续数据片段间隔
    uint32_t dstStride;         // 目的tensor相邻连续数据片段间隔
    uint16_t oriNSize;          // NZ转ND时，源tensorN方向大小
    bool enUnitFlag;            // 是否开启UnitFlag
    uint64_t quantScalar;       // 量化场景下量化Scalar的值
    uint64_t cbufWorkspaceAddr; // 量化场景下量化Tensor地址
}
```

## 约束说明

无

## 调用示例

```
// 用户自定义回调函数
void DataCopyOut(
    const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr,
    const uint64_t dataPtr);
void CopyA1(
    const LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK,
    const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(
    const LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN,
    const uint64_t tilingPtr, const uint64_t dataPtr);

AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
uint64_t tilingPtr = reinterpret_cast<uint64_t>(tiling);
mm.SetUserDefInfo(tilingPtr); // 设置算子tiling地址，用于回调函数使用
GlobalTensor<SrcT> dataGM;    // 保存有回调函数需使用的计算数据的GM
uint64_t dataGMPtr = reinterpret_cast<uint64_t>(dataGM.address_);
mm.SetSelfDefineData(dataGMPtr); // 设置需要的计算数据或在GM上存储的数据地址等信息，用于回调函数使用
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll();
mm.End();
```
