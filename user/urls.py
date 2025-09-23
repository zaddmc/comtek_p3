
from django.urls import path
from .views import ClassicView 


urlpatterns = [
        path("",ClassicView.as_view(),name="classic")
]
