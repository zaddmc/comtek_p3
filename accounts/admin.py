from django.contrib import admin
from django.contrib.auth.admin import UserAdmin

# Register your models here.

from .models import User, UserInfo

admin.register(User,UserAdmin)
admin.site.register(User,UserAdmin)

admin.register(UserInfo)
admin.site.register(UserInfo)
