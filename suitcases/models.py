import uuid
from django.db import models
from django.core.signing import Signer
from accounts.models import User

class Suitcase(models.Model):
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)

    rented_by = models.ForeignKey(User,null=True,blank=True,on_delete=models.SET_NULL)
    rented = models.BooleanField()

    created_at = models.DateField(auto_now_add=True)

    rented_date = models.DateField(blank=True,null=True)
    expiration_date = models.DateField(blank=True,null=True)


    objects = models.Manager()

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None):

        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)
        if not hasattr(self,"suitcasebleinfo"):
            suitcase_info = SuitcaseBleInfo()
            suitcase_info.suitcase = self
            suitcase_info.save()

        return


class SuitcaseBleInfo(models.Model):
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)
    suitcase = models.OneToOneField(Suitcase,null=False,blank=False,on_delete=models.CASCADE)
    secret_key = models.CharField(max_length=50,blank=True)
    msg_count = models.SmallIntegerField(default=0)

    objects = models.Manager()

    def save(self, *args, force_insert=False, force_update=False, using=None, update_fields=None):
        signer = Signer()
        self.secret_key = signer.sign(str(self.suitcase.uuid) + str(self.uuid)).split(":")[1];
        super().save(*args, force_insert=force_insert, force_update=force_update, using=using, update_fields=update_fields)

        return





