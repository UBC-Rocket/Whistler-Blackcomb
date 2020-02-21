from digi.xbee.devices import XBeeDevice
from digi.xbee.devices import RemoteXBeeDevice
from digi.xbee.devices import XBee64BitAddress
import base64
import sys

inputString = sys.argv[1]
print(input)

device = XBeeDevice("COM18", 9600)
device.open()
print(device.get_64bit_addr())


stringSend = "Hello XBee World!"
device.send_data_async(remote_device, inputString)
print(stringSend)

device.close()
