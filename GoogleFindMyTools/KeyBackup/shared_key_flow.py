#
#  GoogleFindMyTools - A set of tools to interact with the Google Find My API
#  Copyright © 2024 Leon Böttger. All rights reserved.
#
from KeyBackup.response_parser import get_fmdn_shared_key

def request_shared_key_flow():

    print("[SharedKeyFlow] Requesting Shared Key.")
    try:
        while True:
            # Check for alerts indicating JavaScript calls
            try:
                data = {"method":"setVaultSharedKeys","str":"111955209674853380513","vaultKeys":"{\"hw_protected\":[{\"epoch\":388,\"key\":{\"0\":94,\"1\":231,\"2\":95,\"3\":44,\"4\":191,\"5\":29,\"6\":198,\"7\":126,\"8\":242,\"9\":174,\"10\":174,\"11\":196,\"12\":45,\"13\":182,\"14\":202,\"15\":202,\"16\":193,\"17\":202,\"18\":221,\"19\":183,\"20\":24,\"21\":228,\"22\":91,\"23\":110,\"24\":40,\"25\":208,\"26\":79,\"27\":121,\"28\":122,\"29\":69,\"30\":132,\"31\":38}}],\"finder_hw\":[{\"epoch\":531,\"key\":{\"0\":186,\"1\":221,\"2\":179,\"3\":34,\"4\":17,\"5\":1,\"6\":184,\"7\":92,\"8\":185,\"9\":152,\"10\":162,\"11\":246,\"12\":221,\"13\":226,\"14\":20,\"15\":201,\"16\":154,\"17\":219,\"18\":194,\"19\":223,\"20\":106,\"21\":73,\"22\":243,\"23\":2,\"24\":46,\"25\":42,\"26\":64,\"27\":219,\"28\":60,\"29\":107,\"30\":98,\"31\":43}}]}"}
                if data['method'] == 'setVaultSharedKeys':
                    shared_key = get_fmdn_shared_key(data['vaultKeys'])
                    print("[SharedKeyFlow] Received Shared Key.")
                    return shared_key.hex()
                elif data['method'] == 'closeView':
                    print("[SharedKeyFlow] closeView() called. Closing browser.")
                    break

            except Exception as e:
                print("[SharedKeyFlow]", e)

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
   request_shared_key_flow()
