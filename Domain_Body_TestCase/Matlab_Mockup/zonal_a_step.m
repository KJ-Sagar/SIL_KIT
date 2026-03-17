function [state, bus] = zonal_a_step(state, bus)
%ZONAL_A_STEP Convert incoming CAN1 payloads to Ethernet frames for HPC (Eth4).

[bus, canMsgs] = bus_take_for(bus, 'CAN1', 'ZonalA');

for i = 1:numel(canMsgs)
    canMsg = canMsgs{i};
    payload = uint8(canMsg.data(:).');
    state.zonalA.lastCanPayload = payload;

    frame = create_eth_frame(payload, ...
        [102, 85, 68, 51, 34, 34], ...
        [102, 85, 68, 51, 34, 51], ...
        hex2dec('0800'));

    out = struct('raw', frame.raw, 'source', 'ZonalA', 'destination', 'HPC');
    bus = bus_push(bus, 'Eth4', out);
end
end
