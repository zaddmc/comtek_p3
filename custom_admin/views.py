import calendar
import json
from django.db.models import QuerySet
from django.db.models.base import connection
from django.db.models.functions import Lower
from django.db.models.sql import Query
from django.http import HttpRequest, HttpResponse, HttpResponseRedirect, JsonResponse
from django.shortcuts import render
from datetime import datetime, time,timedelta

from django.urls import reverse
from django.views import View
from django.views.generic.base import TemplateView

from django.contrib.auth.mixins import UserPassesTestMixin

from accounts.models import User
from suitcases.google_fmd import fmd_get_briefcase_location, fmd_refresh_device_list, fmd_register_briefcase 
from user.models import QuizQuestion, QuestionOption
from suitcases.models import Suitcase, Category, SuitcaseLocation, SuitcaseRentLogs

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
        sorted_suitcases = sorted(all_suitcases,key= lambda x:x.suitcasedata.rented_amount,reverse=True)[0:3]
        ctxt["most_rented_suitcases"] = sorted_suitcases

        users = User.objects.all()

        sorted_users_most_active= sorted(users,key= lambda x:x.userinfo.rent_amount,reverse=True)[0:3]
        sorted_users_current_active= sorted(users,key= lambda x:x.userinfo.current_rented,reverse=True)[0:3]


        ctxt["most_active_customers"] = sorted_users_most_active 
        ctxt["most_current_active_customers"] =sorted_users_current_active 

        cur_date = datetime.now()
        out_days = [0 for _ in range(calendar.monthrange(cur_date.year,cur_date.month)[1])]
        rent_logs:list[SuitcaseRentLogs] = SuitcaseRentLogs.objects.all()
        for log in rent_logs:
            timestamp:datetime = log.created_at
            out_days[timestamp.day-1] += 1
        ctxt["rent_data"] = json.dumps(out_days)

        location_out = []

        for suitcase in all_suitcases:
            suitcase_location_data_points: QuerySet[SuitcaseLocation]  = SuitcaseLocation.objects.filter(suitcase=suitcase)
            if not suitcase_location_data_points:
                continue
            location_data_point= suitcase_location_data_points.latest("time_stamp")
            out_obj = {"name":suitcase.name,"lon":location_data_point.longitude,"lat":location_data_point.lattitude}
            location_out.append(out_obj)
        ctxt["location_data"] = json.dumps(location_out)

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

    def delete(self,request:HttpRequest):

        quiz_id= request.POST.get("question_id", None)
        
        QuizQuestion.objects.get(pk=quiz_id).delete()

        return HttpResponseRedirect(reverse("custom-admin-questions-create"))

    def post(self, request:HttpRequest):

        request_method = request.POST.get("_method",None)
        if request_method == "delete":
            return self.delete(request=request)
        
        request_quizname = request.POST.get("quiz_name_input",None)
        index = request.POST.getlist("index",None)
        if index == None:
            return HttpResponseRedirect(reverse("custom-admin-questions-create"))
        question_order = QuizQuestion.objects.all().count()
        quiz = QuizQuestion.objects.create(question_text=request_quizname,question_order=question_order)
        quiz.save()

        for i in index:
            optionname = request.POST.get("option_name-"+i,None)
            categories = request.POST.getlist("category-"+i,None)

            if optionname== None or categories == None:
                quiz.delete()
                return HttpResponseRedirect(reverse("custom-admin-questions-create"))
            option = QuestionOption.objects.create(option_text=optionname,question=quiz)
            for p in categories:
                category = Category.objects.get(pk=p)
                option.categories.add(category)
            option.save()
            quiz.options.add(option)
        
        quiz.save()
        return HttpResponseRedirect(reverse("custom-admin-questions-create"))

    def get(self,request:HttpRequest):
        all_categories = Category.objects.all()

        all_questions = QuizQuestion.objects.all()

        context = {}

        context["questions"] = all_questions
        context["categories"] = all_categories
        

        return render(request,template_name="custom_admin/quiz/quiz_create.html",context=context) 

class AdminSuitcaseCreate(UserAdminRequiredMixin,View):
    def post(self,request:HttpRequest):
        request_suitcasename = request.POST.get("suitcase_name_input",None)
        request_categories = request.POST.getlist("categories[]",None)
        request_user = request.POST.get("users",None)

        suitcase:Suitcase = Suitcase.objects.create(name=request_suitcasename)

        suitcase.canonic_id = "a"; 
        suitcase.ephemeral_id="b"; 

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

