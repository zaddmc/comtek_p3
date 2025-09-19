"""
URL configuration for mysite project.

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
from django.urls import include, path

from . import views

app_name = "mysite"
urlpatterns = [
    path("api/state/<str:suitcase_id>", views.get_state, name="get_state"),
    path("api/state/<str:suitcase_id>/set", views.set_state, name="set_state"),
    path("", views.index, name="index"),
    path("polls/", include("polls.urls")),
    path("admin/", admin.site.urls),
    path("BLE/", include("BLE.urls")),
]
