from django.http import HttpResponse,HttpRequest
from django.template import loader

from .models import ValidNFC 

def nfc(request:HttpRequest):
  template = loader.get_template('index.html')
  return HttpResponse(template.render())

def hello(request:HttpRequest):

    response = HttpResponse() 

    if request.method != "GET":
        response.write("Invalid method")
        response.status_code = 303 

        return response

    get_params = request.GET

    uuid = get_params.get("uuid")
    name = get_params.get("name")

    if not uuid or not name:
        response.write("Missing uuid or name")
        response.status_code = 303 
        return response

    print(f"UUID: {uuid}")
    print(f"Name: {name}")

    v = ValidNFC(uuid = uuid,name = name)
    v.save()

    response.write("Sugondese")
    response.status_code = 200 

    return response 

