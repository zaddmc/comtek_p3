from unicodedata import name
from django.contrib import admin

# Register your models here.

from .models import Suitcase, SuitcaseBleInfo


admin.register(Suitcase)
admin.site.register(Suitcase)

admin.register(SuitcaseBleInfo)
admin.site.register(SuitcaseBleInfo)

class SuitcaseAdmin(admin.ModelAdmin):
    list_display = ("name")


