import uuid
from django.contrib.auth.models import AbstractUser
from django.db import models


class User(AbstractUser):
    """
    Custom user model so uuid is used instead of ID 
    """
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None):
        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)
        return

class UserInfo(models.Model):
    user = models.OneToOneField(to=User,on_delete=models.CASCADE)
    company_cvr = models.IntegerField()
    company_name = models.CharField(max_length=30)
    company_industri_code= models.IntegerField()


