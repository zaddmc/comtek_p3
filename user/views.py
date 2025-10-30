from django.urls import reverse_lazy
from django.views.generic import ListView
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import JsonResponse, HttpResponseRedirect
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
        
        # Check if we should show recommendations from session
        context["show_recommendations"] = "recommended_suitcases" in self.request.session
        if context["show_recommendations"]:
            context["recommended_suitcases"] = self.request.session.get("recommended_suitcases", [])
            
        return context

    def post(self, request, *args, **kwargs):
        """Handle quiz submission and return recommended suitcases."""
        # Check if it's an AJAX request first (for backward compatibility)
        if request.headers.get("X-Requested-With") == "XMLHttpRequest":
            try:
                data = json.loads(request.body)
                user_categories = data.get("categories", [])
                recommendations = self._get_recommendations_data(user_categories)
                request.session["recommended_suitcases"] = recommendations
                return JsonResponse({"success": True, "recommendations": recommendations})
            except Exception as exc:
                return JsonResponse({"success": False, "error": str(exc)})
        
        # Handle regular form submission
        try:
            # Get categories from the hidden input - it's a comma-separated string
            categories_str = request.POST.get('categories[]', '')
            user_categories = [cat.strip() for cat in categories_str.split(',') if cat.strip()]
            
            print(f"Received categories: {user_categories}")  # Debug print
            
            recommendations = self._get_recommendations_data(user_categories)
            
            print(f"Generated recommendations: {recommendations}")  # Debug print
            
            request.session["recommended_suitcases"] = recommendations
            # Redirect to same page to show results
            return HttpResponseRedirect(request.path)
        except Exception as exc:
            print(f"Error in quiz submission: {exc}")  # Debug print
            # Handle error - you might want to add error messages here
            return HttpResponseRedirect(request.path)
    
    def _get_recommendations_data(self, user_categories):
        """Helper method to get recommendation data"""
        recommendations = get_recommended_suitcases(user_categories)
        return [
            {
                "uuid": str(rec["suitcase"].uuid),
                "name": rec["suitcase"].name,
                "score": round(rec["score"] * 100, 1),  # percentage
                "categories": rec["categories"],
            }
            for rec in recommendations
        ]

# Add a view to clear recommendations
def clear_recommendations(request):
    """Clear stored recommendations from session"""
    if "recommended_suitcases" in request.session:
        del request.session["recommended_suitcases"]
    return HttpResponseRedirect(reverse_lazy("non-rented-view"))

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
