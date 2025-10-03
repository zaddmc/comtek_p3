from django.http import HttpRequest, HttpResponse
from django.views.generic import ListView, View 
from django.contrib.auth.mixins import LoginRequiredMixin

from suitcases.models import Suitcase


# Create your views here.

class ClassicView(LoginRequiredMixin,ListView):
    model = Suitcase 
    template_name = "index.html"
    login_url = "/accounts/login"


    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        context["username"] = self.request.user.username
        return context

class RentedView(LoginRequiredMixin,ListView):
    model = Suitcase 
    template_name = "rented.html"
    login_url = "/accounts/login"


    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        context["username"] = self.request.user.username
        context["user_uuid"] = self.request.user.uuid
        return context

class TestView(View):
    def get(self,request:HttpRequest):
        return HttpResponse(status=200)




