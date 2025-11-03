from django.contrib import admin
from django.contrib.auth.admin import UserAdmin

from accounts.forms import CustomForm, CustomLogin

# Register your models here.

from .models import User, UserInfo

admin.register(User,UserAdmin)
admin.site.register(User,UserAdmin)

admin.register(UserInfo)
admin.site.register(UserInfo)

admin.autodiscover()

admin.site.login_form = CustomForm 
