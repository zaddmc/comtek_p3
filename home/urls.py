

from django.urls import path

from .views import HomeView,AboutView,PricingView,ProductView,ContactView,WIPView

urlpatterns = [
        path("",HomeView.as_view(),name="front-page"),
        path("about/",AboutView.as_view(),name="front-page-about"),
        path("about/goals",WIPView.as_view(),name="front-page-about-goals"),
        path("about/partners",WIPView.as_view(),name="front-page-about-partners"),


        path("contact/",ContactView.as_view(),name="front-page-contact"),
        path("contact/private",WIPView.as_view(),name="front-page-contact-private"),
        path("contact/company",WIPView.as_view(),name="front-page-contact-company"),
        path("contact/hours",WIPView.as_view(),name="front-page-contact-hours"),

        path("products/",ProductView.as_view(),name="front-page-products"),
        path("products/personality/",WIPView.as_view(),name="front-page-product-personality"),
        path("products/variations/",WIPView.as_view(),name="front-page-product-variation"),
        path("products/pricing/",PricingView.as_view(),name="front-page-product-pricing"),
]
