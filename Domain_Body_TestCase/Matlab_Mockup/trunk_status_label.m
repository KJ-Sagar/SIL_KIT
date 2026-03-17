function label = trunk_status_label(statusByte)
%TRUNK_STATUS_LABEL Convert trunk status byte to readable text.
if isempty(statusByte)
    label = 'NONE';
    return;
end

if statusByte == uint8(128)
    label = 'OPEN';
else
    label = 'CLOSED';
end
end
