
from django import forms
from django.contrib.auth.forms import UserCreationForm
from .models import User

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
