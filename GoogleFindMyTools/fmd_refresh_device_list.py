import json
from ProtoDecoders.DeviceUpdate_pb2 import DevicesList
from out_classes import OutData
from NovaApi.ListDevices.nbe_list_devices import request_device_list
from ProtoDecoders.decoder import parse_device_list_protobuf
from SpotApi.UploadPrecomputedPublicKeyIds.upload_precomputed_public_key_ids import refresh_custom_trackers


def fmd_refresh_device_list() -> OutData:
    out_data= OutData(None,None)
    result_hex = request_device_list()
    device_list = parse_device_list_protobuf(result_hex)

    err = refresh_custom_trackers(device_list)
    if err != None:
        out_data.error_msg = f"Error refreshing device list{str(err)}"
        return out_data

    with open("text.txt","wb") as f:
        f.write(device_list.SerializeToString())
    out_data.data = "something"
    return out_data

def fmd_get_device_list():
    device_list = None
    with open("text.txt","rb") as f:
        file_content = f.read()
        device_list = DevicesList.FromString(file_content)
    return device_list

def main():
    out = fmd_refresh_device_list()
    print(json.dumps(out.__dict__))


if __name__ == "__main__":
    main()

