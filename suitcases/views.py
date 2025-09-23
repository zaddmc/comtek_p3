from django.views.generic.detail import DetailView
from django.views.generic import View 
from .models import Suitcase
from django.http import HttpRequest,HttpResponse
from django.template.loader import render_to_string
import datetime
import json


class SuitcaseDisplayView(DetailView):
    model = Suitcase
    template_name = "get_suitcase.html"
    slug_url_kwarg = "uuid"
    slug_field = "uuid"

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)

        context["renter"] = "" 
        if(self.object.rented):
            context["renter"] = self.object.rented_by.uuid

        return context

class SuitcaseGetView(View):
    def get(self,request:HttpRequest,uuid:str):
        suitcase:Suitcase = Suitcase.objects.get(uuid = uuid)


        print(suitcase.rented_by.uuid)

        out_obj = {"uuid": str(suitcase.uuid),"rented":suitcase.rented,"rented_by":str(suitcase.rented_by.uuid),"rendted_date":suitcase.rented_date,"expiration_date":suitcase.expiration_date}
        

        out_obj_json = json.dumps(out_obj)

        out_obj_str = str(out_obj_json).encode() 



        return HttpResponse(out_obj_str,content_type="application/json")



