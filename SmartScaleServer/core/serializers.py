from dataclasses import field
from attr import fields
from rest_framework import serializers
from rest_framework_jwt.settings import api_settings
from django.contrib.auth.models import User
from .models import Scale, Beacon

class IDScaleSerializer(serializers.ModelSerializer):

    class Meta:
        model=Scale
        fields=('scaleID',)

class ScaleNumberSerializer(serializers.ModelSerializer):

    class Meta:
        model=Scale
        fields=('scaleID', 'numStored')

class BeaconSerializer(serializers.ModelSerializer):
    class Meta:
        model=Beacon
        fields=('id', 'name', 'major', 'minor', 'xPos', 'yPos', 'zPos')

class ScaleSerializer(serializers.ModelSerializer):

    beaconOne = BeaconSerializer(read_only=True)
    beaconTwo = BeaconSerializer(read_only=True)
    beaconThree = BeaconSerializer(read_only=True)
    beaconFour = BeaconSerializer(read_only=True)

    class Meta:
        model=Scale
        fields=(
            'scaleID', 'name', 'numStored', 'weightGramsPerXItmes', 
            'numItemsPerWeight', 'minNumItems', 'okNumItems', 
            'beaconOne', 'beaconOneDistance', 
            'beaconTwo', 'beaconTwoDistance', 
            'beaconThree', 'beaconThreeDistance',
            'beaconFour', 'beaconFourDistance',
        )

class UserSerializer(serializers.ModelSerializer):

    class Meta:
        model = User
        fields = ('username',)


class UserSerializerWithToken(serializers.ModelSerializer):

    token = serializers.SerializerMethodField()
    password = serializers.CharField(write_only=True)

    def get_token(self, obj):
        jwt_payload_handler = api_settings.JWT_PAYLOAD_HANDLER
        jwt_encode_handler = api_settings.JWT_ENCODE_HANDLER

        payload = jwt_payload_handler(obj)
        token = jwt_encode_handler(payload)
        return token

    def create(self, validated_data):
        password = validated_data.pop('password', None)
        instance = self.Meta.model(**validated_data)
        if password is not None:
            instance.set_password(password)
        instance.save()
        return instance

    class Meta:
        model = User
        fields = ('token', 'username', 'password')