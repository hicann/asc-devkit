# Math API样例介绍

## 概述

本样例集介绍了Math API不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
|--------------------------------------| --------------------------------------------------- | --- |
| [acosh](./acosh) |  本样例演示了基于Acosh高阶API的算子实现。样例按元素做双曲反余弦函数计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [axpy_half_float](./axpy_half_float) |  本样例基于Axpy实现源操作数src中每个元素与标量求积后和目的操作数dst中的对应元素相加的功能。Axpy接口的源操作数和目的操作数的数据类型只能取三种组合：(half, half)、(float, float)、(half, float)。本样例中输入tensor和标量的数据类型为half，输出tensor数据类型为float。本样例通过Ascend C编程语言实现了Axpy算子，使用<<<>>>内核调用符来完成算子核函数在NPU侧运行验证的基础流程，给出了对应的端到端实现 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [bitwiseand](./bitwiseand) |  本样例演示了基于BitwiseAnd高阶API的算子实现。样例逐比特对两个输入进行与操作 | Ascend 950PR&950DT系列产品 |
| [ceil](./ceil) |  本样例演示了基于Ceil高阶API的算子实现。样例获取大于或等于x的最小的整数值，即向正无穷取整操作 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [clamp](./clamp) |  本样例演示了基于Clamp高阶API的算子实现。将输入中非nan且超出[min, max]范围的值剪裁至max或min，若min>max，则所有非nan值均置为max | Ascend 950PR&950DT系列产品 |
| [clampmin](./clampmin) |  本样例演示了基于ClampMin高阶API的算子实现。样例将srcTensor中小于scalar的数替换为scalar，大于等于scalar的数保持不变，作为dstTensor输出 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [cumsum](./cumsum) |  本样例介绍了调用CumSum高阶API实现cumsum单算子，用于对输入张量按行或列进行累加和操作 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [erf](./erf) |  本样例演示了基于Erf高阶API的算子实现。样例按元素做误差函数计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [exp](./exp) |  本样例演示了基于Exp高阶API的算子实现。样例按元素取自然指数，用户可以选择是否使用泰勒展开公式进行计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [fma](./fma) |  本样例演示了基于Fma高阶API的算子实现。样例按元素计算两个输入相乘后与第三个输入相加的结果 | Ascend 950PR&950DT系列产品 |
| [fmod](./fmod) |  本样例演示了基于Fmod高阶API的算子实现。样例按元素计算两个浮点数a，b相除后的余数 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [frac](./frac) |  本样例演示了基于Frac高阶API的算子实现。样例按元素做取小数计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [isfinite](./isfinite) |  本样例演示了基于IsFinite高阶API的算子实现。样例按元素判断输入的浮点数是否非NAN、非INF，输出结果为浮点数或者布尔值 | Ascend 950PR&950DT系列产品 |
| [log](./log) |  本样例演示了基于Log高阶API的算子实现。样例按元素以e、2、10为底做对数运算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [logicaland](./logicaland) |  本样例演示了基于LogicalAnd高阶API的算子实现。样例按元素进行与操作，输入数据类型不是bool时，零被视为False，非零数据被视为True | Ascend 950PR&950DT系列产品 |
| [power](./power) |  本样例演示了基于Power高阶API的算子实现。样例实现按元素做幂运算功能，支持三种功能：指数和底数分别为张量对张量、张量对标量、标量对张量的幂运算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [rint](./rint) |  本样例演示了基于Rint高阶API的算子实现。样例获取与输入数据最接近的整数，若存在两个相同接近的整数，则获取其中的偶数 | Ascend 950PR&950DT系列产品 |
| [where](./where) |  本样例演示了基于Where高阶API的算子实现。样例根据指定的条件，从两个源操作数中选择元素，生成目标操作数。两个源操作数均可以是LocalTensor或标量 | Ascend 950PR&950DT系列产品 |
| [xor](./xor) |  本样例演示了基于Xor高阶API的算子实现。样例按元素执行Xor运算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
