# user/views.py
from functools import reduce
from django.contrib.auth import views
from django.template.loader import render_to_string
from django.views.generic import TemplateView, View
from django.urls import reverse, reverse_lazy
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import HttpRequest, HttpResponse, HttpResponseRedirect
from django.shortcuts import redirect, render

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
    """
    Manages the step-by-step quiz process for users to get suitcase recommendations.
    Handles quiz navigation, answer validation, and recommendation generation.
    """
    login_url = reverse_lazy("login")

    def get(self, request:HttpRequest, *args, **kwargs):
        question_id_str = request.GET.get("quiz_id","") 
        if not question_id_str:
            return HttpResponseRedirect(redirect_to=reverse("quiz") + f"?quiz_id={1}")
        """Initialize quiz session data and handle reset requests."""
        question_id = 0
        try:
           question_id = int(question_id_str) 
        except:
            ctx = {}
            ctx["error_text"] = "Invalid question index."
            ctx["error_code"] = 400 
            return render(request,template_name="common/error.html",context=ctx)


        questions = QuizQuestion.objects.all()
        question = questions.get(pk=question_id)
        if not question: 
            ctx = {}
            ctx["error_text"] = f"No question with ID: {question_id}."
            ctx["error_code"] = 404 
            return render(request,template_name="common/error.html",context=ctx)
        ctx = {}
        form = QuizStepForm(question=question)

        session_data =  request.session.get("quiz_data",[None for _ in range(questions.count())])
        print(len(session_data))
        current_answer = session_data[question_id - 1]
        ctx["current_session_data"] = current_answer
        if current_answer:
            form.initial["selected_option"] = current_answer
        ctx["question"] = question
        ctx["form"] = form 
        ctx["total_questions"] =questions.count() 
        ctx["progress_percent"] =  (question_id ) * 100 / questions.count()

        request.session["quiz_data"] = session_data

        return render(request,template_name="user/quiz.html",context=ctx) 

    def post(self, request, *args, **kwargs):
        session_data =  request.session.get("quiz_data",None)

        if not session_data:
            ctx = {}
            ctx["error_text"] = "No session data."
            ctx["error_code"] = 400 
            return render(request,template_name="common/error.html",context=ctx)
        """handle answer submission, validation, and step navigation."""

        question_id_str = request.GET.get("quiz_id","") 

        if not question_id_str:
            return HttpResponseRedirect(redirect_to=reverse("quiz") + f"?quiz_id={1}")
        question_id = 0
        try:
           question_id = int(question_id_str) 
        except :
            ctx = {}
            ctx["error_text"] = "Invalid question index."
            ctx["error_code"] = 400 
            return render(request,template_name="common/error.html",context=ctx)

        question = QuizQuestion.objects.get(pk=question_id)
        form = QuizStepForm(data=request.POST,question=question)

        #data = request.POST.get("selected_option")
        if not form.is_valid():
            return HttpResponse("fuc".encode())
        action = request.POST.get("action", "next")  # default to next
        if action == "results":
            session_data[question_id-1] = form.cleaned_data["selected_option"].pk
            request.session["quiz_data"] = session_data
            request.session["last_quiz_data"] = request.session.pop("quiz_data") 
            return HttpResponseRedirect(reverse("quiz-recommendations"))
        if action != "next" and action != "prev":
            ctx = {}
            ctx["error_text"] = "Bad action."
            ctx["error_code"] = 400 
            return render(request,template_name="common/error.html",context=ctx)
        addition = 0
        if action == "next":
            addition = 1
        elif action == "prev":
            addition = -1
        session_data[question_id-1] = form.cleaned_data["selected_option"].pk
        request.session["quiz_data"] = session_data
        return HttpResponseRedirect(redirect_to=reverse("quiz") + f"?quiz_id={question_id+addition}")

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


def clear_recommendations(request):
    """Clear any stored quiz recommendations and related session data."""
    keys_to_remove = [
        "recommended_suitcases",
        "quiz_in_progress",
        "quiz_answers",
        "quiz_step",
    ]
    for key in keys_to_remove:
        if key in request.session:
            del request.session[key]
    return HttpResponseRedirect(reverse_lazy("non-rented-view"))


class RentedView(LoginRequiredMixin, TemplateView):
    """Displays all suitcases currently rented by the user logged in."""

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
