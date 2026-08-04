# 概述

逻辑计算支持以下接口：

**表1**  逻辑计算接口

| 接口 | 描述 |
| :--- | :--- |
| [Not](Not.md) | • 对RegTensor操作：<br>根据mask对输入数据srcReg执行按位取反操作，将结果写入dstReg。<br>• 对MaskReg操作：<br>根据mask对输入数据src执行按位取反操作，将结果写入dst。 |
| [And](And.md) | • 对RegTensor操作：<br>根据mask对输入数据srcReg0、srcReg1执行按位与（&）操作，将结果写入dstReg。<br>• 对MaskReg操作：<br>根据mask对两个输入MaskReg数据src0、src1执行按位与（&）操作，将结果写入dst。 |
| [Or](Or.md) | • 对RegTensor操作：<br>根据mask对输入数据srcReg0、srcReg1按位求或（\|）操作，将结果写入dstReg。<br>• 对MaskReg操作：<br>根据mask对两个输入MaskReg数据src0、src1按位求或（\|）操作，将结果写入dst。 |
| [Xor](Xor.md) | • 对RegTensor操作：<br>根据mask对输入数据srcReg0、srcReg1按位异或（^）操作，将结果写入dstReg。<br>• 对MaskReg操作：<br>根据mask对两个输入MaskReg数据src0、src1按位异或（^）操作，将结果写入dst。 |
| [ShiftLeft](ShiftLeft.md) | $dstReg_i = srcReg0_i \ll srcReg1_i$ |
| [ShiftRight](ShiftRight.md) | $dstReg_i = srcReg0_i \gg srcReg1_i$ |
| [ShiftLefts](ShiftLefts.md) | $dstReg_i = srcReg_i \ll scalarValue$ |
| [ShiftRights](ShiftRights.md) | $dstReg_i = srcReg_i \gg scalarValue$ |
