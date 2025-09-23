

from django.urls import path

from .views import SuitcaseGetView


urlpatterns = [
        path("get/<slug:uuid>",SuitcaseGetView.as_view(),name="get_suitcase"),
]
