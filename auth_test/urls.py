"""
URL configuration for auth_test project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path,include

urlpatterns = [
        path("", include("home.urls"), name="home"),   
        path("user/", include("user.urls"), name="home"),   
        path("suitcases/", include("suitcases.urls"),name="suitcase-home"),   
        path("external/", include("external.urls"),name="external-home"),   
        path("accounts/", include("accounts.urls"),name="cust-accounts-home"),   
        path("accounts/", include("django.contrib.auth.urls"),name="accounts-home"),
        path('admin/', admin.site.urls,name="admin"),
]
