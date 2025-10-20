from http import HTTPStatus
from django.contrib.auth.views import LoginView
from django.http import HttpRequest, HttpResponse, HttpResponseRedirect
from django.views.generic import CreateView

from accounts.models import User, UserInfo
from .forms import CustomUserCreate


class SignUpView(CreateView):
    form_class = CustomUserCreate 
    success_url = "/" 
    template_name = "registration/signup.html"
    def get(self, request:HttpRequest, *args, **kwargs):
        if(self.request.user.is_authenticated):
            return HttpResponseRedirect("/")
        return super().get(request, *args, **kwargs)
    def post(self,request:HttpRequest):
        post_form = request.POST
        custom_user_create_form = CustomUserCreate(post_form)
        if not custom_user_create_form.is_valid():
            return HttpResponse("Not valid form".encode(),HTTPStatus.BAD_REQUEST)
        user_info = UserInfo()
        company_cvr_int = 0
        company_industri_code_int = 0
        try:
            company_cvr_int = int(custom_user_create_form.cleaned_data["company_cvr"])
            company_industri_code_int= int(custom_user_create_form.cleaned_data["company_industri_code"])
        except:
            return HttpResponse("Not valid form".encode(),HTTPStatus.BAD_REQUEST)
        user:User = custom_user_create_form.save(commit=False)
        user.save()

        user_info.company_cvr = company_cvr_int 
        user_info.company_industri_code = company_industri_code_int 
        user_info.company_name= custom_user_create_form.cleaned_data["company_name"]
        user_info.user = user
        user_info.save()

        return HttpResponseRedirect("/") 

class CustLoginView(LoginView):
    def get(self, request, *args, **kwargs):
        if(self.request.user.is_authenticated):
            return HttpResponseRedirect("/")
        return super().get(request, *args, **kwargs)



