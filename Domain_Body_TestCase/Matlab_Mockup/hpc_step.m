function [state, bus] = hpc_step(state, bus)
%HPC_STEP Merge vehicle and trunk request inputs, apply policy, fan out outputs.

[bus, fromZonalA] = bus_take_for(bus, 'Eth4', 'HPC');
if ~isempty(fromZonalA)
    payload = extract_eth_payload(fromZonalA{end});
    state.hpc.vehicleSignals = decode_vehicle_signals(payload);
    state.hpc.vehicleSignalsKnown = true;
end

[bus, fromTelematics] = bus_take_for(bus, 'Eth1', 'HPC');
if ~isempty(fromTelematics)
    payload = extract_eth_payload(fromTelematics{end});
    if ~isempty(payload)
        state.hpc.trunkRequest = uint8(payload(1));
        state.hpc.trunkRequestKnown = true;
    end
end

if state.hpc.vehicleSignalsKnown && state.hpc.vehicleSignals.hazardActive
    state.hpc.blinkOn = ~state.hpc.blinkOn;
    if state.hpc.blinkOn
        hazardByte = uint8(254);
    else
        hazardByte = uint8(0);
    end

    hazardFrame = create_eth_frame(hazardByte, ...
        [102, 85, 68, 51, 34, 51], ...
        [102, 85, 68, 51, 34, 17], ...
        hex2dec('0800'));

    hazardMsg = struct('raw', hazardFrame.raw, 'source', 'HPC', 'destination', 'ZonalB');
    bus = bus_push(bus, 'Eth2', hazardMsg);
end

if state.hpc.vehicleSignalsKnown && state.hpc.trunkRequestKnown
    reqOpen = (state.hpc.trunkRequest == uint8(128));
    isMoving = state.hpc.vehicleSignals.vehicleMoving;

    if reqOpen && isMoving
        trunkResult = uint8(0);
    else
        trunkResult = uint8(state.hpc.trunkRequest);
    end

    trunkFrame = create_eth_frame(trunkResult, ...
        [102, 85, 68, 51, 34, 51], ...
        [102, 85, 68, 51, 34, 17], ...
        hex2dec('0800'));

    toZonalB = struct('raw', trunkFrame.raw, 'source', 'HPC', 'destination', 'ZonalB');
    bus = bus_push(bus, 'Eth3', toZonalB);

    responseFrame = create_eth_frame(trunkResult, ...
        [102, 85, 68, 51, 34, 51], ...
        [102, 85, 68, 51, 34, 85], ...
        hex2dec('0800'));

    toTelematics = struct('raw', responseFrame.raw, 'source', 'HPC', 'destination', 'Telematics');
    bus = bus_push(bus, 'Eth1', toTelematics);
end
end
