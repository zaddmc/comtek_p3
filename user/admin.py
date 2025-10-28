# user/admin.py
from django.contrib import admin
from django import forms
from .models import QuizQuestion, QuestionOption
from suitcases.models import Category

class QuestionOptionForm(forms.ModelForm):
    class Meta:
        model = QuestionOption
        fields = '__all__'

        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)
            self.fields['categories'].widget = forms.SelectMultiple(attrs={'size': '6'})
            self.fields['categories'].queryset = Category.objects.all().order_by('name')

class QuestionOptionInline(admin.TabularInline):
    model = QuestionOption
    form = QuestionOptionForm
    extra = 1  # Number of empty option forms to show

@admin.register(QuizQuestion)
class QuizQuestionAdmin(admin.ModelAdmin):
    list_display = ['question_text', 'order', 'is_active']
    list_editable = ['order', 'is_active']
    inlines = [QuestionOptionInline]
    list_filter = ['is_active']

