from datetime import datetime
from hashlib import sha256
import uuid
from django.db import models
from django.core.signing import Signer
from accounts.models import User
from suitcases.forms import RentForm


class Category(models.Model):
    name = models.CharField(max_length=256)

class Suitcase(models.Model):
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)

    rented_by = models.ForeignKey(User,null=True,blank=True,on_delete=models.SET_NULL)
    rented = models.BooleanField()
    created_at = models.DateField(auto_now_add=True)
    rented_date = models.DateField(blank=True,null=True)
    expiration_date = models.DateField(blank=True,null=True)

    name = models.CharField(max_length=20, unique=True)
    objects = models.Manager()
    categories = models.ManyToManyField(Category)

    def getCategories(self):
        return [category.name for category in self.categories.all()]
        

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None):
        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)
        if not hasattr(self,"suitcasebleinfo"):
            suitcase_info = SuitcaseBleInfo(suitcase=self)
            suitcase_info.save()

        if not hasattr(self,"suitcasedata"):
            suitcase_data = SuitcaseData(suitcase=self)
            suitcase_data.save()

        return

    def unrent(self):
        self.rented = False
        self.rented_by = None 
        self.rented_date= None 
        self.expiration_date= None 

    def rent(self,user:User,rent_form:RentForm) -> str|None:
        self.rented_by = user
        self.rented = True 

        self.rented_date = datetime.today()
        self.expiration_date = rent_form.cleaned_data["expiration_date"]

        if(self.expiration_date <= self.rented_date):
            return "Expiration date must be after today"
        self.save()

        return None

class SuitcaseData(models.Model):
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)
    suitcase = models.OneToOneField(Suitcase,null=False,blank=False,on_delete=models.CASCADE,related_name="suitcasedata")
    rented_amount= models.IntegerField(default=0)
    objects = models.Manager()

class SuitcaseBleInfo(models.Model):
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)
    suitcase = models.OneToOneField(Suitcase,null=False,blank=False,on_delete=models.CASCADE,related_name="suitcasebleinfo")
    secret_key = models.CharField(max_length=50,blank=True)
    msg_count = models.SmallIntegerField(default=0)

    objects = models.Manager()

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None):
        signer = Signer()
        self.secret_key = signer.sign(str(self.suitcase.uuid) + str(self.uuid)).split(":")[1];
        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)

        return

    def get_hashed_secret_key(self) -> str:
        hashed_key_str = str(self.secret_key) + str(self.msg_count)

        m = sha256()
        m.update(hashed_key_str.encode())

        return m.hexdigest()


