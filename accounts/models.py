import uuid
from django.contrib.auth.models import AbstractUser
from django.db import models


class User(AbstractUser):
    """
    Custom user model so uuid is used instead of ID 
    """
    uuid = models.UUIDField(default=uuid.uuid4,editable=False,unique=True,primary_key=True)
    pass

