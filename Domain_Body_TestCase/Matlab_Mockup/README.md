# MATLAB Mockup Design (Domain Body Test Case)

This folder contains a MATLAB-only architecture mockup of the C++ SIL Kit setup.

## Goal

Provide a beginner-friendly, modular design you can run and extend while learning how the original distributed system works.

## Modeled participants

- `PlantModel`
- `ZonalA`
- `HPC`
- `ZonalB`
- `Telematics`

## Channels modeled as queues

- `CAN1`, `CAN2`, `CAN3`
- `Eth1`, `Eth2`, `Eth3`, `Eth4`

Each message includes:
- `source`
- `destination`
- payload (`data` for CAN, `raw` for Ethernet)

## Core files

- `run_mockup_demo.m`: runs test cases 1/2/3 and prints summary.
- `init_simulation.m`: initializes state for one test case.
- `plant_model_step.m`
- `zonal_a_step.m`
- `hpc_step.m`
- `zonal_b_step.m`
- `telematics_step.m`

Helpers:
- `init_bus.m`, `bus_push.m`, `bus_take_for.m`
- `create_eth_frame.m`, `extract_eth_payload.m`
- `encode_vehicle_signals.m`, `decode_vehicle_signals.m`
- `trunk_status_label.m`

## Policy in this mockup

- Trunk open request (`0x80`) is allowed only when vehicle is not moving.
- If moving and open is requested, HPC forces result to closed (`0x00`).
- Test case expectations:
  - TC1: open allowed
  - TC2: close remains close
  - TC3: open denied (forced close)

## How to run

In MATLAB:

```matlab
cd('/home/kj/CES_2025_artifacts/Domain_Body_TestCase/Matlab_Mockup');
results = run_mockup_demo();
```

## Next extension ideas

- Replace queue-based channels with UDP/MQTT adapters.
- Expand payload format to match exact byte maps from C++.
- Add timeline plotting per tick for trunk/hazard signals.
