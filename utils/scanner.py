
"""
pip install bleak

https://github.com/hbldh/bleak/blob/develop/examples/sensortag.py
"""

import asyncio
import platform
import logging
import sys
import subprocess
import codecs
from bleak import BleakClient
from bleak.backends.scanner import AdvertisementData
from bleak import BleakScanner
from bleak.backends.device import BLEDevice

#mac_addr = '30:AE:A4:FF:45:3E' # esp32 TTGO
mac_addr = 'DC:9E:23:1F:92:96' # turbo levo

try:
    mac_addr = sys.argv[1]
except:
    pass


def bytes2str(value):
    value_str = "".join(map(chr, value))
    try:
        return codecs.decode(value_str[:-1], "hex").decode("utf-8")
    except:
        return ''

def adv_callback(device: BLEDevice, advertisement_data: AdvertisementData):
    if advertisement_data.local_name in ('SPECIALIZED',"BV9800Pro"):
        print(device.address, "RSSI:", device.rssi, advertisement_data)

async def keypress_handler(sender, data):
    print("{0}: {1}".format(sender, data))

async def print_services(mac_addr: str):
    async with BleakClient(mac_addr) as client:
        svcs = await client.get_services()
        for service in svcs:
            print("service: %s" % service)
        print()
        for uid in (
            # "00002a00-0000-1000-8000-00805f9b34fb",
            # "00002a01-0000-1000-8000-00805f9b34fb",
            # "00002a04-0000-1000-8000-00805f9b34fb",
            # "00002a28-0000-1000-8000-00805f9b34fb",
            # "00002a29-0000-1000-8000-00805f9b34fb",
            # "00000013-0000-4b49-4e4f-525441474947",
            "00000011-0000-4b49-4e4f-525441474947",
            "00000001-0000-4b49-4e4f-525441474947",
            "00000021-0000-4b49-4e4f-525441474947",
            "00000002-0000-4b49-4e4f-525441474947",
            "00000012-0000-4b49-4e4f-525441474947",
            "00000003-0000-4b49-4e4f-525441474947",
            "00000013-0000-4b49-4e4f-525441474947",
            ):
            try:
                value = await client.read_gatt_char(uid)
                print("char {}: {} {}".format(uid, value, bytes2str(value)))
            except:
                print("char %s not found" % uid)

        #await client.start_notify(KEY_PRESS_UUID, keypress_handler)
        #await asyncio.sleep(5.0)
        #await client.stop_notify(KEY_PRESS_UUID)

async def scan_adv():
    scanner = BleakScanner()
    scanner.register_detection_callback(adv_callback)

    print("scanner start")
    await scanner.start()
    await asyncio.sleep(3.0)
    print("scanner stop")
    await scanner.stop()

async def run(address, debug=False):
    log = logging.getLogger(__name__)
    if debug:
        import sys

        log.setLevel(logging.DEBUG)
        h = logging.StreamHandler(sys.stdout)
        h.setLevel(logging.DEBUG)
        log.addHandler(h)

    async with BleakClient(address) as client:
        log.info(f"Connected: {client.is_connected}")

        for service in client.services:
            print(f"[Service] {service}")
            for char in service.characteristics:
                if "read" in char.properties:
                    try:
                        value = bytes(await client.read_gatt_char(char.uuid))
                        print(
                            f"\t[Characteristic] {char} ({','.join(char.properties)}), Value: {value} '{bytes2str(value)}'"
                        )
                    except Exception as e:
                        print(
                            f"\t[Characteristic] {char} ({','.join(char.properties)}), Value: {e}"
                        )
                else:
                    value = None
                    print(f"\t[Characteristic] {char} ({','.join(char.properties)}), Value: {value}"
                    )

                for descriptor in char.descriptors:
                    try:
                        value = bytes(
                            await client.read_gatt_descriptor(descriptor.handle)
                        )
                        value_str = "".join(map(chr, value))
                        print(f"\t\t[Descriptor] {descriptor}) | Value: {value} '{value_str}'")
                    except Exception as e:
                        print(f"\t\t[Descriptor] {descriptor}) | Value: {e}")
    subprocess.run(["bluetoothctl", "--", "remove", address], stdout=subprocess.DEVNULL)

async def print_handles(mac_addr: str):
    async with BleakClient(mac_addr) as client:
        value = bytes(await client.read_gatt_char(25))
        print(
            f" Value: {value} '{bytes2str(value)}'"
            )

loop = asyncio.get_event_loop()
loop.run_until_complete(scan_adv())
#loop.run_until_complete(run(mac_addr, False))
loop.run_until_complete(print_services(mac_addr))
#loop.run_until_complete(print_handles(mac_addr))
