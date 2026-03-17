function payload = extract_eth_payload(frame)
%EXTRACT_ETH_PAYLOAD Return payload bytes after 14-byte Ethernet header.
headerBytes = 14;
raw = frame.raw;

if numel(raw) <= headerBytes
    payload = uint8([]);
else
    payload = uint8(raw(headerBytes+1:end));
end
end
