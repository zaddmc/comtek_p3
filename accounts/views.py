from http import HTTPStatus
from django.contrib.auth.views import LoginView
from django.http import HttpRequest, HttpResponse, HttpResponseRedirect
from django.shortcuts import render
from django.urls import reverse
from django.views.generic import CreateView
from django.contrib.auth import login

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
        items = post_form.items()
        for i in items:
            print(f"\nkey: {i[0]}\nvalue: {i[1]}\n")

        print(post_form)
        custom_user_create_form = CustomUserCreate(post_form)
        if not custom_user_create_form.is_valid():
            return render(request,"registration/signup.html",{"form":custom_user_create_form}) 
        company_cvr_int = 0
        company_industri_code_int = 0
        try:
            company_cvr_int = int(custom_user_create_form.cleaned_data["company_cvr"])
            company_industri_code_int= int(custom_user_create_form.cleaned_data["company_industri_code"])

        except:
            return render(request,"registration/signup.html",{"form":custom_user_create_form}) 
        user:User = custom_user_create_form.save(commit=False)
        user.save()

        user.userinfo.company_cvr = company_cvr_int 
        user.userinfo.company_industri_code = company_industri_code_int 
        user.userinfo.company_name= custom_user_create_form.cleaned_data["company_name"]
        user.userinfo.save()

        login(request,user)


        return HttpResponseRedirect(reverse("front-page")) 

class CustLoginView(LoginView):
    def get(self, request, *args, **kwargs):
        if(self.request.user.is_authenticated):
            return HttpResponseRedirect("/")
        return super().get(request, *args, **kwargs)



