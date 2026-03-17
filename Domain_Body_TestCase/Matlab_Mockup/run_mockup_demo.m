function results = run_mockup_demo()
%RUN_MOCKUP_DEMO Execute TC1/TC2/TC3 in a MATLAB-only architecture mockup.

fprintf('\nDomain Body Mockup Simulation\n');
fprintf('=============================================\n');

testCases = [1, 2, 3];
results = repmat(struct( ...
    'testCaseId', 0, ...
    'pass', false, ...
    'expectedTrunk', uint8(0), ...
    'observedTrunk', uint8([]), ...
    'ticksUsed', 0), 1, numel(testCases));

for i = 1:numel(testCases)
    tc = testCases(i);
    [state, bus] = init_simulation(tc);

    for tick = 1:state.maxTicks
        state.tick = tick;

        [state, bus] = plant_model_step(state, bus);
        [state, bus] = telematics_step(state, bus);
        [state, bus] = zonal_a_step(state, bus);
        [state, bus] = hpc_step(state, bus);
        [state, bus] = zonal_b_step(state, bus);
        [state, bus] = telematics_step(state, bus);
        [state, bus] = plant_model_step(state, bus);

        if state.done
            break;
        end
    end

    results(i).testCaseId = tc;
    results(i).pass = state.plant.pass;
    results(i).expectedTrunk = state.plant.expectedTrunk;
    results(i).observedTrunk = state.plant.observedTrunk;
    results(i).ticksUsed = tick;

    fprintf('TC%d -> expected=%s observed=%s status=%s (ticks=%d)\n', ...
        tc, ...
        trunk_status_label(results(i).expectedTrunk), ...
        trunk_status_label(results(i).observedTrunk), ...
        pass_fail_label(results(i).pass), ...
        tick);
end

fprintf('---------------------------------------------\n');
numPass = sum([results.pass]);
fprintf('Summary: %d/%d passed\n', numPass, numel(results));
fprintf('=============================================\n\n');
end

function label = pass_fail_label(isPass)
if isPass
    label = 'PASS';
else
    label = 'FAIL';
end
end
