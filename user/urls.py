
from django.urls import path
from .views import NonRentedView, RentedView

urlpatterns = [
        path("",NonRentedView.as_view(),name="classic"),
        path("rented",RentedView.as_view(),name="rented"),
]
