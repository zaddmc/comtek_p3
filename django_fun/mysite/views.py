from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.http import require_POST


def index(request):
    return render(request, "mysite/index.html", {})

# Super simple test af Lock unlock WIfi
DESIRED = {}

def get_state(request, suitcase_id: str):
    state = DESIRED.get(suitcase_id, "LOCKED") # ATM default is LOCKED
    return JsonResponse({"suitcase_id": suitcase_id, "state": state})

@require_POST
def set_state(request, suitcase_id: str):
    desired = (request.POST.get("state") or "").upper()
    if desired not in ("LOCKED", "UNLOCKED"):
        return JsonResponse({"error": "Invalid/bad state"}, status=400)
    DESIRED[suitcase_id] = desired
    return JsonResponse ({"ok": True, "suitcase_id": suitcase_id, "state": desired})