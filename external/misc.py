import csv
from typing import Any
import requests
from difflib import SequenceMatcher

class NaceInfo:
    def __init__(self,sector:str,title:str) -> None:
        self.sector = sector
        self.title = title
        pass

def get_NACE_sector(reader:csv.DictReader[str],industri_code:int) -> NaceInfo| None:
    for row  in reader: 
        #Bruger kode1 siden det koden uden , og .
        row_code = int(row["KODE1"])
        if(row_code == industri_code):
            assert("Hovedafdeling" in row)
            assert("NACE_TITEL_ENG" in row)
            return NaceInfo(row["Hovedafdeling"], row["NACE_TITEL_ENG"])
    return None

def request_company_CVR_name(company_name:str) -> list[dict[str,Any]] | None:
    search_url = "https://apicvr.dk/api/v1/fuzzy_search/company/"
    resp = requests.get(url=search_url + company_name)
    if(resp.status_code < 200 or resp.status_code >= 300):
        return None
    content:list[dict[str,Any]] = resp.json()
    content = sorted(content,key=lambda x:SequenceMatcher(None,x["name"],company_name).ratio(),reverse=True)
    return content

def request_company_CVR_cvr(cvr:int) -> list[dict[str,Any]] | None:
    search_url = "https://apicvr.dk/api/v1/"
    resp = requests.get(url=search_url + str(cvr))
    if(resp.status_code < 200 or resp.status_code >= 300):
        return None
    content:dict[str,Any] = resp.json()
    out_content= [{"name":content["name"],"cvr_number":cvr,"industrycode":content["industrycode"]}] 
    return out_content

