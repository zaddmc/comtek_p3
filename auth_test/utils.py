
from django.http import HttpRequest, HttpResponse
from django.shortcuts import render


def show_error_page(request:HttpRequest,error_text:str,error_code:int) -> HttpResponse:
            ctx = {}
            ctx["error_text"] = error_text 
            ctx["error_code"] =error_code 
            return render(request,template_name="common/error.html",context=ctx)
