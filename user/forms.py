# user/forms.py
from django import forms
from .models import QuestionOption


class QuizStepForm(forms.Form):
    """
    Form for single question quiz with steps user has to select an option to proceed.
    """

    selected_option = forms.ModelChoiceField(
        queryset=QuestionOption.objects.none(),
        widget=forms.RadioSelect(),
        empty_label=None,
        required=True,
        error_messages={"required": "Vælg venligst et svar før du fortsætter."},
    )
