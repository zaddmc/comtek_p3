import uuid
from django.contrib.auth.models import AbstractUser, BaseUserManager
from django.db import models

from suitcases.forms import RentForm
class CustomUserManager(BaseUserManager):
    use_in_migrations = True

    def create_user(self, first_name:str,last_name:str,email:str, password=None, **extra_fields):

        user = self.model(username=email,first_name=first_name,last_name=last_name,email=email, **extra_fields)
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, first_name:str,last_name:str,email:str, password=None, **extra_fields):
        """
        Custom logic for creating superusers.
        """
        extra_fields.setdefault('is_staff', True)
        extra_fields.setdefault('is_superuser', True)
        extra_fields.setdefault('is_active', True)


        if extra_fields.get('is_staff') is not True:
            raise ValueError('Superuser must have is_staff=True.')
        if extra_fields.get('is_superuser') is not True:
            raise ValueError('Superuser must have is_superuser=True.')

        return self.create_user(first_name=first_name,last_name=last_name,email=email, password=password, **extra_fields)

class User(AbstractUser):
    """
    Custom user model so uuid is used instead of ID 
    """
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)
    objects = CustomUserManager()

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None) -> None:
        print(f"Name: {self.get_full_name()}")
        self.username = self.email

        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)

        if not hasattr(self,"userinfo"):
            user_info = UserInfo(user=self)
            user_info.company_cvr = 0
            user_info.company_name = ""
            user_info.company_industri_code=0 
            user_info.save()
        return
    def rent_briefcase(self,briefcase,rent_form:RentForm) -> str | None:
        self.userinfo.rent_amount += 1
        self.userinfo.current_rented+= 1
        self.save()
        self.userinfo.save()
        return briefcase.rent(self,rent_form)

    def unrent_briefcase(self,briefcase) -> None:
        self.userinfo.current_rented -=1
        self.save()
        self.userinfo.save()
        return briefcase.unrent()
        

class UserInfo(models.Model):
    user = models.OneToOneField(to=User,on_delete=models.CASCADE,related_name="userinfo")
    company_cvr = models.IntegerField()
    company_name = models.CharField(max_length=30)
    company_industri_code= models.IntegerField()
    rent_amount = models.IntegerField(default=0)
    current_rented = models.IntegerField(default=0)


