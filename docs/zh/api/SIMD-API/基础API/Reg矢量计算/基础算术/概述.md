# 概述

**表 1**  单目运算

| 接口 | 描述 |
| --- | --- |
| [Abs](Abs.md) | &bull; 对实数类型<br>对srcReg中的有效元素逐个取绝对值，并将结果写入dstReg对应位置，计算公式如下：$dstReg_i = \lvert srcReg_i \rvert$<br>&bull; 对复数类型<br>对srcReg中有效元素逐个取模，并将结果写入dstReg对应位置，计算公式如下：$dstReg_i = \lvert srcReg_i \rvert = (\alpha^2 + \beta^2)^{1/2}$（其中$srcReg_i = \alpha + \beta i$，α为复数的实部，β为复数的虚部） |
| [Exp](Exp.md) | $dstReg_i = e^{srcReg_i}$ |
| [Ln](Ln.md) | $dstReg_i = \ln(srcReg_i)$ |
| [Log](Log.md) | $dstReg_i = \log(srcReg_i)$ |
| [Log2](Log2.md) | $dstReg_i = \log_2(srcReg_i)$ |
| [Log10](Log10.md) | $dstReg_i = \log_{10}(srcReg_i)$ |
| [Neg](Neg.md) | $dstReg_i = -srcReg_i$ |
| [Relu](Relu.md) | $dstReg_i = \max(0, srcReg_i)$ |
| [Sqrt](Sqrt.md) | $dstReg_i = (srcReg_i)^{1/2}$ |

**表 2**  双目运算

| 接口 | 描述 |
| --- | --- |
| [Add](Add.md) | $dstReg_i = srcReg0_i + srcReg1_i$ <br>$\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i$ |
| [AddC](AddC.md) | $\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i + carrySrc_i$ |
| [Div](Div.md) | $dstReg_i = srcReg0_i / srcReg1_i$ |
| [Max](Max.md) | $dstReg_i = \max(srcReg0_i, srcReg1_i)$ |
| [Min](Min.md) | $dstReg_i = \min(srcReg0_i, srcReg1_i)$ |
| [Mul](Mul.md) | $dstReg_i = srcReg0_i \times srcReg1_i$ |
| [Mull](Mull.md) | $dstReg0_i = (srcReg0_i \times srcReg1_i) \bmod 2^{bit}$<br>$dstReg1_i = (srcReg0_i \times srcReg1_i) / 2^{bit}$<br>其中，bit表示操作数的位宽bit数。 |
| [Prelu](Prelu.md) | $dstReg_i = \begin{cases} srcReg0_i & srcReg0_i > 0 \\ srcReg0_i \times srcReg1_i & srcReg0_i \leq 0 \end{cases}$ |
| [Sub](Sub.md) | $dstReg_i = srcReg0_i - srcReg1_i$ <br>$\{carry_i, dstReg_i\} = \{1'b0, srcReg0_i\} + \{1'b0, \sim srcReg1_i\} + 33'b1$ |
| [SubC](SubC.md) | $\{carry_i, dstReg_i\} = \{1'b0, srcReg0_i\} + \{1'b0, \sim srcReg1_i\} + \{32'b0,carrySrc_i\}$ |

**表 3**  标量双目运算

| 接口 | 描述 |
| --- | --- |
| [Adds](Adds.md) | $dstReg_i = srcReg_i + scalarValue$ |
| [LeakyRelu](LeakyRelu.md) | $dstReg_i = \begin{cases} srcReg_i & srcReg_i > 0 \\ srcReg_i \times scalarValue & srcReg_i \leq 0 \end{cases}$ |
| [Maxs](Maxs.md) | $dstReg_i = \max(srcReg_i, scalarValue)$ |
| [Mins](Mins.md) | $dstReg_i = \min(srcReg_i, scalarValue)$ |
| [Muls](Muls.md) | $dstReg_i = srcReg_i \times scalarValue$ |
