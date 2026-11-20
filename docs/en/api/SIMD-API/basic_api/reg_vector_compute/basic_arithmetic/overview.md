# Overview

<!-- md-trans-meta sourceCommit=00f2fa768749ee1a0d8f4a453d77d0c75cf3b952 translatedAt=2026-11-05T02:35:19.189Z pushedAt=2026-11-18T01:25:30.825Z nl=2 -->

**Table 1**  Unary operations

| API | Description |
| --- | --- |
| [Abs](Abs.md) | &bull; For real number types<br>Obtains the absolute value of each valid element in srcReg and writes the result to the corresponding position in dstReg. The calculation formula is as follows: $dstReg_i = \lvert srcReg_i \rvert$<br>&bull; For complex number types<br>Obtains the modulus of each valid element in srcReg and writes the result to the corresponding position in dstReg. The calculation formula is as follows: $dstReg_i = \lvert srcReg_i \rvert = (\alpha^2 + \beta^2)^{1/2}$ (where $srcReg_i = \alpha + \beta i$, α is the real part of the complex number, and β is the imaginary part of the complex number) |
| [Exp](Exp.md) | $dstReg_i = e^{srcReg_i}$ |
| [Ln](Ln.md) | $dstReg_i = \ln(srcReg_i)$ |
| [Log](Log.md) | $dstReg_i = \log(srcReg_i)$ |
| [Log2](Log2.md) | $dstReg_i = \log_2(srcReg_i)$ |
| [Log10](Log10.md) | $dstReg_i = \log_{10}(srcReg_i)$ |
| [Neg](Neg.md) | $dstReg_i = -srcReg_i$ |
| [Relu](Relu.md) | $dstReg_i = \max(0, srcReg_i)$ |
| [Sqrt](Sqrt.md) | $dstReg_i = (srcReg_i)^{1/2}$ |

**Table 2**  Binary operations

| API | Description |
| --- | --- |
| [Add](Add.md) | $dstReg_i = srcReg0_i + srcReg1_i$ <br>$\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i$ |
| [AddC](AddC.md) | $\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i + carrySrc_i$ |
| [Div](Div.md) | $dstReg_i = srcReg0_i / srcReg1_i$ |
| [Max](Max.md) | $dstReg_i = \max(srcReg0_i, srcReg1_i)$ |
| [Min](Min.md) | $dstReg_i = \min(srcReg0_i, srcReg1_i)$ |
| [Mul](Mul.md) | $dstReg_i = srcReg0_i \times srcReg1_i$ |
| [Mull](Mull.md) | $dstReg0_i = (srcReg0_i \times srcReg1_i) \bmod 2^{bit}$<br>$dstReg1_i = (srcReg0_i \times srcReg1_i) / 2^{bit}$<br>where bit indicates the bit width of the operands. |
| [Prelu](Prelu.md) | $dstReg_i = \begin{cases} srcReg0_i & srcReg0_i \geq 0 \\ srcReg0_i \times srcReg1_i & srcReg0_i < 0 \end{cases}$ |
| [Sub](Sub.md) | $dstReg_i = srcReg0_i - srcReg1_i$ <br>$\{carry_i, dstReg_i\} = \{1'b0, srcReg0_i\} + \{1'b0, \sim srcReg1_i\} + 33'b1$ |
| [SubC](SubC.md) | $\{carry_i, dstReg_i\} = \{1'b0, srcReg0_i\} + \{1'b0, \sim srcReg1_i\} + \{32'b0,carrySrc_i\}$ |

**Table 3**  Scalar binary operations

| API | Description |
| --- | --- |
| [Adds](Adds.md) | $dstReg_i = srcReg_i + scalarValue$ |
| [LeakyRelu](LeakyRelu.md) | $dstReg_i = \begin{cases} srcReg_i & srcReg_i > 0 \\ srcReg_i \times scalarValue & srcReg_i \leq 0 \end{cases}$ |
| [Maxs](Maxs.md) | $dstReg_i = \max(srcReg_i, scalarValue)$ |
| [Mins](Mins.md) | $dstReg_i = \min(srcReg_i, scalarValue)$ |
| [Muls](Muls.md) | $dstReg_i = srcReg_i \times scalarValue$ |
