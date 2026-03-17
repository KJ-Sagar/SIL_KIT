function payload = encode_vehicle_signals(signals)
%ENCODE_VEHICLE_SIGNALS Pack ignition/moving/hazard into first byte.
% bit0 (LSB): ignition
% bit1: vehicleMoving
% bit2: hazardActive

b = uint8(0);
b = bitset(b, 1, logical(signals.ignition));
b = bitset(b, 2, logical(signals.vehicleMoving));
b = bitset(b, 3, logical(signals.hazardActive));

payload = zeros(1, 8, 'uint8');
payload(1) = b;
end
