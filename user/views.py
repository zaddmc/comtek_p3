from django.urls import reverse_lazy
from django.views.generic import ListView
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import JsonResponse
import json

from .models import QuizQuestion
from suitcases.models import Suitcase
from .utils import get_recommended_suitcases

class NonRentedView(LoginRequiredMixin, ListView):
    model = Suitcase
    template_name = "user/index.html"
    login_url = reverse_lazy("login")

    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)

        # Only not-rented items in the list (preserves existing behavior)
        context["object_list"] = [obj for obj in context["object_list"] if not obj.rented]
        context["username"] = self.request.user.username

        # Prefetch options + categories to avoid N+1 in template loop
        context["quiz_questions"] = (
            QuizQuestion.objects.filter(is_active=True)
            .prefetch_related("options__categories")
        )
        return context

    def post(self, request, *args, **kwargs):
        """Handle quiz submission and return recommended suitcases."""
        if request.headers.get("X-Requested-With") == "XMLHttpRequest":
            try:
                data = json.loads(request.body)
                user_categories = data.get("categories", [])

                recommendations = get_recommended_suitcases(user_categories)
                recommended_data = [
                    {
                        "uuid": str(rec["suitcase"].uuid),
                        "name": rec["suitcase"].name,
                        "score": round(rec["score"] * 100, 1),  # percentage
                        "categories": rec["categories"],
                    }
                    for rec in recommendations
                ]

                request.session["recommended_suitcases"] = recommended_data
                return JsonResponse({"success": True, "recommendations": recommended_data})
            except Exception as exc:
                return JsonResponse({"success": False, "error": str(exc)})
        # Fallback to GET if not XHR
        return super().get(request, *args, **kwargs)

class RentedView(LoginRequiredMixin, ListView):
    model = Suitcase
    template_name = "user/rented.html"
    login_url = reverse_lazy("login")

    def get_context_data(self, *, object_list=None, **kwargs):
        context = super().get_context_data(object_list=object_list, **kwargs)
        context["username"] = self.request.user.username
        context["user_uuid"] = self.request.user.uuid
        context["object_list"] = [
            obj for obj in context["object_list"]
            if obj.rented and obj.rented_by.uuid == self.request.user.uuid
        ]
        return context
