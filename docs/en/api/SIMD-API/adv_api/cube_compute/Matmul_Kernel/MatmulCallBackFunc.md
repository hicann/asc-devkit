# MatmulCallBackFunc

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:33:06.095Z pushedAt=2026-11-18T04:53:14.670Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Not supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulCallBackFunc_res.md#id1 -->

## Description

The **MatmulCallBackFunc** template parameter allows users to customize the load and store operations of the A, B, and C cubes in Matmul, such as non-contiguous loading or setting different data segment intervals for storing. Specifically, users implement one or more custom copy functions as needed. When defining a Matmul object, they pass the function pointers of the implemented copy functions through the **MatmulCallBackFunc** template parameter. The passed function pointers replace the default copy functions in the Matmul process.

**MatmulCallBackFunc** contains three callback function APIs that users can customize, meaning users can configure three function pointers. The three function pointers are the callback function pointers for copying the C cube from the L0C Buffer (CO1) to GM, the A cube from GM to the L1 Buffer (A1), and the B cube from GM to the L1 Buffer (B1), respectively. The positions of the three function pointers are fixed, and function pointer positions that do not use custom copy functions must be set to null pointers. For the API definitions and parameter descriptions of each callback function, see [Table 1 MatmulCallBackFunc callback function APIs and parameter descriptions](#table10989848113111). Each callback function implements the copy strategy for a single basic block in cube movement (the A cube basic block baseM \* baseK, the B cube basic block baseK \* baseN, and the C cube basic block baseM \* baseN) and cannot manage the entire memory space. The default Matmul copy functions implement the copy of a single basic block on a single core, and the size of the copied basic block is fixed. During the complete Matmul computation process, the copy functions are called multiple times to copy the contiguously arranged basic blocks one by one in order. Taking the process of loading the A cube as an example, the schematic diagram is shown below.

**Figure 1**  Diagram of the default A cube loading in Matmul  
![](../../../../figures/matmul_default_load_a.png "Diagram of the default A cube loading in Matmul")

**Table 1**  MatmulCallBackFunc callback function APIs and parameter description

<a name="table10989848113111"></a>

| Callback Function Functionality | Callback Function API | Parameters |
| --- | --- | --- |
| Customize parameters such as the number of data segments to be stored to implement the function of copying the Matmul computation result from L0C Buffer (CO1) to GM | void DataCopyOut(const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr) | gm: Output GM address.<br><br>co1Local: Computation result on L0C Buffer (CO1).<br><br>dataCopyOutParams: Pointer to the DataCopyOutParams structure defined by Matmul. The specific definition is shown in the code below for your reference.<br><br>tilingPtr: Tiling parameter address set by the user through [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Computation data address set by the user through [SetSelfDefineData](SetSelfDefineData.md). |
| Customize left cube loading start address, transfer block position, and transfer block size to implement the function of loading the left cube from GM to L1 | void CopyA1(const LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr) | aMatrix: Target L1 Buffer address.<br><br>gm: Left cube GM start address.<br><br>row, col: Indices of the transfer block in the M and K directions, that is, the sequence numbers of the transfer block in the M and K directions, starting from 0.<br><br>useM, useK: Sizes of the transfer block in the M and K directions, in elements. The address offset of the upper-left corner of the transfer block in the left cube is calculated from row, col, useM, and useK.<br><br>tilingPtr: Tiling parameter address set by the user through [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Computation data address set by the user through [SetSelfDefineData](SetSelfDefineData.md). |
| Customize right cube loading start address, transfer block position, and transfer block size to implement the function of loading the right cube from GM to L1 | void CopyB1(const LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr) | bMatrix: Target L1 Buffer address.<br><br>gm: Right cube GM start address.<br><br>row, col: Indices of the transfer block in the K and N directions, that is, the sequence numbers of the transfer block in the K and N directions, starting from 0.<br><br>useK, useN: Sizes of the transfer block in the K and N directions, in elements. The address offset of the upper-left corner of the transfer block in the right cube is calculated from row, col, useK, and useN.<br><br>tilingPtr: Tiling parameter address set by the user through [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Computation data address set by the user through [SetSelfDefineData](SetSelfDefineData.md). |

```
struct DataCopyOutParams {
    uint16_t cBurstNum;         // Number of data fragments to transfer.
    uint16_t burstLen;          // Length of consecutive data fragments to transfer.
    uint16_t srcStride;         // Interval between adjacent consecutive data fragments in the source tensor.
    uint32_t dstStride;         // Interval between adjacent consecutive data fragments in the destination tensor.
    uint16_t oriNSize;          // Size of the source tensor in the N direction when converting NZ to ND.
    bool enUnitFlag;            // Whether to enable UnitFlag.
    uint64_t quantScalar;       // Value of the quantization scalar in the quantization scenario.
    uint64_t cbufWorkspaceAddr; // Address of the quantization tensor in the quantization scenario.
}
```

## Constraints

None

## Examples

```
// User-defined callback function.
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
mm.SetUserDefInfo(tilingPtr); // Set the operator tiling address for use by the callback function.
GlobalTensor<SrcT> dataGM;    // GM that stores the computation data required by the callback function.
uint64_t dataGMPtr = reinterpret_cast<uint64_t>(dataGM.address_);
mm.SetSelfDefineData(dataGMPtr); // Set the required computation data or the address of data stored in GM for use by the callback function.
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll();
mm.End();
```
