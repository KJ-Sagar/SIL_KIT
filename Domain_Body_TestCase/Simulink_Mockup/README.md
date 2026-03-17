# Simulink Mockup Starter

This folder gives you a **programmatic Simulink starter model** for your domain-body feature flow.

## What it builds

Model name (default): `DomainBody_Simulink_Mockup.slx`

Top-level subsystems:
- `PlantModel`
- `ZonalA`
- `HPC`
- `ZonalB`
- `Telematics`
- `Verifier`

Signal path:
- `PlantModel -> ZonalA -> HPC -> ZonalB -> Verifier`
- `Telematics -> HPC -> Telematics`

## Files

- `build_domain_body_simulink_model.m`:
  - Creates and wires the `.slx` model.
  - Populates each subsystem with a `MATLAB Function` block.
- `run_simulink_testcases.m`:
  - Runs TC1/TC2/TC3 by changing `TestCaseId`.
  - Prints PASS/FAIL summary.

## Usage (in MATLAB)

```matlab
cd('/home/kj/CES_2025_artifacts/Domain_Body_TestCase/Simulink_Mockup');
build_domain_body_simulink_model();
results = run_simulink_testcases();
```

## Design intent

This is a mock architecture to accelerate modeling.
It is intentionally byte-level and simple, so you can quickly replace internals with:
- full frame buses (`Bus Creator` + `Simulink.Bus` objects),
- Stateflow charts for ECU behavior,
- test harnesses (`Test Sequence` / Simulink Test),
- real communication adapters.

## Next recommended upgrades

1. Replace single-byte payloads with structured buses (`id`, `dlc`, `data[64]`).
2. Move HPC logic to Stateflow for clear state machine + blink timing.
3. Add assertions for trunk policy in each test case.
4. Add signal logging and dashboard controls.
