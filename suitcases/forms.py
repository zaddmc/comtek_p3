from django import forms
from django.forms.widgets import SelectDateWidget

class RentForm(forms.Form):
    expiration_date = forms.DateField(
        label="Date of Birth",
        required=True,
        widget=forms.DateInput(format="%Y-%m-%d", attrs={"type": "date"}),
        input_formats=["%Y-%m-%d"]
    )
 


