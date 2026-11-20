# Summary of Dual-Issue and Single-Issue Reg Vector Computation Interface Instructions<a name="reg-vector-dual-issue"></a>

<!-- md-trans-meta sourceCommit=aff19f76633e6edc0b718e456c0601a03d2f7983 translatedAt=2026-11-17T03:28:28.844Z pushedAt=2026-11-17T09:52:41.760Z nl=2 -->

This section summarizes the dual-issue and single-issue situations of Reg vector computation API instructions. Single-issue means that one instruction can be executed in one cycle, and dual-issue means that two instructions can be executed in one cycle.
<br>**For registers: only two Reg registers can be written in one cycle. For Unified Buffer (UB): only two VLs can be read in one cycle (one VL is 256B), or one VL can be read and one VL can be written in one cycle.**

This section does not include software simulation APIs and applies only to the following models:

Ascend 950PR & 950DT products

**Table 1**  Summary of Dual-Issue API Instructions<a id="table-dual-issue"></a>

| API Category | Hardware Instruction Name | Corresponding API |
| --- | --- | --- |
| register data type | PSET | CreateMask |
| register data type | PLT | UpdateMask |
| Reg Data Load | VLDS/VLD | Continuous alignment load-in (LoadAlign):<br>single load-in mode (except DIST_DS_B8 and DIST_DS_B16 distribution modes) |
| Reg Data Load | VLDAS/VLDA | Continuous unaligned load initialization (LoadUnalignPre) |
| Reg Data Load | VLDUS/VLDU | Continuous unaligned load (LoadUnalign) |
| Reg Data Load | PLDS/PLD | MaskReg load (LoadAlign) |
| Reg Data Load | VMOV | Move (Reg data load-in) |
| MaskReg computation | PMOV | Move (MaskReg computation)|
| MaskReg computation | PSEL | Select |
| MaskReg computation | MOVP | MoveMask |
| Basic Arithmetic | VABS | Abs |
| Basic Arithmetic | VEXP | Exp |
| Basic Arithmetic | VSQRT | Sqrt |
| Basic Arithmetic | VLN | Ln |
| Basic Arithmetic | VLN | Log |
| Basic Arithmetic | VNEG | Neg |
| Basic Arithmetic | VRELU | Relu |
| Basic Arithmetic | VPRELU | Prelu |
| Basic Arithmetic | VLRELU | LeakyRelu |
| Basic Arithmetic | VADD/VADDC | Add |
| Basic Arithmetic | VADDCS | AddC |
| Basic Arithmetic | VDIV | Div |
| Basic Arithmetic | VMAX | Max |
| Basic Arithmetic | VMIN | Min |
| Basic Arithmetic | VMUL | Mul |
| Basic Arithmetic | VMULL | Mull |
| Basic Arithmetic | VSUB/VSUBC | Sub |
| Basic Arithmetic | VSUBCS | SubC |
| Basic Arithmetic | VADDS | Adds |
| Basic Arithmetic | VMAXS | Maxs |
| Basic Arithmetic | VMINS | Mins |
| Basic Arithmetic | VMULS | Muls |
| Logical Calculation | VNOT/PNOT | Not |
| Logical Calculation | VAND/PAND | And |
| Logical Calculation | VOR/POR | Or |
| Logical Calculation | VXOR/PXOR | Xor |
| Logical Calculation | VSHL | ShiftLeft |
| Logical Calculation | VSHR | ShiftRight |
| Logical Calculation | VSHLS | ShiftLefts |
| Logical Calculation | VSHRS | ShiftRights |
| Compound Calculation | VABSDIF | AbsSub |
| Compound Calculation | VAXPY | Axpy |
| Compound Calculation | VEXPDIF | ExpSub |
| Compound Calculation | VMADD | MulDstAdd |
| Compound Calculation | VMULA | MulAddDst |
| Compound Calculation | VMULSCVT | MulsCast |
| Comparison and selection | VCMP | Compare |
| Comparison and selection | VCMPS | Compares |
| Comparison and selection | VSEL/PSEL | Select |
| Type Conversion | VCVTFI/VCVTFF/VCVTFF2/VCVTIF/VCVTII | Cast |
| Type Conversion | VTRC | Truncate |
| Data padding | VBR/VDUPI | (Broadcast scalarValue to registers) Duplicate |
| index operation | VCI | Arange |

**Table 2**  Summary of Single-Issue Interface Instructions<a id="table-single-issue"></a>

| API Category | Hardware Instruction Name | Corresponding API |
| --- | --- | --- |
| Reg Data Load | VSLDB | Non-contiguous aligned load (LoadAlign) |
| Reg Data Load | VLDS/VLD | Continuous alignment load-in (LoadAlign):<br>Dual load-in mode and the DIST_DS_B8 and DIST_DS_B16 distribution modes of the single load-in mode |
| Reg Data Load | MOVVP | MaskGenWithRegTensor |
| Reg Data Load | VGATHER2 | Gather |
| Reg Data Load | VGATHERB | GatherB |
| Reg Data Store | VSSTB | Non-contiguous aligned store (StoreAlign)|
| Reg Data Store | VSTS/VST | Contiguous aligned store (StoreAlign) |
| Reg Data Store | VSTUS/VSTU/VSTUR | Continuous unaligned store (StoreUnalign) |
| Reg Data Store | VSTAS/VSTA/VSTAR | Continuous unaligned store post-processing (StoreUnalignPost) |
| Reg Data Store | PSTS/PST | MaskReg store (StoreAlign) |
| Reg Data Store | PSTU | MaskReg unaligned store (StoreUnalign) |
| Reg Data Store | VSCATTER | Scatter |
| MaskReg computation | PINTLV | Interleave |
| MaskReg computation | PDINTLV | DeInterleave |
| MaskReg computation | PPACK | Pack |
| MaskReg computation | PUNPACK | UnPack |
| Reduction Computation | VCADD | Reduce (ReduceType::SUM) |
| Reduction Computation | VCMAX | Reduce (ReduceType::MAX) |
| Reduction Computation | VCMIN | Reduce (ReduceType::MIN) |
| Reduction Computation | VCGADD | ReduceDataBlock (ReduceType::SUM) |
| Reduction Computation | VCCGMAX | ReduceDataBlock (ReduceType::MAX) |
| Reduction Computation | VCGMIN | ReduceDataBlock (ReduceType::MIN) |
| Reduction Computation | VCPADD | PairReduceElem |
| Comparison and selection | VSQZ | Squeeze |
| Data padding | VDUP | Broadcast the lowest-bit element of srcReg to register (Duplicate) |
| discrete operation | VSELR | Gather |
| Data rearrangement | VINTLV | Interleave |
| Data rearrangement | VDINTLV | DeInterleave |
| data compression | VUSQZ | Unsqueeze |
| data compression | VPACK | Pack |
| data compression | VUNPACK | UnPack |
| histogram computation | DHISTv2 | Histograms |
