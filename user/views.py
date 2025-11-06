# user/views.py
from enum import Enum
from functools import reduce
from typing import Any
from django.contrib.auth import views
from django.template.loader import render_to_string
from django.views.generic import TemplateView, View
from django.urls import reverse, reverse_lazy
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import HttpRequest, HttpResponse, HttpResponseBadRequest, HttpResponseRedirect
from django.shortcuts import redirect, render

from auth_test.utils import show_error_page

from .forms import QuizStepForm
from .models import QuestionOption, QuizQuestion
from suitcases.models import Category, Suitcase
from .utils import get_recommended_suitcases

class NonRentedView(LoginRequiredMixin, View):
    """
    Displays all unrented suitcases and optionally shows quiz recommendations / progress status.
    """
    login_url = reverse_lazy("login")

    def get(self,request:HttpRequest, **kwargs):
        """
        Add user info, available suitcases, quiz questions, and recommendation/quiz status to context.
        """
        context = {} 

        session_data =  request.session.get("quiz_data",None)
        context["show_quiz"] = True if session_data else False 
        context["current_id"] = 0 
        if not session_data:
            return render(request,"user/index.html",context) 
        current_id = 0
        try:
            current_id = session_data.index(None) + 1
        except:
            current_id = False
        print(current_id)
        context["current_id"] = current_id if current_id else 1
        return render(request,"user/index.html",context) 


class QuizView(LoginRequiredMixin, View):
    login_url = reverse_lazy("login")

    def get_context_data(self,request:HttpRequest) -> dict[str,Any] | None:
        question_count = self.get_question_count()
        question_id = self.get_question_id(request)
        if not question_id:
            return None
        question = QuizQuestion.objects.get(pk=question_id)
        form = QuizStepForm(question=question)

        if not question: 
            return None 
        ctx = {}
        session_data = self.get_quiz_session_data(request)
        assert(session_data)

        current_answer = session_data[question_id - 1]
        if current_answer:
            form.initial["selected_option"] = current_answer
        ctx["question"] = question
        ctx["form"] = form 
        ctx["total_questions"] =question_count 
        ctx["progress_percent"] =  (question_id ) * 100 / question_count 

        return ctx 

    def get(self, request:HttpRequest):
        session_data =  self.get_quiz_session_data(request) 
        question_count = self.get_question_count()
        if not session_data:
            session_data = [None for _ in range(question_count)]
  
        request.session["quiz_data"] = session_data

        ctx = self.get_context_data(request)
        if not ctx:
            return show_error_page(request,"Could not fetch question",400)

        return render(request,template_name="user/quiz.html",context=ctx) 

    def post(self, request):

        question_id = self.get_question_id(request)
        if not question_id:
            return show_error_page(request,"Invalid questoin ID",400)

        session_data =  self.get_quiz_session_data(request) 
        if not session_data:
            return show_error_page(request,"No session data",400) 

        question = QuizQuestion.objects.get(pk=question_id)
        form = QuizStepForm(data=request.POST,question=question)

        if not form.is_valid():
            return HttpResponseBadRequest("Invalid form".encode()) 

        action = request.POST.get("action", "next")  # default to next
        if action == "results":
            return self.go_to_results(request,session_data,question_id,form)
        if action != "next" and action != "prev":
            return show_error_page(request,"Invalid action",400)
        session_data[question_id-1] = form.cleaned_data["selected_option"].pk
        request.session["quiz_data"] = session_data

        if action == "next":
            return self.go_to_next_question(question_id)
        elif action == "prev":
            return self.go_to_prev_question(question_id)

    def get_question_id(self,request:HttpRequest) -> int | None:
        question_id_str = request.GET.get("quiz_id","") 
        if not question_id_str:
            return None 
        question_id = 0
        try:
           question_id = int(question_id_str) 
        except:
            return None 
        return question_id
    def get_question_count(self) -> int:
        return QuizQuestion.objects.all().count()

    def go_to_question(self,question_id:int):

        return HttpResponseRedirect(redirect_to=reverse("quiz") + f"?quiz_id={question_id}")
    def go_to_results(self,request:HttpRequest,session_data:list[int],question_id:int,form:QuizStepForm) -> HttpResponse:
        session_data[question_id-1] = form.cleaned_data["selected_option"].pk
        request.session["quiz_data"] = session_data
        request.session["last_quiz_data"] = request.session.pop("quiz_data") 
        return HttpResponseRedirect(reverse("quiz-recommendations"))

    def go_to_next_question(self,question_id:int):
        question_count = self.get_question_count()
        assert(question_id + 1 <= question_count)
        return self.go_to_question(question_id+1) 

    def go_to_prev_question(self,question_id:int):
        assert(question_id - 1 > 0)
        return self.go_to_question(question_id-1)

    def get_quiz_session_data(self,request:HttpRequest) -> list[int] | None:
        return request.session.get("quiz_data",None)

class RecommendationView(LoginRequiredMixin,View):
    def get(self,request:HttpRequest):
        session_data =  request.session.get("last_quiz_data",None)
        print(session_data)
        if not session_data:
            ctx = {}
            ctx["error_text"] = "No session data."
            ctx["error_code"] = 400 
            return render(request,template_name="common/error.html",context=ctx)
        for i in session_data:
            if i == None:
                ctx = {}
                ctx["error_text"] = "Missing questions."
                ctx["error_code"] = 400 
                return render(request,template_name="common/error.html",context=ctx)
        categories = []

        for pk in session_data:
            print(pk)
            ob =  QuestionOption.objects.get(pk=pk)
            for cat in ob.categories.all():
                print(f"name:{cat.name}")
                categories.append(cat)
        print(f"cat:{len(categories)}")
        recommendations = get_recommended_suitcases(categories)

        ctx = {}
        ctx["recommendations"] = recommendations

        return render(request,"user/recommendations.html",ctx) 

def sort_suitcases(x:Suitcase,categories:dict[str,int]):
    score = 0
    for cat in x.categories.all():
        if cat.name in categories:
            score += categories[cat.name]
    return score

class RentedView(LoginRequiredMixin, TemplateView):

    template_name = "user/rented.html"
    login_url = reverse_lazy("login")

    def get_context_data(self, **kwargs):
        """Add user and their rented suitcase to suitcase list context."""
        context = super().get_context_data(**kwargs)
        context["username"] = self.request.user.username
        context["user_uuid"] = self.request.user.uuid
        context["object_list"] = Suitcase.objects.filter(
            rented=True, rented_by=self.request.user
        )
        return context
