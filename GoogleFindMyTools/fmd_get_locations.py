import asyncio
import hashlib
import json
import sys
from time import time
from Auth.fcm_receiver import FcmReceiver
import fmd_refresh_device_list
from NovaApi.ExecuteAction.LocateTracker.decrypt_locations import is_mcu_tracker, retrieve_identity_key
from NovaApi.ExecuteAction.LocateTracker.location_request import MAX_TIMEOUT_S, create_location_request, get_location_data_for_device
from NovaApi.nova_request import nova_request
from NovaApi.scopes import NOVA_ACTION_API_SCOPE
from NovaApi.util import generate_random_uuid
from ProtoDecoders.decoder import get_canonic_ids,  parse_device_update_protobuf
from SpotApi.CreateBleDevice.create_ble_device import register_esp32
from ProtoDecoders import Common_pb2, DeviceUpdate_pb2
from NovaApi.ExecuteAction.LocateTracker.decrypted_location import WrappedLocation
from KeyBackup.cloud_key_decryptor import decrypt_aes_gcm
from FMDNCrypto.foreign_tracker_cryptor import decrypt
from out_classes import BriefcaseFMDData, BriefcaseLocation, OutData





def fmd_get_briefcase_location(canonic_id:str) -> OutData:
    out_data = OutData(None,None)
    device_list =fmd_refresh_device_list.fmd_get_device_list()
    if device_list == None:
        out_data.error_msg = "Device list is none"
        return out_data 

    canonic_ids = get_canonic_ids(device_list)
    canonic_id_exists = False

    for local_canonic_id in canonic_ids:
        if canonic_id == local_canonic_id[1]:
            canonic_id_exists = True
            break

    if not canonic_id_exists:
        out_data.error_msg = "No matching canonic id"
        return out_data

    result = None
    request_uuid = generate_random_uuid()

    def handle_location_response(response):
        nonlocal result
        device_update = parse_device_update_protobuf(response)

        if device_update.fcmMetadata.requestUuid == request_uuid:
            result = parse_device_update_protobuf(response)

    fcm_token = FcmReceiver().register_for_location_updates(handle_location_response)

    hex_payload = create_location_request(canonic_id, fcm_token, request_uuid)
    nova_request(NOVA_ACTION_API_SCOPE, hex_payload)


    #start_time = time()

    while result is None :
        asyncio.get_event_loop().run_until_complete(asyncio.sleep(0.1))

    if result == None:
        out_data.error_msg = "Max timeout reached while getting location data for device"
        return out_data

    device_registration = result.deviceMetadata.information.deviceRegistration

    identity_key = retrieve_identity_key(device_registration)
    locations_proto = result.deviceMetadata.information.locationInformation.reports.recentLocationAndNetworkLocations
    is_mcu = is_mcu_tracker(device_registration)

    # At All Areas Reports or Own Reports
    recent_location = locations_proto.recentLocation
    recent_location_time = locations_proto.recentLocationTimestamp

    # High Traffic Reports
    network_locations = list(locations_proto.networkLocations)
    network_locations_time = list(locations_proto.networkLocationTimestamps)

    if locations_proto.HasField("recentLocation"):
        network_locations.append(recent_location)
        network_locations_time.append(recent_location_time)

    if not len(network_locations_time) or not len(network_locations):
        out_data.error_msg = "No data received"
        return out_data

    latest_data =None 
    last_network= list(zip(network_locations,network_locations_time))[-1]
    loc, network_time = last_network
    if loc.status == Common_pb2.Status.SEMANTIC:
        wrapped_location = WrappedLocation(
            decrypted_location=b'',
            time=int(network_time.seconds),
            accuracy=0,
            status=loc.status,
            is_own_report=True,
            name=loc.semanticLocation.locationName
        )
        latest_data = wrapped_location
    else:

        encrypted_location = loc.geoLocation.encryptedReport.encryptedLocation
        public_key_random = loc.geoLocation.encryptedReport.publicKeyRandom

        if public_key_random == b"":  # Own Report
            identity_key_hash = hashlib.sha256(identity_key).digest()
            decrypted_location = decrypt_aes_gcm(identity_key_hash, encrypted_location)
        else:
            time_offset = 0 if is_mcu else loc.geoLocation.deviceTimeOffset
            decrypted_location = decrypt(identity_key, encrypted_location, public_key_random, time_offset)

        wrapped_location = WrappedLocation(
            decrypted_location=decrypted_location,
            time=int(network_time.seconds),
            accuracy=loc.geoLocation.accuracy,
            status=loc.status,
            is_own_report=loc.geoLocation.encryptedReport.isOwnReport,
            name=""
        )
        latest_data = wrapped_location

    if not latest_data:
        out_data.error_msg = "Latest data is none"
        return out_data 
     
    if latest_data.status == Common_pb2.Status.SEMANTIC:
        out_data.error_msg = "Latest data is sementic report"
        return out_data 
    proto_loc = DeviceUpdate_pb2.Location()
    proto_loc.ParseFromString(latest_data.decrypted_location)

    latitude = proto_loc.latitude / 1e7
    longitude = proto_loc.longitude / 1e7
    altitude = proto_loc.altitude

    briefcase_location = BriefcaseLocation(latitude,longitude,altitude)

    out_data.data = json.dumps(briefcase_location.__dict__)
    return out_data 

def main():
    briefcase_location = fmd_get_briefcase_location(sys.argv[1])

    briefcase_location_json = json.dumps(briefcase_location.__dict__)
    print(briefcase_location_json)

if __name__ == "__main__":
    main()

