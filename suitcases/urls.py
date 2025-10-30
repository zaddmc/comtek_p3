

from django.urls import path

from .views import SuitcaseDisplayView


urlpatterns = [
        path("<slug:uuid>",SuitcaseDisplayView.as_view(),name="display-suitcase"),
]
