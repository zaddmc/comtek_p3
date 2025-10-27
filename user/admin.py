# user/admin.py
from django.contrib import admin
from .models import QuizQuestion, QuestionOption

class QuestionOptionInline(admin.TabularInline):
    model = QuestionOption
    extra = 1  # Number of empty option forms to show

@admin.register(QuizQuestion)
class QuizQuestionAdmin(admin.ModelAdmin):
    list_display = ['question_text', 'order', 'is_active']
    list_editable = ['order', 'is_active']
    inlines = [QuestionOptionInline]
    list_filter = ['is_active']