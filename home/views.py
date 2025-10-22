from django.http import HttpResponse, request
from django.shortcuts import render
from django.views import View
from django.views.generic.base import TemplateView 
class HomeView(TemplateView):
    template_name = "home/index.html"

class AboutView(TemplateView):
    template_name = "home/about.html"

class ProductView(TemplateView):
    template_name = "home/product.html"

class PricingView(TemplateView):
    template_name = "home/pricing.html"

class ContactView(TemplateView):
    template_name = "home/contact.html"

class WIPView(View):
    def get(self,request:request.HttpRequest):
        return HttpResponse("W.I.P".encode())



