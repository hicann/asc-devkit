# 概述

## 试验接口概述

试验接口提供双返回值形式的寄存器计算、数据搬运及指令发射队列状态查询能力。使用时需定义`CANN_ASC_USE_EXPERIMENTAL`宏，并显式包含对应头文件；后续版本不保证兼容性。

## API列表

### 试验接口

**表1** 试验接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_get_pipe_mte2_idle_slot_count、asc_get_pipe_mte3_idle_slot_count、asc_get_pipe_v_idle_slot_count、asc_get_pipe_m_idle_slot_count、asc_get_pipe_mte1_idle_slot_count、asc_get_pipe_fix_idle_slot_count](asc_get_pipe_idle_slot_count.md) | 获取指定流水线对应发射队列当前的空闲槽位数量。 | `c_api/experimental/asc_get_pipe_idle_slot_count.h` |
| [asc_add_carry](asc_add_carry.md) | 按掩码执行32位逐元素加法，同时返回加法结果和进位标志，支持带输入进位的运算。 | `c_api/experimental/reg_arith_exp.h` |
| [asc_sub_carry](asc_sub_carry.md) | 按掩码执行32位逐元素减法，同时返回减法结果和进位标志；进位为1表示无借位，为0表示有借位。 | `c_api/experimental/reg_arith_exp.h` |
| [asc_mull](asc_mull.md) | 按掩码将32位元素相乘，分别返回64位乘积的低32位和高32位。 | `c_api/experimental/reg_arith_exp.h` |
| [asc_intlv](asc_intlv.md) | 对两个源矢量数据寄存器或掩码寄存器进行交织，返回两个寄存器结果。 | `c_api/experimental/reg_permute_sel_exp.h` |
| [asc_deintlv](asc_deintlv.md) | 对两个源矢量数据寄存器或掩码寄存器进行解交织，返回两个寄存器结果。 | `c_api/experimental/reg_permute_sel_exp.h` |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | 从UB的32字节对齐地址读取2×VL长度数据，分别返回偶数下标和奇数下标元素。 | `c_api/experimental/loadalign_exp.h` |

### 相关类型

**表2** 相关类型

| 名称 | 描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [双返回值结构体](pair.md) | 通过`first`和`second`两个成员承载试验接口返回的两个寄存器结果。 | `c_api/experimental/pair.h` |
