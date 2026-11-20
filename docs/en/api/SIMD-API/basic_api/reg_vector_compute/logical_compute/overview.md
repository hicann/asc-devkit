# Overview

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T03:02:52.749Z pushedAt=2026-11-18T01:25:30.884Z nl=2 -->

Logical computation supports the following APIs:

**Table 1** Logical computation APIs

| API | Description |
| :--- | :--- |
| [Not](Not.md) | • For RegTensor operations:<br>Performs a bitwise NOT operation on the input data srcReg based on mask, and writes the result to dstReg.<br>• For MaskReg operations:<br>Performs a bitwise NOT operation on the input data src based on mask, and writes the result to dst. |
| [And](And.md) | • For RegTensor operations:<br>Performs a bitwise AND (&) operation on the input data srcReg0 and srcReg1 based on mask, and writes the result to dstReg.<br>• For MaskReg operations:<br>Performs a bitwise AND (&) operation on the two input MaskReg data src0 and src1 based on mask, and writes the result to dst. |
| [Or](Or.md) | • For RegTensor operations:<br>Performs a bitwise OR (\|) operation on the input data srcReg0 and srcReg1 based on mask, and writes the result to dstReg.<br>• For MaskReg operations:<br>Performs a bitwise OR (\|) operation on the two input MaskReg data src0 and src1 based on mask, and writes the result to dst. |
| [Xor](Xor.md) | • For RegTensor operations:<br>Performs a bitwise XOR (^) operation on the input data srcReg0 and srcReg1 based on mask, and writes the result to dstReg.<br>• For MaskReg operations:<br>Performs a bitwise XOR (^) operation on the two input MaskReg data src0 and src1 based on mask, and writes the result to dst. |
| [ShiftLeft](ShiftLeft.md) | $dstReg_i = srcReg0_i \ll srcReg1_i$ |
| [ShiftRight](ShiftRight.md) | $dstReg_i = srcReg0_i \gg srcReg1_i$ |
| [ShiftLefts](ShiftLefts.md) | $dstReg_i = srcReg_i \ll scalarValue$ |
| [ShiftRights](ShiftRights.md) | $dstReg_i = srcReg_i \gg scalarValue$ |
