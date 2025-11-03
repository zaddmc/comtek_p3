# user/views.py
from django.views.generic import TemplateView
from django.urls import reverse_lazy
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import HttpResponseRedirect
from django.shortcuts import redirect

from .forms import QuizStepForm
from .models import QuizQuestion
from suitcases.models import Suitcase
from .utils import get_recommended_suitcases


class NonRentedView(LoginRequiredMixin, TemplateView):
    """
    Displays all unrented suitcases and optionally shows quiz recommendations / progress status.
    """

    template_name = "user/index.html"
    login_url = reverse_lazy("login")

    def get_context_data(self, **kwargs):
        """
        Add user info, available suitcases, quiz questions, and recommendation/quiz status to context.
        """
        context = super().get_context_data(**kwargs)
        context["username"] = self.request.user.username
        context["object_list"] = Suitcase.objects.filter(rented=False)
        context["quiz_questions"] = QuizQuestion.objects.filter(
            is_active=True
        ).prefetch_related("options__categories")
        context["show_recommendations"] = (
            "recommended_suitcases" in self.request.session
        )
        # Determine if recommendations should be displayed
        if context["show_recommendations"]:
            context["recommended_suitcases"] = self.request.session.get(
                "recommended_suitcases", []
            )
        # show quiz notice if a quiz session is in progress/active
        context["show_quiz"] = "quiz_in_progress" in self.request.session
        return context


class QuizView(LoginRequiredMixin, TemplateView):
    """
    Manages the step-by-step quiz process for users to get suitcase recommendations.
    Handles quiz navigation, answer validation, and recommendation generation.
    """

    template_name = "user/quiz.html"
    login_url = reverse_lazy("login")

    def get_context_data(self, **kwargs):
        """Prepare quiz state, current question, and progress tracking."""
        context = super().get_context_data(**kwargs)
        questions = QuizQuestion.objects.filter(is_active=True).prefetch_related(
            "options__categories"
        )
        total_questions = questions.count()
        current_step = int(self.request.session.get("quiz_step", 0))
        # Reset step if out of bounds
        if current_step >= total_questions:
            current_step = 0
        # Basic quiz state info
        context["total_questions"] = total_questions
        context["current_step"] = current_step
        context["current_question"] = (
            questions[current_step] if current_step < total_questions else None
        )
        context["progress_percent"] = (
            int((current_step / total_questions) * 100) if total_questions > 0 else 0
        )

        # Prefill form if user answered this step previously
        if context["current_question"]:
            form = QuizStepForm()
            form.fields["selected_option"].queryset = context[
                "current_question"
            ].options.all()
            answers = self.request.session.get("quiz_answers", {})
            prev = answers.get(str(current_step))
            if prev:
                form.initial = {"selected_option": str(prev.get("option_id"))}

        return context

    def get(self, request, *args, **kwargs):
        """Initialize quiz session data and handle reset requests."""
        if "quiz_answers" not in request.session:
            request.session["quiz_answers"] = {}
        request.session["quiz_in_progress"] = True

        # Reset quiz manually if requested
        if "reset_quiz" in request.GET:
            request.session["quiz_step"] = 0
            return redirect("quiz")

        # Ensure quiz_step is set
        request.session.setdefault("quiz_step", 0)
        return super().get(request, *args, **kwargs)

    def post(self, request, *args, **kwargs):
        """handle answer submission, validation, and step navigation."""
        action = request.POST.get("action", "next")  # default to next
        current_step = int(request.session.get("quiz_step", 0))
        questions = QuizQuestion.objects.filter(is_active=True).prefetch_related(
            "options__categories"
        )
        total_questions = questions.count()

        # Safety: if somehow beyond bounds, treat as complete
        if current_step >= total_questions:
            return self.process_complete_quiz(request)

        if action == "prev":
            # Move back one question (skip validation)
            request.session["quiz_step"] = max(current_step - 1, 0)
            return redirect("quiz")

        # Otherwise, process current answer
        current_question = questions[current_step]
        form = QuizStepForm(request.POST)
        form.fields["selected_option"].queryset = current_question.options.all()

        if form.is_valid():
            selected_option = form.cleaned_data["selected_option"]
            answers = request.session.get("quiz_answers", {})
            answers[str(current_step)] = {
                "question_id": current_question.id,
                "option_id": selected_option.id,
                "categories": list(
                    selected_option.categories.values_list("name", flat=True)
                ),
            }
            request.session["quiz_answers"] = answers

            # advance to next question
            next_step = current_step + 1
            request.session["quiz_step"] = next_step

            if next_step >= total_questions:
                return self.process_complete_quiz(request)
            return redirect("quiz")

        # invalid submission - re-render with errors
        context = self.get_context_data()
        context["form"] = form
        return self.render_to_response(context)

    def process_complete_quiz(self, request):
        # Collect all categories from answers
        all_categories = []
        for answer in request.session.get("quiz_answers", {}).values():
            all_categories.extend(answer["categories"])

        # generate recommendations
        recommendations = get_recommended_suitcases(all_categories)
        recommendations_data = [
            {
                "uuid": str(rec["suitcase"].uuid),
                "name": rec["suitcase"].name,
                "score": round(rec["score"] * 100, 1),
                "categories": rec["categories"],
            }
            for rec in recommendations
        ]

        # Store results and clean up session data
        request.session["recommended_suitcases"] = recommendations_data
        request.session.pop("quiz_answers", None)
        request.session.pop("quiz_step", None)
        request.session.pop("quiz_in_progress", None)

        return redirect("non-rented-view")


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
