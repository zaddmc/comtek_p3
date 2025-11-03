# user/models.py
from django.db import models
from suitcases.models import Category


class QuizQuestion(models.Model):
    """
    Represents a single quiz question.
    it is used to determine suitcase recommendations.
    Questions are ordered by the 'order' field. & toggleable via 'is_active'.
    """

    question_text = models.CharField(max_length=360)
    objects = models.Manager()

    def __str__(self):
        return self.question_text


class QuestionOption(models.Model):
    """
    Represents a single option for a quiz question.
    Each option can be linked to multiple suitcase categories, to influence recommendations.
    """

    question = models.ForeignKey(
        QuizQuestion, on_delete=models.CASCADE, related_name="options"
    )
    option_text = models.CharField(max_length=255)
    categories = models.ManyToManyField(
        Category, blank=True, related_name="question_options"
    )
    objects = models.Manager()

    def __str__(self):
        return f"{self.question.question_text} - {self.option_text}"

    def get_category_names(self):
        """Helper to get all category names"""
        return [category.name for category in self.categories.all()]
