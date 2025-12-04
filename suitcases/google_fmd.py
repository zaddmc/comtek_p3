from dataclasses import dataclass
import json
import subprocess
@dataclass
class BriefcaseLocation:
    longitude: float
    latitude: float
    altitude: float

@dataclass
class BriefcaseFMDData:
    ephemeral_key:str  
    canonic_id:str  


@dataclass
class OutData:
    data:str|None
    error_msg:str|None


def fmd_get_briefcase_location(canonic_id:str) -> BriefcaseLocation | None:
    res = subprocess.run(
        ["../venv/bin/python", "fmd_get_locations.py", f"{canonic_id}"],
        stdout=subprocess.PIPE,
        cwd="GoogleFindMyTools"
        
    )

    out_json = res.stdout.decode("utf-8").replace("\n", "")

    print(out_json)
    out = OutData(**json.loads(out_json))
    if out.data == None: 
        print(out.error_msg)
        return None
    data = BriefcaseLocation(**json.loads(out.data))
    return data

def fmd_refresh_device_list() -> None | bool:
    res = subprocess.run(
        ["../venv/bin/python", "fmd_refresh_device_list.py"],
        stdout=subprocess.PIPE,
        cwd="GoogleFindMyTools"
        
    )
    out_json = res.stdout.decode("utf-8").replace("\n", "")

    print(out_json)
    out = OutData(**json.loads(out_json))
    if out.data == None: 
        print(out.error_msg)
        return None
    return True

def fmd_register_briefcase() -> BriefcaseFMDData | None:
    res = subprocess.run(
        ["../venv/bin/python", "fmd_register_device.py"],
        stdout=subprocess.PIPE,
        cwd="GoogleFindMyTools"
        
    )
    out_json = res.stdout.decode("utf-8").replace("\n", "")
    print(out_json)
    out = OutData(**json.loads(out_json))
    if out.data == None: 
        print(out.error_msg)
        return None
    data = BriefcaseFMDData(**json.loads(out.data))
    return data

def main():
    briefcase_location = None
    while briefcase_location == None:
        out_status = fmd_refresh_device_list()
        if out_status == None:
            return

        print("Getting location")
        briefcase_location = fmd_get_briefcase_location("692a0e8f-0000-2281-a61d-2405887036ac")
        #briefcase_location = fmd_get_briefcase_location("68a98641-0000-2671-82d5-34c7e91a39ca")
        if briefcase_location == None:
            print("No location")
            continue
        else:
            print(briefcase_location.latitude)
            print(briefcase_location.longitude)
            print(briefcase_location.altitude)



if __name__ == "__main__":
    main()
