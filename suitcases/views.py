from django.shortcuts import render
from django.urls import reverse, reverse_lazy
from django.views.generic.detail import DetailView
from .forms import RentForm
from django.http import HttpRequest,HttpResponse, HttpResponseRedirect  
from http import HTTPStatus
from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib import messages
from .models import Suitcase


class SuitcaseDisplayView(LoginRequiredMixin,DetailView):
    model = Suitcase
    template_name = "suitcase/get_suitcase.html"
    slug_url_kwarg = "uuid"
    slug_field = "uuid"
    login_url = reverse_lazy("login")

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["categories"] = self.object.getCategories()
        context["renter_uuid"] = "" 
        if(self.object.rented):
            renter_uuid = self.object.rented_by.uuid
            context["renter_uuid"] = renter_uuid 

        context["form"] = RentForm()
        context["user_uuid"] = self.request.user.uuid 

        context["encoded_key"] = None
        if not hasattr(self.object,"suitcasebleinfo"):
            return context
        context["encoded_key"] = self.object.suitcasebleinfo.get_hashed_secret_key() 

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
        if(suitcase.rented_by and suitcase.rented):
            if(suitcase.rented_by.uuid != self.request.user.uuid):
                return HttpResponse("You are not allowed to see this >(".encode(),HTTPStatus.BAD_REQUEST)
                
        self.object = suitcase  
        return render(request,self.template_name,self.get_context_data())


    def post(self,request:HttpRequest,uuid:str):

        #Man kan ikke lave put request med post form i html
        #så vi laver en field i post request hvor vi kan
        #specificere hvad for en method det er
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

        rent_err = request.user.rent_briefcase(suitcase,post_form) 

        if(rent_err):
            messages.error(request,rent_err) 
            return HttpResponseRedirect(request.path)


        #Ved ikke hvorfor men man skal åbenbart fetch suitcase igen med get
        #efter man har brugt save.
        #Prøv at fjerne linjen og se om det stadigvæk virker
        suitcase = Suitcase.objects.get(uuid=uuid)
        self.object = suitcase  
        return HttpResponseRedirect(request.path)

    def handle_put(self,request:HttpRequest,uuid:str):
        suitcase:Suitcase = Suitcase.objects.get(uuid=uuid)

        if(suitcase.rented):
            if(suitcase.rented_by.uuid != self.request.user.uuid):
                return HttpResponse("You are not allowed to unrent this >(".encode(),HTTPStatus.BAD_REQUEST)

        request.user.unrent_briefcase(suitcase)

        return HttpResponseRedirect(request.path)



