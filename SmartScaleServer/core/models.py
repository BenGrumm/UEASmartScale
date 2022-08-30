from django.db import models
from django.contrib.auth.models import User

# Create your models here.

class Beacon(models.Model):
    owner = models.ForeignKey(User, on_delete=models.CASCADE, null=False)

    name = models.CharField(max_length=50, default="")
    major = models.IntegerField()
    minor = models.IntegerField()

    xPos = models.FloatField(default=0)
    yPos = models.FloatField(default=0)
    zPos = models.FloatField(default=0)

    class Meta:
        constraints = [
            models.UniqueConstraint(fields = ['owner', 'major', 'minor'], name = 'unique_primary')
        ]

class Scale(models.Model):
    class Meta:
        unique_together=(('owner', 'scaleID'))

    pkID = models.AutoField(primary_key=True)

    owner = models.ForeignKey(User, on_delete=models.CASCADE, blank=True)

    name = models.CharField(max_length=50, default="")

    scaleID = models.BigIntegerField()
    numStored = models.BigIntegerField(default=0)
    weightGramsPerXItmes = models.FloatField(default=10)
    numItemsPerWeight = models.IntegerField(default=1)
    minNumItems = models.IntegerField(default=1)
    okNumItems = models.IntegerField(default=10)

    beaconOne = models.ForeignKey(Beacon, on_delete=models.SET_DEFAULT, default=None, related_name='beacon_one')
    beaconTwo = models.ForeignKey(Beacon, on_delete=models.SET_DEFAULT, default=None, related_name='beacon_two')
    beaconThree = models.ForeignKey(Beacon, on_delete=models.SET_DEFAULT, default=None, related_name='beacon_three')
    beaconFour = models.ForeignKey(Beacon, on_delete=models.SET_DEFAULT, default=None, related_name='beacon_four')

    beaconOneDistance = models.FloatField(default=0)
    beaconTwoDistance = models.FloatField(default=0)
    beaconThreeDistance = models.FloatField(default=0)
    beaconFourDistance = models.FloatField(default=0)

    hasUpdatedSettings = models.BooleanField(default=False)
