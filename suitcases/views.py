from hashlib import sha256
from django.shortcuts import render
from django.views.generic.detail import DetailView
from .models import Suitcase, SuitcaseBleInfo
from .forms import RentForm
from django.http import HttpRequest,HttpResponse, HttpResponseRedirect  
from http import HTTPStatus
import datetime
from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib import messages



class SuitcaseDisplayView(LoginRequiredMixin,DetailView):
    model = Suitcase
    template_name = "get_suitcase.html"
    slug_url_kwarg = "uuid"
    slug_field = "uuid"
    login_url = "/accounts/login"

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["categories"] = self.object.getCategories()
        context["renter_uuid"] = "" 
        if(self.object.rented):
            renter_uuid = self.object.rented_by.uuid
            context["renter_uuid"] = renter_uuid 

        context["form"] = RentForm()

        if(context["renter_uuid"] == self.request.user.uuid):
            context["form"] = ""

        context["user_uuid"] = self.request.user.uuid 
        context["encoded_key"] = "null"

        if hasattr(self.object,"suitcasebleinfo"):
            suitcase_ble_info:SuitcaseBleInfo = self.object.suitcasebleinfo
            m = sha256()
            m.update((str(suitcase_ble_info.secret_key) + str(suitcase_ble_info.msg_count)).encode())
            context["encoded_key"] = m.hexdigest()

        return context

    def handle_put(self,request:HttpRequest,uuid:str):

        suitcase:Suitcase = Suitcase.objects.get(uuid=uuid)

        if(suitcase.rented):
            if(suitcase.rented_by.uuid != self.request.user.uuid):
                return HttpResponse("You are not allowed to unrent this >(".encode(),HTTPStatus.BAD_REQUEST)

        suitcase.rented = False
        suitcase.rented_by = None 
        suitcase.rented_date= None 
        suitcase.expiration_date= None 

        suitcase.save()

        return HttpResponseRedirect(request.path)


    def get(self, request:HttpRequest, uuid:str):

        suitcase:Suitcase = Suitcase.objects.get(uuid=uuid)
        if(suitcase.rented):
            if(suitcase.rented_by.uuid != self.request.user.uuid):
                return HttpResponse("You are not allowed to see this >(".encode(),HTTPStatus.BAD_REQUEST)
                
        self.object = suitcase  # Set self.object explicitly here
        return render(request,self.template_name,self.get_context_data())


    def post(self,request:HttpRequest,uuid:str):
        real_method = request.POST.get("_method")
        if(real_method == "put"):
            return self.handle_put(request,uuid)
        post_form = RentForm(request.POST)

        if not post_form.is_valid():
            return HttpResponse("Invalid form".encode(),HTTPStatus.BAD_REQUEST)

        suitcase:Suitcase = Suitcase.objects.get(uuid = uuid)
        if(suitcase.rented):
            messages.error(request,"Suitcase is already rented") 

            return HttpResponseRedirect(request.path)


        suitcase.rented_by = self.request.user
        suitcase.rented = True 

        suitcase.rented_date = datetime.date.today()
        suitcase.expiration_date = post_form.cleaned_data["expiration_date"]
        if(suitcase.expiration_date <= suitcase.rented_date):
            messages.error(request,"Expiration date must be after today") 
            return HttpResponseRedirect(request.path)


        suitcase.save()

        suitcase = Suitcase.objects.get(uuid=uuid)
        self.object = suitcase  # Set self.object explicitly here



        return HttpResponseRedirect(request.path)



