
import json
import fmd_refresh_device_list
from ProtoDecoders.decoder import get_canonic_ids
from SpotApi.CreateBleDevice.create_ble_device import register_esp32
from out_classes import BriefcaseFMDData, OutData


def fmd_register_briefcase() -> OutData:
    print("a")
    device_list =fmd_refresh_device_list.fmd_get_device_list()
    out_data = OutData(None,None)
    if device_list == None:
        out_data.error_msg = "Device list is none"
        return out_data 

    print("b")
    ephemeral_key = register_esp32()
    print("c")
    out =fmd_refresh_device_list.fmd_refresh_device_list()
    print("d")
    if out_data.data:
        return out

    print("e")
    device_list =fmd_refresh_device_list.fmd_get_device_list()

    print("f")
    canonic_ids = get_canonic_ids(device_list)

    print("g")
    newest_canonic_id = canonic_ids[-1]

    briefcase_data = BriefcaseFMDData(ephemeral_key=ephemeral_key,canonic_id=newest_canonic_id[1])

    out_data.data = json.dumps(briefcase_data.__dict__)

    print("h")

    return out_data 

def main():
    out_data = fmd_register_briefcase()
    out_data_json = json.dumps(out_data.__dict__)
    print(out_data_json)

if __name__ == "__main__":
    main()
