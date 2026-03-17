function frame = create_eth_frame(payload, srcMac, dstMac, etherType)
%CREATE_ETH_FRAME Build a minimal Ethernet frame: dst(6)+src(6)+type(2)+payload.
payload = uint8(payload(:).');
srcMac = uint8(srcMac(:).');
dstMac = uint8(dstMac(:).');

ethTypeHi = uint8(bitshift(uint16(etherType), -8));
ethTypeLo = uint8(bitand(uint16(etherType), uint16(255)));

frame = struct();
frame.raw = [dstMac, srcMac, ethTypeHi, ethTypeLo, payload];
end
