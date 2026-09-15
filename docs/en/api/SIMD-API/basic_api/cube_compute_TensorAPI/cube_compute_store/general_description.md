# General Description of Cube Computation Storage

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:11:07.396Z -->

The storage API of cube computation mainly implement efficient data transfer between the L0C Buffer and Global Memory or the Unified Buffer.

For Ascend 950PR/Ascend 950DT:

**Figure 1** Overall flowchart of cube computation storage

![Overall flowchart of cube computation storage](../../../../figures/fixpipe-20.png)

**Table 1** Data paths and storage levels

|Source Location|Source Address Alignment Requirement|destination location|Destination Address Alignment Requirement|format conversion|Typical Application Scenario|
|--------|--------|--------|--------|--------|--------|
|L0C Buffer|64 bytes|Global Memory|1 byte/32 bytes|NZ2ND, NZ2DN, NZ2NZ|Moves data from the L0C Buffer to Global Memory.|
|L0C Buffer|64 bytes|Unified Buffer|32 bytes|NZ2ND, NZ2DN, NZ2NZ|Moves data from the L0C Buffer to the Unified Buffer.|
