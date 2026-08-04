# 概述

复合计算支持以下接口：

**表1**  复合计算接口

| 接口 | 描述 |
| :--- | :--- |
| [AbsSub](AbsSub.md) | $dstReg_i = \lvert srcReg0_i - srcReg1_i \rvert$ |
| [Axpy](Axpy.md) | $dstReg_i = scalarValue \times srcReg_i + dstReg_i$ |
| [ExpSub](ExpSub.md) | • srcReg数据类型为float时：<br>$dstReg_i = e^{srcReg0_i - srcReg1_i}$<br>• srcReg数据类型为half时：<br>$dstReg_i = e^{cast\_f16\_to\_f32(srcReg0_i) - cast\_f16\_to\_f32(srcReg1_i)}$ |
| [MulDstAdd](MulDstAdd.md) | $dstReg_i = dstReg_i \times srcReg0_i + srcReg1_i$ |
| [MulAddDst](MulAddDst.md) | $dstReg_i = srcReg0_i \times srcReg1_i + dstReg_i$ |
| [MulsCast](MulsCast.md) | $dst_i = cast\_round\_to\_f16(src_i \times scalarValue)$ |
