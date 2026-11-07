# 概述

## API列表

### 系统初始化

**表1** 系统初始化接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_init](sys_init/asc_init.md) | 初始化NPU状态。 | `c_api/utils/sys_init.h` |

### 系统其他操作

**表2** 系统其他操作接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_nop](sys_misc/asc_nop.md) | 生成一条空操作指令，不执行实际计算。 | `c_api/utils/sys_misc.h` |

### 系统资源和变量

**表3** 系统资源和变量接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_get_arch_ver](sys_var/asc_get_arch_ver.md) | 获取当前AI处理器架构版本号。 | `c_api/utils/sys_var.h` |
| [asc_get_core_id](sys_var/asc_get_core_id.md) | 获取当前核的编号。 | `c_api/utils/sys_var.h` |
| [asc_get_ffts_base_addr](sys_var/asc_get_ffts_base_addr.md) | 获取核间同步寄存器的基地址。 | `c_api/utils/sys_var.h` |
| [asc_get_phy_buf_addr](sys_var/asc_get_phy_buf_addr.md) | 基于偏移量获取片上实际物理地址。 | `c_api/utils/sys_var.h` |
| [asc_get_phy_stack_base](sys_var/asc_get_phy_stack_base.md) | 获取物理堆栈基地址。 | `c_api/utils/sys_var.h` |
| [asc_get_program_counter](sys_var/asc_get_program_counter.md) | 获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。 | `c_api/utils/sys_var.h` |
| [asc_get_smmu_tag_version](sys_var/asc_get_smmu_tag_version.md) | 获取SMMU（System Memory Management Unit）版本信息。 | `c_api/utils/sys_var.h` |
| [asc_get_status](sys_var/asc_get_status.md) | 获取状态信息。 | `c_api/utils/sys_var.h` |
| [asc_get_sub_block_id](sys_var/asc_get_sub_block_id.md) | 获取AI Core上Vector核的ID。 | `c_api/utils/sys_var.h` |
| [asc_get_sub_block_num](sys_var/asc_get_sub_block_num.md) | 分离模式下，获取一个AI Core上Cube Core（AIC）或者Vector Core（AIV）的数量。 | `c_api/utils/sys_var.h` |
| [asc_get_sys_virtual_base](sys_var/asc_get_sys_virtual_base.md) | 获取系统虚拟基地址。 | `c_api/utils/sys_var.h` |
| [asc_get_system_cycle](sys_var/asc_get_system_cycle.md) | 获取当前系统cycle数。 | `c_api/utils/sys_var.h` |
| [asc_get_system_clock](sys_var/asc_get_system_clock.md) | 获取当前系统时钟时间值（微秒）。 | `c_api/utils/sys_var.h` |
| [asc_get_vf_len](sys_var/asc_get_vf_len.md) | 获取Tensor位宽VL（Vector Length）的大小。 | `c_api/utils/sys_var.h` |
| [asc_set_ffts_base_addr](sys_var/asc_set_ffts_base_addr.md) | 在[asc_sync_block_arrive](../sync/inter_core_sync/asc_sync_block_arrive.md)和[asc_sync_block_wait](../sync/inter_core_sync/asc_sync_block_wait.md)之前使用，设置核间同步寄存器的基地址。 | `c_api/utils/sys_var.h` |
