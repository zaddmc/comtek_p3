from unicodedata import name
from django.contrib import admin

# Register your models here.

from .models import Suitcase, SuitcaseBleInfo, Category


admin.register(Suitcase)
admin.site.register(Suitcase)

admin.register(SuitcaseBleInfo)
admin.site.register(SuitcaseBleInfo)
admin.register(Category)
admin.site.register(Category)
class SuitcaseAdmin(admin.ModelAdmin):
    list_display = ("name")


