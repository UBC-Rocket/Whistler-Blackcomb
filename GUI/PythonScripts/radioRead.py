from digi.xbee.devices import XBeeDevice
from digi.xbee.devices import RemoteXBeeDevice
from digi.xbee.devices import XBee64BitAddress
import base64
import sys

PORT = "COM18"
BAUD_RATE = 9600

def main():
    sys.stdout.flush()
    device = XBeeDevice(PORT, BAUD_RATE)
    device.close()

    try:
        device.open()
        
        device.flush_queues()

        sys.stdout.flush()

        while True:
            xbee_message = device.read_data()
            if xbee_message is not None:
                #address = xbee_message.remote_device.get_64bit_addr()
                data = xbee_message.data.decode()
                #print("Received data from %s: %s\n" % (address, data))
                print(data)
                sys.stdout.flush()

    finally:
        if device is not None and device.is_open():
            device.close()

        



if __name__ == '__main__':
    main()


