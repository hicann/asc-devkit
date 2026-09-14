# Copying a Larger Data Block in a Single Transfer Whenever Possible<a name="ZH-CN_TOPIC_0000001893771813"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:19:56.765Z -->

[Priority] High

[Description] When copying data blocks of different sizes, the bandwidth utilization (effective bandwidth/theoretical bandwidth) differs. Based on measured experience, when the data length of a single transfer is 16KB or more, the optimal bandwidth performance can usually be achieved. Therefore, for a single transfer, you should copy a larger data block whenever possible. The following figures show the actual bandwidth variation under different data volumes measured on a certain AI processor.

>[!NOTE] Note
>Test data is related to the processor model, and slight fluctuations may occur during actual testing. The specific bandwidth values are not necessarily strictly consistent with the test data below.

**Figure 1** Actual bandwidth occupancy under different single transfer data volumes in the UB->GM direction<a name="fig925755124312"></a>  
![](../../../figures/ub_gm_direction_bandwidth_usage_change_for_different_single_movement_data_sizes.png "UB->GM direction actual bandwidth occupancy under different single transfer data volumes")

**Figure 2** Actual bandwidth occupancy under different single transfer data volumes in the GM->UB direction<a name="fig859712315453"></a>  
![](../../../figures/gm_ub_direction_bandwidth_usage_change_for_different_single_movement_data_sizes.png "GM->UB direction actual bandwidth occupancy under different single transfer data volumes")

