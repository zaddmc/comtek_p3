# user/forms.py
from django import forms
from django.forms.forms import ErrorList
from .models import QuestionOption, QuizQuestion


class QuizStepForm(forms.Form):
    """
    Form for single question quiz with steps user has to select an option to proceed.
    """

    selected_option = forms.ModelChoiceField(
        queryset=QuestionOption.objects.none(),
        widget=forms.RadioSelect(),
        empty_label=True,
        label="",
        required=True,
        error_messages={"required": "Vælg venligst et svar før du fortsætter."},
    )
    def __init__(self, *args, **kwargs):
        question = kwargs.pop("question", None)
        super().__init__(*args, **kwargs)
        if question:
            self.fields["selected_option"].queryset = QuestionOption.objects.filter(question=question)


