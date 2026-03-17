function [state, bus] = telematics_step(state, bus)
%TELEMATICS_STEP Send one trunk command to HPC and receive trunk response.

if ~state.telematics.commandSent
    requestPayload = uint8(state.telematics.commandByte);

    frame = create_eth_frame(requestPayload, ...
        [102, 85, 68, 51, 34, 85], ...
        [102, 85, 68, 51, 34, 51], ...
        hex2dec('0800'));

    out = struct('raw', frame.raw, 'source', 'Telematics', 'destination', 'HPC');
    bus = bus_push(bus, 'Eth1', out);
    state.telematics.commandSent = true;
end

[bus, responseMsgs] = bus_take_for(bus, 'Eth1', 'Telematics');
if ~isempty(responseMsgs)
    payload = extract_eth_payload(responseMsgs{end});
    if ~isempty(payload)
        state.telematics.lastResponse = uint8(payload(1));
    end
end
end
