from django.http import request
from django.shortcuts import render
from django.views import View
from django.views.generic.base import TemplateView 
class HomeView(TemplateView):

    template_name = "home/index.html"


