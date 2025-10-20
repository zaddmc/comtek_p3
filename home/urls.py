

from django.urls import path

from .views import HomeView

urlpatterns = [
        path("",HomeView.as_view(),name="front-page"),
        path("about/",HomeView.as_view(),name="front-page-about"),
        path("products/",HomeView.as_view(),name="front-page-products"),
        path("pricing/",HomeView.as_view(),name="front-page-pricing"),
]
