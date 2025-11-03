from django.http import HttpRequest, HttpResponse, HttpResponseRedirect, JsonResponse
from django.shortcuts import render

from django.urls import reverse
from django.views import View
from django.views.generic.base import TemplateView

from django.contrib.auth.mixins import UserPassesTestMixin

from accounts.models import User
from suitcases.models import Suitcase

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
    
class AdminSuitcaseView(UserAdminRequiredMixin,View):
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


        return render(request,template_name="templates/custom_admin/suitcase/suitcase_view.html",context=ctxt) 
    

    

class WIPView(TemplateView):
    template_name = "custom_admin/wip.html"



        
