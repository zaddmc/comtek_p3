from django.shortcuts import render
from django.views.generic import View

# Create your views here.

class ClassicView(View):

    def get(self,request):

        return render(request=request,template_name="index.html")


