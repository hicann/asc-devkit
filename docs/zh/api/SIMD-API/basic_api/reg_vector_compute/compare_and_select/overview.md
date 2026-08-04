# 概述

比较与选择支持以下接口：

**表1**  比较与选择接口

| 接口 | 描述 |
| :--- | :--- |
| [Compare](Compare.md) | 逐元素比较两个RegTensor大小，根据模板参数指定的比较模式，将比较结果写入目的操作数MaskReg中对应比特位，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [Compares](Compares.md) | 该接口用于逐元素比较源操作数中每个元素与一个标量的大小关系，根据模板参数指定的比较模式判断结果是否为真，若为真则输出MaskReg对应比特位置1，否则置0。<br>其中比较模式包括LT（小于）、GT（大于）、GE（大于或等于）、EQ（等于）、NE（不等于）、LE（小于或等于）。 |
| [Select](Select.md) | 给定两个源操作数srcReg0和srcReg1，根据mask的比特位值选取元素，得到目的操作数dstReg。当mask的比特位是1时，从srcReg0中选取对应位置的数，比特位是0时从srcReg1选取对应位置的数。 |
| [Squeeze](Squeeze.md) | 将传入的srcReg中被mask选择的有效元素依次复制到dstReg中，有效元素在dstReg中从低到高连续排列。dstReg中剩余位置元素置为0。 |
