
from django import forms
from django.contrib.auth.forms import UserCreationForm
from .models import User

class CustomUserCreate(UserCreationForm):
    company_name= forms.CharField()
    company_cvr = forms.CharField(widget= forms.HiddenInput())
    company_industri_code = forms.CharField(widget=forms.HiddenInput())

    class Meta(UserCreationForm.Meta):
        model = User 
        fields = ["username","password1","password2"]
        help_texts = {"username":None} # Remove username help text.

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fields["password1"].help_text = ""
        self.fields["password2"].help_text = ""