
from django.http import HttpResponse
from django.urls import path
from .views import ClassicView , RentedView,TestView

urlpatterns = [
        path("",ClassicView.as_view(),name="classic"),
        path("rented",RentedView.as_view(),name="rented"),
        path("test",TestView.as_view(),name="sui"),
]
