function [state, bus] = plant_model_step(state, bus)
%PLANT_MODEL_STEP Publish initial CAN stimulus and evaluate returned CAN data.

if ~state.plant.stimulusSent
    payload = encode_vehicle_signals(state.plant.vehicleSignals);
    msg = struct('id', hex2dec('401'), ...
                 'data', payload, ...
                 'source', 'PlantModel', ...
                 'destination', 'ZonalA');
    bus = bus_push(bus, 'CAN1', msg);
    state.plant.stimulusSent = true;
end

[bus, can2Msgs] = bus_take_for(bus, 'CAN2', 'PlantModel');
if ~isempty(can2Msgs)
    state.plant.lastHazardPayload = can2Msgs{end}.data;
end

[bus, can3Msgs] = bus_take_for(bus, 'CAN3', 'PlantModel');
if ~isempty(can3Msgs)
    trunkPayload = can3Msgs{end}.data;
    if ~isempty(trunkPayload)
        state.plant.observedTrunk = uint8(trunkPayload(1));
        if state.plant.observedTrunk == state.plant.expectedTrunk
            state.plant.pass = true;
            state.done = true;
        end
    end
end

if state.tick >= state.maxTicks && ~state.done
    state.done = true;
    state.plant.pass = false;
end
end
