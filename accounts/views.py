from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.views import LoginView
from django.http import HttpResponseRedirect
from django.http.request import HttpRequest
from django.urls import reverse_lazy
from django.views.generic import CreateView
from .models import User

class CustomUserCreate(UserCreationForm):
    class Meta(UserCreationForm.Meta):
        model = User 
        fields = ["username","password1","password2"]

class SignUpView(CreateView):
    form_class = CustomUserCreate 
    success_url = reverse_lazy("/")
    template_name = "registration/signup.html"
    def get(self, request, *args, **kwargs):
        if(self.request.user.is_authenticated):
            return HttpResponseRedirect("/")
        return super().get(request, *args, **kwargs)

class CustLoginView(LoginView):
    def get(self, request, *args, **kwargs):
        if(self.request.user.is_authenticated):
            return HttpResponseRedirect("/")
        return super().get(request, *args, **kwargs)



