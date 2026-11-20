# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-18T07:40:48.309701+00:00 pushedAt=2026-11-18T07:42:30.257Z nl=2 -->

## API List

### System Initialization

**Table 1** System initialization APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_init](sys_init/asc_init.md) | Initializes the NPU state. | `c_api/utils/sys_init.h` |

### Other System Operations

**Table 2** Other system operation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_nop](sys_misc/asc_nop.md) | Generates a no-operation instruction without performing actual computation. | `c_api/utils/sys_misc.h` |

### System Resources and Variables

**Table 3** System resource and variable APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_get_arch_ver](sys_var/asc_get_arch_ver.md) | Obtains the architecture version number of the current AI processor. | `c_api/utils/sys_var.h` |
| [asc_get_core_id](sys_var/asc_get_core_id.md) | Obtains the ID of the current core. | `c_api/utils/sys_var.h` |
| [asc_get_ffts_base_addr](sys_var/asc_get_ffts_base_addr.md) | Obtains the base address of the inter-core synchronization register. | `c_api/utils/sys_var.h` |
| [asc_get_phy_buf_addr](sys_var/asc_get_phy_buf_addr.md) | Obtains the actual on-chip physical address based on the offset. | `c_api/utils/sys_var.h` |
| [asc_get_phy_stack_base](sys_var/asc_get_phy_stack_base.md) | Obtains the base address of the physical stack. | `c_api/utils/sys_var.h` |
| [asc_get_program_counter](sys_var/asc_get_program_counter.md) | Obtains the pointer to the program counter, which records the current program execution position. | `c_api/utils/sys_var.h` |
| [asc_get_smmu_tag_version](sys_var/asc_get_smmu_tag_version.md) | Obtains the SMMU (System Memory Management Unit) version information. | `c_api/utils/sys_var.h` |
| [asc_get_status](sys_var/asc_get_status.md) | Obtains status information. | `c_api/utils/sys_var.h` |
| [asc_get_sub_block_id](sys_var/asc_get_sub_block_id.md) | Obtains the ID of the Vector Core on the AI Core. | `c_api/utils/sys_var.h` |
| [asc_get_sub_block_num](sys_var/asc_get_sub_block_num.md) | In separate mode, obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on an AI Core. | `c_api/utils/sys_var.h` |
| [asc_get_sys_virtual_base](sys_var/asc_get_sys_virtual_base.md) | Obtains the system virtual base address. | `c_api/utils/sys_var.h` |
| [asc_get_system_cycle](sys_var/asc_get_system_cycle.md) | Obtains the current system cycle count. | `c_api/utils/sys_var.h` |
| [asc_get_system_clock](sys_var/asc_get_system_clock.md) | Obtains the current system clock time value (in microseconds). | `c_api/utils/sys_var.h` |
| [asc_get_vf_len](sys_var/asc_get_vf_len.md) | Obtains the Tensor bit width VL (Vector Length). | `c_api/utils/sys_var.h` |
| [asc_set_ffts_base_addr](sys_var/asc_set_ffts_base_addr.md) | Used before [asc_sync_block_arrive](../sync/inter_core_sync/asc_sync_block_arrive.md) and [asc_sync_block_wait](../sync/inter_core_sync/asc_sync_block_wait.md) to set the base address of the inter-core synchronization register. | `c_api/utils/sys_var.h` |
