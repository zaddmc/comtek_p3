from django.views.generic import ListView 
from django.contrib.auth.mixins import LoginRequiredMixin

from suitcases.models import Suitcase

class NonRentedView(LoginRequiredMixin,ListView):
    model = Suitcase 
    template_name = "index.html"
    login_url = "/accounts/login"


    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        print(context.keys())

        context["object_list"] = [obj for obj in context["object_list"] if not obj.rented]
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

        context["object_list"] = [obj for obj in context["object_list"] if obj.rented and obj.rented_by.uuid == self.request.user.uuid]

        return context




