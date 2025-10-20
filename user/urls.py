
from django.urls import path
from .views import NonRentedView, RentedView

urlpatterns = [
        path("",NonRentedView.as_view(),name="non-rented-view"),
        path("rented",RentedView.as_view(),name="rented-view"),
]
