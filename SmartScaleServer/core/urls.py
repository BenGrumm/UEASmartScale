from django.urls import path
from .views import current_user, UserList, Scales, ScaleItemNums, ScaleSettings, RecievedUpdate, Beacons, scale_location

urlpatterns = [
    path('current_user/', current_user),
    path('users/', UserList.as_view()),
    path('scales/',Scales.as_view()),
    path('scale/', ScaleItemNums.as_view()),
    path('settings/', ScaleSettings.as_view()),
    path('has_updated/', RecievedUpdate.as_view()),
    path('beacons/', Beacons.as_view()),
    path('scale_location/<int:scale_id>/', scale_location),
]