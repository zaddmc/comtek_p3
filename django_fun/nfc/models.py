from django.db import models

class ValidNFC(models.Model):
    uuid = models.CharField(max_length=8)
    name = models.CharField(max_length=200)


    def __str__(self):
        return self.name 


