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
    template_name = "user/index.html"
    login_url = reverse_lazy("login")
    
    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["username"] = self.request.user.username
        context["object_list"] = Suitcase.objects.filter(rented=False)
        context["quiz_questions"] = QuizQuestion.objects.filter(is_active=True).prefetch_related("options__categories")
        context["show_recommendations"] = "recommended_suitcases" in self.request.session
        
        if context["show_recommendations"]:
            context["recommended_suitcases"] = self.request.session.get("recommended_suitcases", [])
        
        context["show_quiz"] = "quiz_in_progress" in self.request.session
        return context

class QuizView(LoginRequiredMixin, TemplateView):
    template_name = "user/quiz.html"
    login_url = reverse_lazy("login")
    
    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        
        # Get all active questions
        questions = QuizQuestion.objects.filter(is_active=True).prefetch_related("options__categories")
        total_questions = questions.count()
        current_step = int(self.request.session.get('quiz_step', 0))
        
        # Ensure step is within bounds
        if current_step >= total_questions:
            current_step = 0
        
        context["total_questions"] = total_questions
        context["current_step"] = current_step
        context["current_question"] = questions[current_step] if current_step < total_questions else None
        context["progress_percent"] = int((current_step / total_questions) * 100) if total_questions > 0 else 0
        
        return context
    
    def get(self, request, *args, **kwargs):
        # Initialize quiz session
        if 'quiz_answers' not in request.session:
            request.session['quiz_answers'] = {}
        request.session['quiz_in_progress'] = True
        
        # Reset step if starting over
        if 'reset_quiz' in request.GET:
            request.session['quiz_step'] = 0
            return redirect('quiz')
            
        return super().get(request, *args, **kwargs)
    
    def post(self, request, *args, **kwargs):
        current_step = int(request.session.get('quiz_step', 0))
        questions = QuizQuestion.objects.filter(is_active=True).prefetch_related("options__categories")
        total_questions = questions.count()
        
        if current_step >= total_questions:
            return self.process_complete_quiz(request)
        
        # Process form for current step
        current_question = questions[current_step]
        form = QuizStepForm(request.POST)  # Now using the form from forms.py
        form.fields['selected_option'].queryset = current_question.options.all()
        
        if form.is_valid():
            # Store the answer
            selected_option = form.cleaned_data['selected_option']
            request.session['quiz_answers'][str(current_step)] = {
                'question_id': current_question.id,
                'option_id': selected_option.id,
                'categories': list(selected_option.categories.values_list('name', flat=True))
            }
            
            # Move to next step or complete
            request.session['quiz_step'] = current_step + 1
            
            if current_step + 1 >= total_questions:
                return self.process_complete_quiz(request)
            else:
                return redirect('quiz')
        else:
            # Form invalid, show errors
            context = self.get_context_data()
            context['form'] = form
            return self.render_to_response(context)
    
    def process_complete_quiz(self, request):
        # Collect all categories from answers
        all_categories = []
        for answer in request.session.get('quiz_answers', {}).values():
            all_categories.extend(answer['categories'])
        
        # Get recommendations
        recommendations = get_recommended_suitcases(all_categories)
        recommendations_data = [{
            "uuid": str(rec["suitcase"].uuid),
            "name": rec["suitcase"].name,
            "score": round(rec["score"] * 100, 1),
            "categories": rec["categories"],
        } for rec in recommendations]
        
        # Store results and clean up session
        request.session["recommended_suitcases"] = recommendations_data
        request.session.pop('quiz_answers', None)
        request.session.pop('quiz_step', None)
        request.session.pop('quiz_in_progress', None)
        
        return redirect('non-rented-view')

def clear_recommendations(request):
    """Clear stored recommendations and quiz progress from session"""
    keys_to_remove = ['recommended_suitcases', 'quiz_in_progress', 'quiz_answers', 'quiz_step']
    for key in keys_to_remove:
        if key in request.session:
            del request.session[key]
    return HttpResponseRedirect(reverse_lazy("non-rented-view"))

class RentedView(LoginRequiredMixin, TemplateView):
    template_name = "user/rented.html"
    login_url = reverse_lazy("login")
    
    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["username"] = self.request.user.username
        context["user_uuid"] = self.request.user.uuid
        context["object_list"] = Suitcase.objects.filter(
            rented=True, 
            rented_by=self.request.user
        )
        return context