# user/urls.py
from django.urls import path
from .views import NonRentedView, RecommendationView, RentedView, QuizView
from home.views import WIPView

urlpatterns = [
    path("", NonRentedView.as_view(), name="non-rented-view"),
    path("quiz/", QuizView.as_view(), name="quiz"),
    path("quiz/recommendations/", RecommendationView.as_view(), name="quiz-recommendations"),
    path("rented/", RentedView.as_view(), name="rented-view"),
    path("clear-recommendations/", WIPView.as_view(), name="clear-recommendations"),
]
