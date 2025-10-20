

from django.urls import path

from .views import HomeView,AboutView,PricingView,ProductView

urlpatterns = [
        path("",HomeView.as_view(),name="front-page"),
        path("about/",AboutView.as_view(),name="front-page-about"),
        path("products/",ProductView.as_view(),name="front-page-products"),
        path("pricing/",PricingView.as_view(),name="front-page-pricing"),
]
