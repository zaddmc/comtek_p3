import csv
from http import HTTPStatus
import json
from django.http import HttpRequest, HttpResponse
from django.views import View
from .misc import get_NACE_sector, request_company_CVR_cvr,request_company_CVR_name

class No(View):
    def get(self,request:HttpRequest):
        return HttpResponse("no".encode(),HTTPStatus.NOT_FOUND)

class CVRNameView(View):
    def get(self,request:HttpRequest,company_name:str):
        companies = request_company_CVR_name(company_name)
        if not companies:
            return HttpResponse(f"Could not find company with name {company_name}".encode(), HTTPStatus.NOT_FOUND)

        out_dict = {}
        out_dict["companies"] = companies

        out_str = json.dumps(out_dict)

        return HttpResponse(out_str.encode(),HTTPStatus.OK)

class CVRCVRView(View):
    def get(self,request:HttpRequest,cvr:int):
        companies = request_company_CVR_cvr(cvr)
        if not companies:
            return HttpResponse(f"Could not find company with cvr{cvr}".encode(), HTTPStatus.NOT_FOUND)

        out_dict = {}
        out_dict["companies"] = companies

        out_str = json.dumps(out_dict)

        return HttpResponse(out_str.encode(),HTTPStatus.OK)

class NACEView(View):
    def get(self,request:HttpRequest,industri_code:int):
        nace_info = None
        with open("static/misc/branche_bog.csv",newline="",encoding="cp1252") as csv_file:
            reader = csv.DictReader(csv_file,delimiter=";",quotechar='""')
            nace_info = get_NACE_sector(reader,industri_code)

        if not nace_info:
            return HttpResponse(f"Could not find NACE info for code: {industri_code}".encode(),HTTPStatus.NOT_FOUND)

        out_str = json.dumps(nace_info)

        return HttpResponse(out_str.encode(),HTTPStatus.OK)






