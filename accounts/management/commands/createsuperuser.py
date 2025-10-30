# users/management/commands/createsuperuser.py
from django.contrib.auth.management import getpass
from django.contrib.auth.management.commands.createsuperuser import Command as OriginalCreateSuperuserCommand
from django.contrib.auth import get_user_model
from django.core.management import CommandError
from django.core.validators import validate_email
from django.forms import ValidationError 


def validate_name(name:str):
    if(len(name) < 3):
        return False
    if not name.isalpha():
        return False
    return True


class Command(OriginalCreateSuperuserCommand):
    help = "Customized version of Django's createsuperuser command."

    def handle(self, *args, **options):
        """
        Override handle() to make it non-interactive or prefill default values.
        """
        options["interactive"] = True 

        User = get_user_model()

        # Example: Pull defaults from environment variables or fallbacks
        first_name= input("First Name: ").strip() 
        last_name= input("Last Name: ").strip() 
        email = input("Email: ").strip() 

        password1 = getpass.getpass("Password: ") 
        password2 = getpass.getpass("Confirm Password: ") 

        if(password1 != password2):
            raise CommandError(f"The passwords do not match")

        try:
            validate_email(email)
        except ValidationError:
            raise CommandError(f"Invalid email")

        if not validate_name(first_name):
            raise CommandError(f"Invalid First name")
        if not validate_name(last_name):
            raise CommandError(f"Invalid Last name")

        full_name = first_name.lower() + " " + last_name.lower()

        if User.objects.filter(email=email,password=password1).exists():
            self.stdout.write(self.style.WARNING(f"Superuser '{full_name}' already exists."))
            return

        try:
            User.objects.create_superuser(first_name=first_name, last_name=last_name,email=email, password=password1)
        except Exception as e:
            raise CommandError(f"Error creating superuser: {e}")

        self.stdout.write(self.style.SUCCESS(f"Superuser '{full_name}' created successfully!"))
