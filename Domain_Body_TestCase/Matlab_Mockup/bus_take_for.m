function [bus, messages] = bus_take_for(bus, channelName, receiver)
%BUS_TAKE_FOR Pop all messages addressed to receiver from one channel.
queue = bus.(channelName);
messages = {};
keep = {};

for i = 1:numel(queue)
    msg = queue{i};
    if isfield(msg, 'destination') && strcmp(msg.destination, receiver)
        messages{end+1} = msg; %#ok<AGROW>
    else
        keep{end+1} = msg; %#ok<AGROW>
    end
end

bus.(channelName) = keep;
end
