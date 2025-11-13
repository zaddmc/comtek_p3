from django.http import HttpRequest, HttpResponse, HttpResponseRedirect, JsonResponse
from django.shortcuts import render
from datetime import datetime,timedelta

from django.urls import reverse
from django.views import View
from django.views.generic.base import TemplateView

from django.contrib.auth.mixins import UserPassesTestMixin

from accounts.models import User
from user.models import QuizQuestion, QuestionOption
from suitcases.models import Suitcase, Category

# Create your views here.

class UserAdminRequiredMixin(UserPassesTestMixin):

    def test_func(self):
        return self.request.user.is_staff

    def handle_no_permission(self):
        return HttpResponseRedirect(redirect_to=reverse("front-page")) 

class CustAdminIndex(UserAdminRequiredMixin,View):

    def get(self,request:HttpRequest):
        ctxt =  {} 

        all_suitcases = Suitcase.objects.all()

        suitcase_amount = len(all_suitcases)

        ctxt["brief_case_amount"] = suitcase_amount 

        rented_amount = 0
        for i in range(suitcase_amount):
            if all_suitcases[i].rented:
                rented_amount += 1
        ctxt["rented_brief_case_amount"] = rented_amount
        ctxt["rented_brief_case_amount_procent"] =  1.0 if not suitcase_amount else rented_amount / suitcase_amount 
        ctxt["not_rented_brief_case_amount"] = len(all_suitcases) - rented_amount
        sorted_suitcases = sorted(all_suitcases,key= lambda x:x.suitcasedata.rented_amount)[0:3]
        ctxt["most_rented_suitcases"] = sorted_suitcases

        users = User.objects.all()

        sorted_users_most_active= sorted(users,key= lambda x:x.userinfo.rent_amount,reverse=True)[0:3]
        sorted_users_current_active= sorted(users,key= lambda x:x.userinfo.current_rented,reverse=True)[0:3]

        ctxt["most_active_customers"] = sorted_users_most_active 
        ctxt["most_current_active_customers"] =sorted_users_current_active 


        return render(request,template_name="custom_admin/index.html",context=ctxt) 
    
class AdminCategoryCreate(UserAdminRequiredMixin,View):
    def post(self,request:HttpRequest):
        
        request_method = request.POST.get("_method",None)
        if request_method == "delete":
            return self.delete(request=request)

        request_categoryname = request.POST.get("category_name_input",None)
        print(request.POST)
        
        category = Category.objects.create(name=request_categoryname)
   
        category.save()
        
        return HttpResponseRedirect(reverse("custom-admin-categories-create"))
    
    def delete(self,request:HttpRequest):

        category_id = request.POST.get("category_id", None)
        
        Category.objects.get(pk=category_id).delete()
        return HttpResponseRedirect(reverse("custom-admin-categories-create"))

    def get(self,request:HttpRequest):
        
        all_categories = Category.objects.all()
        context = {}
        context["categories"] = all_categories
        return render(request,template_name="custom_admin/category/category_create.html",context=context)
    


class AdminSuitcaseView(UserAdminRequiredMixin,View):
    def get(self,request:HttpRequest):
        
        all_suitcases = Suitcase.objects.all()
        
        

        context = {}

        context["suitcases"] = all_suitcases

        return render(request,template_name="custom_admin/suitcase/suitcase_view.html",context=context) 
    
    
class QuizCreate(UserAdminRequiredMixin,View):
    def post(self, request:HttpRequest):
        #QuizCreate.

        return HttpResponseRedirect(reverse("custom-admin-categories-create"))


class AdminSuitcaseCreate(UserAdminRequiredMixin,View):
    def post(self,request:HttpRequest):
        request_suitcasename = request.POST.get("suitcase_name_input",None)
        request_categories = request.POST.getlist("categories[]",None)
        request_user = request.POST.get("users",None)
        

        suitcase = Suitcase.objects.create(name=request_suitcasename)
        categories = []
        for x in request_categories:
            x = int(x)
            category = Category.objects.get(pk = x)
            categories.append(category)
        suitcase.categories.set(categories)
        if request_user == "None":
            suitcase.save()
            return HttpResponseRedirect(reverse("custom-admin-suitcase-view"))
        user = User.objects.get(uuid = request_user)
        user.userinfo.rent_amount += 1
        user.userinfo.current_rented += 1
        user.save()
        suitcase.rented_by = user
        suitcase.rented = True
        suitcase.rented_date = datetime.today()
        suitcase.expiration_date = timedelta(days=90) + datetime.today()
   
        suitcase.save()
        
        return HttpResponseRedirect(reverse("custom-admin-suitcase-view"))
        
        

    def get(self,request:HttpRequest):
        
        all_users = User.objects.all()        
        all_categories = Category.objects.all()
        context = {}

        

        context["users"] = all_users
        context["categories"] = all_categories
        return render(request,template_name="custom_admin/suitcase/suitcase_create.html",context=context)

class WIPView(TemplateView):
    template_name = "custom_admin/wip.html"

