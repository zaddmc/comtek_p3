from django.urls import reverse, reverse_lazy
from django.views.generic import ListView 
from django.contrib.auth.mixins import LoginRequiredMixin
from django.shortcuts import render
from .models import QuizQuestion

from suitcases.models import Suitcase

class NonRentedView(LoginRequiredMixin,ListView):
    model = Suitcase 
    template_name = "user/index.html"
    login_url = reverse_lazy("login")


    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        print(context.keys())

        # TEST
        quiz_questions = QuizQuestion.objects.filter(is_active=True).prefetch_related('options')

        context["object_list"] = [obj for obj in context["object_list"] if not obj.rented]
        context["username"] = self.request.user.username
        context["quiz_questions"] = quiz_questions
        return context

class RentedView(LoginRequiredMixin,ListView):
    model = Suitcase 
    template_name = "user/rented.html"
    login_url = reverse_lazy("login")


    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        context["username"] = self.request.user.username
        context["user_uuid"] = self.request.user.uuid

        context["object_list"] = [obj for obj in context["object_list"] if obj.rented and obj.rented_by.uuid == self.request.user.uuid]

        return context



