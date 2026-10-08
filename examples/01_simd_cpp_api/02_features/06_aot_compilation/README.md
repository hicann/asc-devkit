# AOT Compilation样例介绍

## 概述

基于AOT（Ahead-of-Time）编译优化的性能实践样例，通过将运行时Tiling参数提前到编译阶段常量化，让编译器进行更激进的循环展开、常量传播和死代码消除等优化，从而在运行时自动匹配预编译的特化版本，获得性能提升。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --- | --- | --- |
| [matmul_aot_compilation](./matmul_aot_compilation) | 在Matmul高阶API基础上引入AOT编译优化，演示Tiling常量定义、AOT Registry注册、运行时分发（AOTDispatcher）的完整流程，运行时根据Tiling参数自动匹配AOT特化版本或回退到通用版本。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
