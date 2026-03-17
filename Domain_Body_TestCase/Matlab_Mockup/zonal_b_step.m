function [state, bus] = zonal_b_step(state, bus)
%ZONAL_B_STEP Convert Ethernet inputs from HPC to CAN outputs for PlantModel.

[bus, hazardMsgs] = bus_take_for(bus, 'Eth2', 'ZonalB');
for i = 1:numel(hazardMsgs)
    payload = extract_eth_payload(hazardMsgs{i});
    state.zonalB.lastHazardPayload = payload;

    canMsg = struct('id', hex2dec('101'), ...
                    'data', payload, ...
                    'source', 'ZonalB', ...
                    'destination', 'PlantModel');
    bus = bus_push(bus, 'CAN2', canMsg);
end

[bus, trunkMsgs] = bus_take_for(bus, 'Eth3', 'ZonalB');
for i = 1:numel(trunkMsgs)
    payload = extract_eth_payload(trunkMsgs{i});
    state.zonalB.lastTrunkPayload = payload;

    canMsg = struct('id', hex2dec('103'), ...
                    'data', payload, ...
                    'source', 'ZonalB', ...
                    'destination', 'PlantModel');
    bus = bus_push(bus, 'CAN3', canMsg);
end
end
