
from django.urls import path

from .views import  CVRCVRView, NACEView,No,CVRNameView

urlpatterns = [
        path("cvrs",No.as_view(),name="fetch-cvr-no-args"),
        path("cvrs/name/<str:company_name>",CVRNameView.as_view(),name="fetch-cvr-from-name"),
        path("cvrs/cvr/<int:cvr>",CVRCVRView.as_view(),name="fetch-cvr-from-cvr"),
        path("nace/<int:industri_code>",NACEView.as_view(),name="fetch-nace-code"),
]
