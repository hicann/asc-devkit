# MatmulCallBackFunc

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T06:46:10.612Z pushedAt=2026-09-12T09:55:18.118Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

The template parameter **MatmulCallBackFunc** allows you to customize the move-in and move-out functionality of matrix A, matrix B, and matrix C in Matmul, such as non-contiguous move-in or setting different data segment intervals for move-out. The specific approach is as follows: based on actual needs, you implement one or more custom data transfer functions. When defining a Matmul object, you pass the function pointers of the implemented data transfer functions through the template parameter **MatmulCallBackFunc**. The passed function pointers replace the default data transfer functions in the Matmul process.

**MatmulCallBackFunc** contains three callback function APIs that you can customize, that is, you can configure three function pointers. The three function pointers are the callback function pointers for copying matrix C from CO1 to GM, matrix A from GM to A1, and matrix B from GM to B1, respectively. The positions of the three function pointers are fixed, and the position of a function pointer that does not use a custom data transfer function must be set to a null pointer. For the definition and parameter description of each callback function API, see [Table 1 MatmulCallBackFunc callback function APIs and parameter descriptions](#table10989848113111). Each callback function implements the data transfer strategy for a single basic block in matrix transfer (basic block baseM \* baseK of matrix A, basic block baseK \* baseN of matrix B, and basic block baseM \* baseN of matrix C), and cannot manage the entire memory space. The default data transfer function of Matmul implements the transfer of a single basic block on a single core. The size of the transferred basic block is fixed. During the complete Matmul computation process, the data transfer function is called multiple times to transfer the contiguously arranged basic blocks one by one in order. The following figure uses the process of moving in matrix A as an example.

**Figure 1** Moving blocks to matrix A by default using Matmul  
![](../../../../figures/moving_blocks_to_matrix_a_by_matmul.png "Moving blocks to matrix A by default using Matmul")

**Table 1** MatmulCallBackFunc callback function APIs and parameter descriptions

<a name="table10989848113111"></a>
| Callback Function Functionality | Callback Function API | Parameter Description |
| --- | --- | --- |
| Customizable parameters such as the number of data segments to move out, implementing the function of moving the Matmul computation result from CO1 to GM | void DataCopyOut(const __gm__ void *gm, const LocalTensor<int8_t> &co1Local, const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr) | gm: Output GM address.<br><br>co1Local: Computation result on CO1.<br><br>dataCopyOutParams: Pointer to the DataCopyOutParams structure defined by Matmul. See the code below for the specific definition, which is provided for your reference.<br><br>tilingPtr: Address of the tiling parameter set by you using [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Address of the computation data set by you using [SetSelfDefineData](SetSelfDefineData.md). |
| Customizable left matrix move-in start address, transfer block position, and transfer block size, implementing the function of moving the left matrix from GM to L1 | void CopyA1(const LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr) | aMatrix: Target L1 buffer address.<br><br>gm: Start address of the left matrix in GM.<br><br>row, col: Indices of the transfer block in the M and K directions, that is, the sequence numbers of the transfer block in the M and K directions, starting from 0.<br><br>useM, useK: Sizes of the transfer block in the M and K directions, in number of elements. The address offset of the upper-left corner of the transfer block in the left matrix is calculated using row, col, useM, and useK.<br><br>tilingPtr: Address of the tiling parameter set by you using [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Address of the computation data set by you using [SetSelfDefineData](SetSelfDefineData.md). |
| Customizable right matrix move-in start address, transfer block position, and transfer block size, implementing the function of moving the right matrix from GM to L1 | void CopyB1(const LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr) | bMatrix: Target L1 buffer address.<br><br>gm: Start address of the right matrix in GM.<br><br>row, col: Indices of the transfer block in the K and N directions, that is, the sequence numbers of the transfer block in the K and N directions, starting from 0.<br><br>useK, useN: Sizes of the transfer block in the K and N directions, in number of elements. The address offset of the upper-left corner of the transfer block in the right matrix is calculated using row, col, useK, and useN.<br><br>tilingPtr: Address of the tiling parameter set by you using [SetUserDefInfo](SetUserDefInfo.md).<br><br>dataPtr: Address of the computation data set by you using [SetSelfDefineData](SetSelfDefineData.md). |

```
struct DataCopyOutParams {
    uint16_t cBurstNum; //Number of transferred data segments.
    uint16_t burstLen; //Length of consecutively transferred data segments.
    uint16_t srcStride;//Interval between adjacent consecutive data segments of the source tensor.
    uint32_t dstStride; // Interval between adjacent consecutive data segments of the destination tensor.
    uint16_t oriNSize; // Size of the source tensor in the N direction when converting from NZ to ND.
    bool enUnitFlag; // Whether to enable UnitFlag.
    uint64_t quantScalar; // Value of the quantization scalar in quantization scenarios.
    uint64_t cbufWorkspaceAddr; //Address of the quantization tensor in quantization scenarios.
}
```

## Constraints

None

## Examples

```
//User-defined callback function.
void DataCopyOut(const __gm__ void *gm, const LocalTensor<int8_t> &co1Local, const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyA1(const LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(const LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr);

AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
uint64_t tilingPtr = reinterpret_cast<uint64_t>(tiling);
mm.SetUserDefInfo(tilingPtr); // Set the operator tiling address for use by the callback function.
GlobalTensor<SrcT> dataGM; // GM that stores the computation data required by the callback function.
uint64_t dataGMPtr = reinterpret_cast<uint64_t>(dataGM.address_);
mm.SetSelfDefineData(dataGMPtr); // Set the required computation data or the address of the data stored in GM for use by the callback function.
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll();
mm.End();
```
