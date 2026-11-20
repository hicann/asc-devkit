# Overview

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T02:42:59.258Z pushedAt=2026-11-18T01:25:30.837Z nl=2 -->

Comparison and selection supports the following APIs:

**Table 1**  Comparison and selection APIs

| API | Description |
| :--- | :--- |
| [Compare](Compare.md) | Compares two RegTensors element by element, and writes the comparison result to the corresponding bit of the destination operand MaskReg according to the comparison mode specified by the template parameter. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. |
| [Compares](Compares.md) | This API compares each element of the source operand with a scalar element by element, and determines whether the result is true according to the comparison mode specified by the template parameter. If true, the corresponding bit of the output MaskReg is set to 1; otherwise, it is set to 0.<br>The comparison modes include LT (less than), GT (greater than), GE (greater than or equal to), EQ (equal to), NE (not equal to), and LE (less than or equal to). |
| [Select](Select.md) | Given two source operands srcReg0 and srcReg1, selects elements based on the bit values of the mask to obtain the destination operand dstReg. When the mask bit is 1, the element at the corresponding position is selected from srcReg0; when the bit is 0, the element at the corresponding position is selected from srcReg1. |
| [Squeeze](Squeeze.md) | Copies the valid elements selected by the mask from the input srcReg to dstReg in sequence. The valid elements are arranged contiguously from low to high in dstReg. The remaining positions in dstReg are set to 0. |
