function signals = decode_vehicle_signals(payload)
%DECODE_VEHICLE_SIGNALS Unpack first-byte ignition/moving/hazard bits.
signals = struct('ignition', false, 'vehicleMoving', false, 'hazardActive', false);

if isempty(payload)
    return;
end

b = uint8(payload(1));
signals.ignition = logical(bitget(b, 1));
signals.vehicleMoving = logical(bitget(b, 2));
signals.hazardActive = logical(bitget(b, 3));
end
