
from django.urls import path
from .views import NonRentedView, RentedView, QuizView, clear_recommendations

urlpatterns = [
        path("",NonRentedView.as_view(),name="non-rented-view"),
        path('quiz/', QuizView.as_view(), name='quiz'),
        path("rented",RentedView.as_view(),name="rented-view"),
        path("clear-recommendations/", clear_recommendations, name="clear-recommendations"),
]
