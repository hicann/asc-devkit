# How to Use Workspace<a name="ZH-CN_TOPIC_0000001566726840"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:19:34.619Z pushedAt=2026-08-31T11:25:05.567Z -->

Workspace is a block of memory in the Global Memory on the device side. Workspace memory consists of two parts: system workspace and user workspace.

-   System workspace: workspace memory that needs to be reserved for Ascend C APIs

    During computation, APIs require some workspace memory as cache. Therefore, the operator needs to reserve workspace memory for the APIs. The size of the reserved memory is obtained through the [GetLibApiWorkSpaceSize](../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md) API.

-   User workspace: workspace memory used by the operator implementation

    User workspace needs to be allocated only when the operator requires additional device memory for data exchange or caching. Allocate it as needed. The usage scenarios are as follows:

    -   When the space on the Unified Buffer and L1 Buffer is required but insufficient, data can be temporarily stored in the workspace.
    -   When calling APIs such as [SyncAll](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll.md), workspace is required as an input parameter.
    -   Other scenarios where memory space in the Global Memory is required.

The specific usage methods under different development approaches are as follows:

-   Engineering-based operator development

    In the tiling function, first call the GetWorkspaceSizes API to obtain the storage location of the workspace size, and then set the workspace size. The framework allocates the corresponding size of device-side Global Memory, which can be used in the kernel-side implementation of the operator. When using high-level APIs that require system workspace, such as the [Matmul Kernel API](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_kernel_api.md), the workspace size to be set is the sum of the system workspace and the user workspace.

    ```
    // User-defined tiling function
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        AddApiTiling tiling;
        ...
        size_t usrSize = 256; // Set the workspace size required by the user to 256 bytes.
        // To use the system workspace, call GetLibApiWorkSpaceSize to obtain the system workspace size.
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        uint32_t sysWorkspaceSize = ascendcPlatform.GetLibApiWorkSpaceSize();
        size_t *currentWorkspace = context->GetWorkspaceSizes(1); // Obtain the workspace pointer through the framework. The input parameter of GetWorkspaceSizes is the number of workspace blocks required. Currently, only one block is supported.
        currentWorkspace[0] = usrSize + sysWorkspaceSize; // Set the total workspace size. The total workspace is allocated and managed by the framework.
        ...
    }
    ```

    The workspace at the device-side kernel entry is the user workspace pointer:

    ```
    // User-defined kernel function. The kernel function must include the GM_ADDR workspace parameter, which must be placed before tiling.
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        ...
        
    }
    ```

-   Kernel launch operator development scenario

When workspace is required, you are advised to enable the compilation option [HAVE\_WORKSPACE](../completing_kernel_launch_based_on_sample_project.md). The host-side developer still needs to allocate the workspace and pass it in. When using advanced APIs that require system workspace, such as the Matmul kernel API, the workspace size to be set is the sum of the system workspace and the user workspace. The system workspace size can be obtained through the GetLibApiWorkSpaceSize API of PlatformAscendCManager. After [HAVE\_WORKSPACE](../completing_kernel_launch_based_on_sample_project.md) is enabled, the workspace obtained by the developer at the kernel-side input parameter is the user workspace offset by the system workspace.
