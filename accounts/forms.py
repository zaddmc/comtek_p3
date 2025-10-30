
from django import forms
from django.apps import apps
from django.conf import LazyObject
from django.contrib.admin import AdminSite
from django.contrib.auth import authenticate
from django.contrib.auth.forms import AuthenticationForm, UserCreationForm, UserModel
from django.db.utils import import_string
from .models import User

class CustomForm(AuthenticationForm):
    username = forms.EmailField(required=True,label="Email: ",max_length=254)
class CustomLogin(AdminSite):
    login_form = CustomForm
    pass



class CustomUserCreate(UserCreationForm):
    company_name= forms.CharField()
    company_cvr = forms.CharField(widget= forms.HiddenInput())
    company_industri_code = forms.CharField(widget=forms.HiddenInput())
    

    class Meta(UserCreationForm.Meta):
        model = User 
        fields = ["first_name","last_name","password1","password2"]

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fields["password1"].help_text = ""
        self.fields["password2"].help_text = ""
