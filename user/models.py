# user/models.py
from django.db import models

class QuizQuestion(models.Model):
    question_text = models.CharField(max_length=255)
    order = models.IntegerField(default=0)  # To control question order
    is_active = models.BooleanField(default=True)
    
    class Meta:
        ordering = ['order']
    
    def __str__(self):
        return self.question_text

class QuestionOption(models.Model):
    question = models.ForeignKey(QuizQuestion, on_delete=models.CASCADE, related_name='options')
    option_text = models.CharField(max_length=255)
    value = models.CharField(max_length=100)  # The value stored when selected
    order = models.IntegerField(default=0)
    
    class Meta:
        ordering = ['order']
    
    def __str__(self):
        return f"{self.question.question_text} - {self.option_text}"