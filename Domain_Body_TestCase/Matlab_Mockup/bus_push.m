function bus = bus_push(bus, channelName, message)
%BUS_PUSH Append one message to a named channel queue.
queue = bus.(channelName);
queue{end+1} = message;
bus.(channelName) = queue;
end
