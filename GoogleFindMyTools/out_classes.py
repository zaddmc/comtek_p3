from dataclasses import dataclass
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


