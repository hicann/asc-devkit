# Summary of MergeMode Support for Reg Vector Computation APIs

<!-- md-trans-meta sourceCommit=bca6c1424f07e7236ea79f6afa0b44e2c7c5eaa2 translatedAt=2026-11-17T03:35:06.507Z pushedAt=2026-11-17T09:52:41.762Z nl=2 -->

APIs that do not support the MergeMode template parameter are not listed below.

**Table 1**  MergeMode support for data loading APIs

| API | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :--- | :----------------------- | :----------------------- |
| Move | MaskMergeMode::MERGING   | None                       |

**Table 2**  MergeMode support for basic arithmetic APIs

| API      | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :-------- | :----------------------- | :----------------------- |
| Add       | MaskMergeMode::ZEROING   | MaskMergeMode::MERGING   |
| Sub       | MaskMergeMode::ZEROING   | None                       |
| Exp       | MaskMergeMode::ZEROING   | None                       |
| Sqrt      | MaskMergeMode::ZEROING   | None                       |
| Max       | MaskMergeMode::ZEROING   | MaskMergeMode::MERGING   |
| Min       | MaskMergeMode::ZEROING   | MaskMergeMode::MERGING   |
| Mul       | MaskMergeMode::ZEROING   | None                       |
| Div       | MaskMergeMode::ZEROING   | None                       |
| Adds      | MaskMergeMode::ZEROING   | None                       |
| Maxs      | MaskMergeMode::ZEROING   | None                       |
| Mins      | MaskMergeMode::ZEROING   | None                       |
| Muls      | MaskMergeMode::ZEROING   | None                       |
| Abs       | MaskMergeMode::ZEROING   | None                       |
| Ln        | MaskMergeMode::ZEROING   | None                       |
| Log       | MaskMergeMode::ZEROING   | None                       |
| Log2      | None                       | MaskMergeMode::ZEROING   |
| Log10     | None                       | MaskMergeMode::ZEROING   |
| Neg       | MaskMergeMode::ZEROING   | None                       |
| Relu      | MaskMergeMode::ZEROING   | None                       |
| Prelu     | MaskMergeMode::ZEROING   | None                       |
| LeakyRelu | MaskMergeMode::ZEROING   | None                       |

**Table 3**  MergeMode support for logical computation APIs

| API                   | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :--------------------- | :----------------------- | :----------------------- |
| Not (operating on RegTensor) | MaskMergeMode::ZEROING   | None                       |
| And (operating on RegTensor) | MaskMergeMode::ZEROING   | None                       |
| Or (operating on RegTensor)  | MaskMergeMode::ZEROING   | None                       |
| Xor (operating on RegTensor) | MaskMergeMode::ZEROING   | None                       |
| ShiftRight             | MaskMergeMode::ZEROING   | None                       |
| ShiftLefts             | MaskMergeMode::ZEROING   | None                       |
| ShiftLeft              | MaskMergeMode::ZEROING   | None                       |
| ShiftRights            | MaskMergeMode::ZEROING   | None                       |

**Table 4**  MergeMode support for compound computation APIs

| API      | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :-------- | :----------------------- | :----------------------- |
| AbsSub    | MaskMergeMode::ZEROING   | None                       |
| ExpSub    | MaskMergeMode::ZEROING   | None                       |
| Axpy      | MaskMergeMode::ZEROING   | None                       |
| MulDstAdd | MaskMergeMode::ZEROING   | None                       |
| MulAddDst | MaskMergeMode::ZEROING   | None                       |

**Table 5**  MergeMode support status of type conversion APIs

| API     | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :------- | :----------------------- | :----------------------- |
| Cast     | MaskMergeMode::ZEROING   | MaskMergeMode::MERGING   |
| Truncate | MaskMergeMode::ZEROING   | None                       |

**Table 6**  MergeMode support status of reduction APIs

| API            | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :-------------- | :----------------------- | :----------------------- |
| Reduce          | MaskMergeMode::ZEROING   | None                       |
| ReduceDataBlock | MaskMergeMode::ZEROING   | None                       |
| PairReduceElem  | MaskMergeMode::ZEROING   | None                       |

**Table 7**  MergeMode support status of data filling APIs

| API      | Hardware-supported template parameter enumeration values | Template parameter enumeration values supported by software |
| :-------- | :--------------------------------------------- | :----------------------- |
| Duplicate | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | None                       |
