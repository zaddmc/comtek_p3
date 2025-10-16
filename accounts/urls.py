
from django.urls import path
from .views import SignUpView,CustLoginView


urlpatterns = [
        path("signup/",SignUpView.as_view(),name="cust-signup"),
        path("login/",CustLoginView.as_view(),name="cust-login")
]
