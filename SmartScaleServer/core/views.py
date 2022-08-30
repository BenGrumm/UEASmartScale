from django.http import HttpResponseRedirect
from django.contrib.auth.models import User
from requests import request
from rest_framework import permissions, status
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework.views import APIView
from .serializers import UserSerializer, UserSerializerWithToken

from .models import Scale, Beacon
from .serializers import ScaleSerializer, ScaleNumberSerializer, IDScaleSerializer, BeaconSerializer

from .multilateration import getXYZFromPositionsAndDistances

from pprint import pprint

"""
Function to return id of beacon using profile and major and minor of device 
"""
def getBeaconFromMajorMinor(user, major, minor):
    try:
        return Beacon.objects.filter(owner=user).filter(major=major).get(minor=minor)
    except Beacon.DoesNotExist as e:
        return -1

def replaceMajorMinorWithID(user, idKey, distanceKey, scaleObj):
    if idKey in scaleObj:
        beaconID = getBeaconFromMajorMinor(user, scaleObj.get(idKey).get("major"), scaleObj.get(idKey).get("minor")).id

        if(beaconID != -1):
            scaleObj[distanceKey] = scaleObj.get(idKey).get("distance")
            scaleObj[idKey] = beaconID
        else:
            scaleObj.pop(idKey, None)

"""
At url /core/scales
Get
    : For getting all scales with their values that are owned by user making request
Post
    : For taking a new scales and adding to their list of scales (if already exists will return current)
"""
class Scales(APIView):
    
    def get(self, request, format=None):
        usersScales = Scale.objects.filter(owner=request.user)
        scaleSerialized = ScaleSerializer(usersScales, many=True)
        return Response(scaleSerialized.data, status=status.HTTP_201_CREATED)

    def post(self, request, format=None):
        if "scales" in request.data:
            returnList = []
            for scale in request.data.get("scales"):
                try:
                    # .filter(owner=request.user) ?
                    Scale.objects.filter(owner=request.user).get(scaleID=scale.get("scaleID"))
                    return Response({"ERROR":"Scale With ID " + scale.get("scaleID") + " Exists"}, status=status.HTTP_400_BAD_REQUEST)
                except Scale.DoesNotExist:
                    replaceMajorMinorWithID(request.user, "beaconOne", "beaconOneDistance", scale)
                    replaceMajorMinorWithID(request.user, "beaconTwo", "beaconTwoDistance", scale)
                    replaceMajorMinorWithID(request.user, "beaconThree", "beaconThreeDistance", scale)
                    replaceMajorMinorWithID(request.user, "beaconFour", "beaconFourDistance", scale)

                    # ID does not exist so create using serializer
                    serializer = ScaleSerializer(data=scale)
                    if serializer.is_valid():
                        returnList.append(serializer)
                    else:
                        print("Ivalid post Scales class")
                        print(serializer.errors)
                        return Response({"ERROR":"Invalid Serializer"}, status=status.HTTP_400_BAD_REQUEST)
            returnData = {"created":[]}
            for serializer in returnList:
                serializer.save(owner=request.user)
                returnData["created"].append(serializer.data)
            return Response(returnData, status=status.HTTP_201_CREATED)
        return Response({"ERROR":"Scales Must Be In List Form Under 'List' Heading"}, status=status.HTTP_400_BAD_REQUEST)

"""
At url /core/scale
Get
    : For a specific scale information by passing id in the body
Post
    : For updating the storage num of a scale by adding the id and numStored in body
"""
class ScaleItemNums(APIView):
    def get(self, request, format=None):
        try:
            currentObj = Scale.objects.get(scaleID=request.data.get("scaleID"))
            serializer = ScaleNumberSerializer(currentObj)
            return Response(serializer.data, status=status.HTTP_200_OK)
        except Scale.DoesNotExist:
            return Response(status=status.HTTP_404_NOT_FOUND)
    
    def post(self, request, format=None):
        pprint(request.data)
        updatedObjs = []
        newSerializers = []
        if "scales" in request.data:
            for scale in request.data.get("scales"):
                try:
                    if "scaleID" in scale and "numStored" in scale:
                        currentObj = Scale.objects.filter(owner=request.user).get(scaleID=scale.get("scaleID"))
                        currentObj.numStored = scale.get("numStored")
                        updatedObjs.append(currentObj)
                except Scale.DoesNotExist:
                    # ID does not exist so create using serializer
                    serializer = ScaleSerializer(data=scale)
                    if serializer.is_valid():
                        newSerializers.append(serializer)
                    else:
                        print("Invalid ser")
                        print(serializer.errors)
                        return Response({"ERROR":"Invalid Serializer"}, status=status.HTTP_400_BAD_REQUEST)
            
            for updateObj in updatedObjs:
                updateObj.save()
            
            for newSer in newSerializers:
                newSer.save(owner=request.user)

            serializer = ScaleSerializer(updatedObjs, many=True)
            return Response(serializer.data + [ser.data for ser in newSerializers], status=status.HTTP_200_OK)

        return Response(status=status.HTTP_400_BAD_REQUEST)

"""
At url /core/settings
Get
    : For getting all scales that have settings that need updating
Post
    : For taking a list of scales and their settings and updating them
"""
class ScaleSettings(APIView):
    def get(self, request, format=None):
        ownedScales = Scale.objects.filter(owner=request.user)
        # Djongo requires workaround for querying by a boolean
        scaleSettings = ownedScales.filter(hasUpdatedSettings__in=[True])
        serializer = ScaleSerializer(scaleSettings, many=True)
        # print(serializer.data)
        return Response(serializer.data, status=status.HTTP_200_OK)

    def post(self, request, format=None):
        pprint(request.data)
        if "scales" in request.data:
            print("Has Scales")
            updateobjs = []
            newSerializers = []
            userScales = Scale.objects.filter(owner=request.user)
            for scale in request.data.get("scales"):
                print("Loop")
                if "scaleID" in scale:
                    try:
                        currentScale = userScales.get(scaleID=scale.get("scaleID"))

                        print("Found existing")

                        if "name" in scale:
                            currentScale.name = scale.get("name")

                        if "numStored" in scale:
                            currentScale.numStored = scale.get("numStored")

                        if "weightGramsPerXItmes" in scale:
                            currentScale.weightGramsPerXItmes = scale.get("weightGramsPerXItmes")

                        if "numItemsPerWeight" in scale:
                            currentScale.numItemsPerWeight = scale.get("numItemsPerWeight")

                        if "minNumItems" in scale:
                            currentScale.minNumItems = scale.get("minNumItems")

                        if "okNumItems" in scale:
                            currentScale.okNumItems = scale.get("okNumItems")

                        if "updateSettings" in scale:
                            currentScale.hasUpdatedSettings = scale.get("updateSettings")

                        if "beaconOne" in scale:
                            print("Beacon One in scale")
                            sentMajor = scale.get("beaconOne").get("major")
                            sentMinor = scale.get("beaconOne").get("minor")

                            foundBeacon = getBeaconFromMajorMinor(request.user, sentMajor, sentMinor)

                            if(foundBeacon != -1):
                                print("Found beacon one")
                                currentScale.beaconOne = foundBeacon
                                currentScale.beaconOneDistance = scale.get("beaconOne").get("distance")
                        
                        if "beaconTwo" in scale:
                            sentMajor = scale.get("beaconTwo").get("major")
                            sentMinor = scale.get("beaconTwo").get("minor")

                            foundBeacon = getBeaconFromMajorMinor(request.user, sentMajor, sentMinor)

                            if(foundBeacon != -1):
                                currentScale.beaconTwo = foundBeacon
                                currentScale.beaconTwoDistance = scale.get("beaconTwo").get("distance")
                        
                        if "beaconThree" in scale:
                            sentMajor = scale.get("beaconThree").get("major")
                            sentMinor = scale.get("beaconThree").get("minor")

                            foundBeacon = getBeaconFromMajorMinor(request.user, sentMajor, sentMinor)

                            if(foundBeacon != -1):
                                currentScale.beaconThree = foundBeacon
                                currentScale.beaconThreeDistance = scale.get("beaconThree").get("distance")

                        if "beaconFour" in scale:
                            sentMajor = scale.get("beaconFour").get("major")
                            sentMinor = scale.get("beaconFour").get("minor")

                            foundBeacon = getBeaconFromMajorMinor(request.user, sentMajor, sentMinor)

                            if(foundBeacon != -1):
                                currentScale.beaconFour = foundBeacon
                                currentScale.beaconFourDistance = scale.get("beaconFour").get("distance")

                        if "nonDevice" in request.data:
                            print("Has Updated True")
                            currentScale.hasUpdatedSettings = True
                        
                        updateobjs.append(currentScale)
                    except Scale.DoesNotExist as e:
                        replaceMajorMinorWithID(request.user, "beaconOne", "beaconOneDistance", scale)
                        replaceMajorMinorWithID(request.user, "beaconTwo", "beaconTwoDistance", scale)
                        replaceMajorMinorWithID(request.user, "beaconThree", "beaconThreeDistance", scale)

                        # ID does not exist so create using serializer
                        serializer = IDScaleSerializer(data=scale)
                        if serializer.is_valid():
                            newSerializers.append(serializer)
                        else:
                            print("Invalid ser")
                            print(serializer.errors)
                            print(scale)
                            # return Response({"ERROR":"Invalid Serializer"}, status=status.HTTP_400_BAD_REQUEST)
                    except Beacon.DoesNotExist as e:
                        print(e)
                else:
                    return Response(status=status.HTTP_400_BAD_REQUEST)
            
            updated = {"updated":[]}

            print("Output Updates")

            for obj in updateobjs:
                print("Update Obj")
                print(obj)
                obj.save()
                updated["updated"].append(obj.scaleID)
            
            print("Output Sers")

            for ser in newSerializers:
                print("New Ser")
                ser.save(owner=request.user)
                updated["updated"].append(ser.data["scaleID"])

            print("return")

            return Response(updated, status=status.HTTP_200_OK)
        else:
            return Response(status=status.HTTP_400_BAD_REQUEST)
        return Response(status=status.HTTP_200_OK)

"""
At url /core/has_updated
Post
    : Used for confirming a scale has successfully updated its settings. Takes a list
    with json objects containing the id of the scale that has successfully updated.
    (If 1 of the ids does not exist none will update)
"""
class RecievedUpdate(APIView):
    def post(self, request, format=None):
        if "scales" in request.data:
            updateobjs = []
            for scale in request.data.get("scales"):
                if "scaleID" in scale:
                    currentScale = Scale.objects.filter(owner=request.user).get(scaleID=scale.get("scaleID"))
                    currentScale.hasUpdatedSettings = False
                    updateobjs.append(currentScale)
                else:
                    return Response(status=status.HTTP_400_BAD_REQUEST)
            
            for obj in updateobjs:
                obj.save()

            serializer = ScaleSerializer(updateobjs, many=True)
            return Response(serializer.data, status=status.HTTP_200_OK)
        else:
            return Response(status=status.HTTP_400_BAD_REQUEST)
        return Response(status=status.HTTP_200_OK)

"""
"""
class Beacons(APIView):
    def get(self, request, format=None):
        ownedBeacons = Beacon.objects.filter(owner=request.user)
        serializer = BeaconSerializer(ownedBeacons, many=True)
        return Response(serializer.data, status=status.HTTP_200_OK)

    def post(self, request, format=None):
        if "beacons" in request.data:
            updateobjs = []
            newSerializers = []
            userBecons= Beacon.objects.filter(owner=request.user)
            for beacon in request.data.get("beacons"):
                if "major" in beacon and "minor" in beacon:
                    try:
                        currentBeacon = userBecons.filter(major=beacon.get("major")).get(minor=beacon.get("minor"))
                        print("Beacon exists")

                        if "name" in beacon:
                            currentBeacon.name = beacon.get("name")

                        if "major" in beacon:
                            currentBeacon.major = beacon.get("major")

                        if "minor" in beacon:
                            currentBeacon.minor = beacon.get("minor")

                        if "xPos" in beacon:
                            currentBeacon.xPos = beacon.get("xPos")

                        if "yPos" in beacon:
                            currentBeacon.yPos = beacon.get("yPos")

                        if "zPos" in beacon:
                            currentBeacon.zPos = beacon.get("zPos")
                        
                        updateobjs.append(currentBeacon)
                    except Beacon.DoesNotExist as e:
                        # ID does not exist so create using serializer
                        print("Beacon does not exist")
                        serializer = BeaconSerializer(data=beacon)
                        if serializer.is_valid():
                            newSerializers.append(serializer)
                        else:
                            print("Invalid ser")
                            print(serializer.errors)
                            print(beacon)
                            # return Response({"ERROR":"Invalid Serializer"}, status=status.HTTP_400_BAD_REQUEST)
                else:
                    return Response(status=status.HTTP_400_BAD_REQUEST)
            
            updated = {"updated":[]}

            print("Output Updates")

            for obj in updateobjs:
                obj.save()
                updated["updated"].append(BeaconSerializer(obj).data)
            
            print("Output Sers")

            for ser in newSerializers:
                ser.save(owner=request.user)
                updated["updated"].append(ser.data)

            print("return")

            return Response(updated, status=status.HTTP_200_OK)
        else:
            return Response(status=status.HTTP_400_BAD_REQUEST)

    def delete(self, request, format=None):
        if "beacons" in request.data:
            for beaconToDelete in request.data["beacons"]:
                if "major" in beaconToDelete and "minor" in beaconToDelete:
                    try:
                        toDelete = Beacon.objects.filter(owner=request.user).filter(major=beaconToDelete["major"]).get(minor=beaconToDelete["minor"])
                        toDelete.delete()
                    except Beacon.DoesNotExist as e:
                        print(e)
                print(beaconToDelete)
        else:
            return Response(status=status.HTTP_400_BAD_REQUEST)
        return Response(status=status.HTTP_200_OK)

@api_view(['GET'])
def current_user(request):
    """
    Determine the current user by their token, and return their data
    """
    
    serializer = UserSerializer(request.user)
    return Response(serializer.data)

@api_view(['GET'])
def scale_location(request, scale_id):
    try:
        scale = Scale.objects.filter(owner=request.user).get(scaleID=scale_id)
        
        if(scale.beaconOne != None and scale.beaconTwo != None and scale.beaconThree != None and scale.beaconFour != None):
            locationsAndDistances = []
            locationsAndDistances.append((scale.beaconOne.xPos, scale.beaconOne.yPos, scale.beaconOne.zPos, scale.beaconOneDistance))
            locationsAndDistances.append((scale.beaconTwo.xPos, scale.beaconTwo.yPos, scale.beaconTwo.zPos, scale.beaconTwoDistance))
            locationsAndDistances.append((scale.beaconThree.xPos, scale.beaconThree.yPos, scale.beaconThree.zPos, scale.beaconThreeDistance))
            locationsAndDistances.append((scale.beaconFour.xPos, scale.beaconFour.yPos, scale.beaconFour.zPos, scale.beaconFourDistance))

            position = getXYZFromPositionsAndDistances(locationsAndDistances)

            location = {"location":position}

            return Response(location, status=status.HTTP_200_OK)

        return Response(status=status.HTTP_500_INTERNAL_SERVER_ERROR)
    except Scale.DoesNotExist as e:
        return Response(status=status.HTTP_400_BAD_REQUEST)

class UserList(APIView):
    """
    Create a new user. It's called 'UserList' because normally we'd have a get
    method here too, for retrieving a list of all User objects.
    """

    permission_classes = (permissions.AllowAny,)

    def post(self, request, format=None):
        serializer = UserSerializerWithToken(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)