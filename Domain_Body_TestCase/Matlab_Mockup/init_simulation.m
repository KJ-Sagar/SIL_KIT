function [state, bus] = init_simulation(testCaseId)
%INIT_SIMULATION Initialize state and channels for one test case run.

switch testCaseId
    case 1
        vehicleSignals = struct('ignition', false, 'vehicleMoving', false, 'hazardActive', true);
        trunkRequest = uint8(128);  % Open request
        expectedTrunk = uint8(128); % Open expected
    case 2
        vehicleSignals = struct('ignition', true, 'vehicleMoving', false, 'hazardActive', true);
        trunkRequest = uint8(0);    % Close request
        expectedTrunk = uint8(0);   % Close expected
    case 3
        vehicleSignals = struct('ignition', true, 'vehicleMoving', true, 'hazardActive', true);
        trunkRequest = uint8(128);  % Open request
        expectedTrunk = uint8(0);   % Must be forced closed while moving
    otherwise
        error('Unsupported test case id: %d', testCaseId);
end

state = struct();
state.testCaseId = testCaseId;
state.tick = 0;
state.maxTicks = 10;
state.done = false;

state.plant = struct(...
    'stimulusSent', false, ...
    'vehicleSignals', vehicleSignals, ...
    'expectedTrunk', expectedTrunk, ...
    'observedTrunk', uint8([]), ...
    'pass', false, ...
    'lastHazardPayload', uint8([]));

state.zonalA = struct('lastCanPayload', uint8([]));

state.hpc = struct(...
    'vehicleSignalsKnown', false, ...
    'vehicleSignals', struct('ignition', false, 'vehicleMoving', false, 'hazardActive', false), ...
    'trunkRequestKnown', false, ...
    'trunkRequest', uint8(0), ...
    'blinkOn', false);

state.zonalB = struct(...
    'lastHazardPayload', uint8([]), ...
    'lastTrunkPayload', uint8([]));

state.telematics = struct(...
    'commandByte', trunkRequest, ...
    'commandSent', false, ...
    'lastResponse', uint8([]));

bus = init_bus();
end
