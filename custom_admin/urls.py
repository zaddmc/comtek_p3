from django.urls import path

from .views import CustAdminIndex,WIPView,AdminSuitcaseView,AdminSuitcaseCreate,AdminCategoryCreate

urlpatterns = [
        path("",CustAdminIndex.as_view(),name="custom-admin-home"),
        path("suitcases/view/",AdminSuitcaseView.as_view(),name="custom-admin-suitcase-view"),
        path("suitcases/view/<slug:uuid>/",WIPView.as_view(),name="custom-admin-suitcase-view"),

        path("suitcases/data/",WIPView.as_view(),name="custom-admin-suitcase-data"),
        path("suitcases/data/<slug:uuid>/",WIPView.as_view(),name="custom-admin-suitcase-data"),
        path("suitcases/create/",AdminSuitcaseCreate.as_view(),name="custom-admin-suitcase-create"),

        path("quiz/",WIPView.as_view(),name="custom-admin-quiz"),

        path("categories/view/",WIPView.as_view(),name="custom-admin-categories-view"),
        path("categories/view/<slug:uuid>/",WIPView.as_view(),name="custom-admin-categories-view"),

        path("categories/create/",AdminCategoryCreate.as_view(),name="custom-admin-categories-create"),

        path("questions/view/",WIPView.as_view(),name="custom-admin-questions-view"),
        path("questions/view/<slug:uuid>/",WIPView.as_view(),name="custom-admin-questions-view"),

        path("questions/create/",WIPView.as_view(),name="custom-admin-questions-create"),

        path("users/view/",WIPView.as_view(),name="custom-admin-users-view"),
        path("users/view/<slug:uuid>/",WIPView.as_view(),name="custom-admin-users-view"),

        path("users/create/",WIPView.as_view(),name="custom-admin-users-create"),
        path("users/invite/",WIPView.as_view(),name="custom-admin-users-invite"),
]
