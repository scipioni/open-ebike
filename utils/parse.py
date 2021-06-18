import json
import sys

with open(sys.argv[1]) as f:
    data  = json.load(f)

pp = {}
processed = []
for row in data:
    packet = row['_source']['layers']
    if 'btatt' in packet:
        #print(packet['btatt'].keys())
        if 'btatt.value' in packet['btatt'] and packet['btatt']['btatt.handle']=='0x00000021':
            date = packet['frame']["frame.time"][13:21]
            value = packet['btatt']['btatt.value']

            key = value[0:5]
            if key in ('00:03','01:05','01:10', '00:01', '01:00', '01:01', '00:05','01:0c','01:07','01:02','01:04','00:06'):
                continue
            if key != '00:0c': # battery SOC
                pass
                continue

            if not key in pp:
                pp[key] = []

            flag = value.split(':')[2]
            #flag = int(value.split(':')[3] + value.split(':')[2], 16)
            if flag in processed:
                continue
            processed.append(flag)
            pp[key].append((value, date))
            #print(date, packet['frame']["frame.time"][13:21], value)

for command in pp:
    print(command)
    for row in pp[command]:
        value, date = row
        print(' %s %s' % (date, value))

print(sorted(processed))
print(f'commands: {pp.keys()}')