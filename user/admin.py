# user/admin.py
from django.contrib import admin
from django import forms

from .models import QuizQuestion, QuestionOption
from suitcases.models import Category


class QuestionOptionForm(forms.ModelForm):
    """
    this class shows the categories field as a multiple select box
    also sorts the categories alphabetically
    """

    class Meta:
        model = QuestionOption
        fields = "__all__"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fields["categories"].widget = forms.SelectMultiple(attrs={"size": "6"})
        self.fields["categories"].queryset = Category.objects.all().order_by("name")


class QuestionOptionInline(admin.TabularInline):
    """
    Displays options directly on the QuizQuestion admin page
    """

    model = QuestionOption
    form = QuestionOptionForm
    extra = 1  # Number of empty option forms to show


@admin.register(QuizQuestion)
class QuizQuestionAdmin(admin.ModelAdmin):
    """
    Displays question text, order, and active status in the admin list view
    """
    inlines = [QuestionOptionInline]
