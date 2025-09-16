from django.urls import path

from . import views

app_name = "nfc"
urlpatterns = [
        path("",views.nfc,name="index"),
        path("hello",views.hello,name="greetings"),
]
