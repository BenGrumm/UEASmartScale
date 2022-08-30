import React, {useState, useEffect} from 'react';
import { Text, View, TouchableOpacity, TextInput, ScrollView, Button } from 'react-native';
import { FontAwesome5, AntDesign } from '@expo/vector-icons'; 

import {styles, Colors} from '../components/styles';
import {AuthContext} from "../../App";
import {isAuthenticated, update_beacon, delete_beacon} from "../Auth";

const CreateBeacon = ({navigation, route}) => {

  const {beacon} = route.params;

  // For button
  const [isLoading, setLoading] = useState(false);

  const [name, setName] = useState(beacon.name);
  const [major, setMajor] = useState(0);
  const [minor, setMinor] = useState(0);
  const [x, setX] = useState(beacon.xPos);
  const [y, setY] = useState(beacon.yPos);
  const [z, setZ] = useState(beacon.zPos);

  const [count, setCount] = React.useState(0);

  useEffect(() => {
    if(beacon.major !== null){
      setMajor(beacon.major);
    }

    if(beacon.minor !== null){
      setMinor(beacon.minor);
    }
  }, []);

  const deleteCurrent = () => {
    if(beacon.major !== null && beacon.minor !== null){
      console.log("DELETING");
      console.log(beacon.major + ", " + beacon.minor);
      delete_beacon(beacon.major, beacon.minor).then(
        response => {
          console.log("Delete Status = " + response.status);
        }
      ).finally(
        () => navigation.goBack()
      );
    }
  }

  React.useLayoutEffect(() => {
    if(beacon.major !== null && beacon.minor !== null){
      navigation.setOptions({
        headerRight: () => (
          <TouchableOpacity style={[styles.stylePrimaryButton, {padding: 10, marginRight:10, backgroundColor:Colors.third}]} onPress={deleteCurrent}>
            <AntDesign name="delete" size={20} color={Colors.first} style={{padding:2}}/>
          </TouchableOpacity>
        ),
      });
    }
  }, [navigation]);

  const handleSubmit =() => {
    console.log('Submit');
    console.log("Name: " + name + ", Major: " + major + ", Minor: " + minor + ", X: " + x + ", Y: " + y + ", Z: " + z);

    setLoading(true);

    let postObj = {
      "beacons":[

      ]
    };

    let newObj = {
      "name":name,
      "major": major,
      "minor": minor,
      "xPos": x,
      "yPos": y,
      "zPos": z,
    };

    postObj.beacons.push(newObj);

    console.log("Update");

    console.log(postObj);

    console.log("Deleting - " + (beacon.major != major || beacon.minor != minor));

    if(beacon.major !== null && beacon.minor !== null && (beacon.major != major || beacon.minor != minor)){
      // TODO delete old
      console.log("DELETING OLD");
      console.log(beacon.major + ", " + beacon.minor);
      delete_beacon(beacon.major, beacon.minor).then(
        response => {
          console.log("Delete Status = " + response.status);
        }
      );
    }

    update_beacon(postObj)
      .then(response => response.json())
      .then(data => {
        console.log(data);
        navigation.goBack();
      })
      .finally(() => setLoading(false));
  }

  const plusOne = ({val, setVal, plusComparison}) => {
    if(plusComparison(val)){
      setVal(val + 1);
    }
  }

  const minusOne = ({val, setVal, minusComparison}) => {
    if(minusComparison(val)){
      setVal(val - 1);
    }
  }

  const plusMinus = ({val, setVal, plusComparison, minusComparison}) => {

    return (
      <View
        style={{
          flex:1,
          flexDirection:"row",
          alignItems: "center",
          justifyContent: "space-between",
          width:"100%",
          paddingLeft:10,
          paddingRight:10,
      }}>
        <TouchableOpacity style={{backgroundColor:Colors.second, borderRadius:5}} onPress={() => {minusOne({val, setVal, minusComparison});}}>
          <AntDesign name="minus" size={30} color={Colors.first} style={{padding:2}}/>
        </TouchableOpacity>
        <TextInput 
          style={styles.subTitleText}
          keyboardType='numeric'
          value={val.toString()}
          onChangeText={text => {
            if(text != ""){
              let parsed = parseInt(text.replace(/[^0-9]/g, ''));
              if(minusComparison(parsed + 1) && plusComparison(parsed - 1)){
                setVal(parsed);
              }
            }else{
              if(minusComparison(0)){
                setVal(0);
              }
            }
          }}
        />
        <TouchableOpacity style={{backgroundColor:Colors.second, borderRadius:5}} onPress={() => {plusOne({val, setVal, plusComparison});}}>
          <AntDesign name="plus" size={30} color={Colors.first} style={{padding:2}}/>
        </TouchableOpacity>
      </View>
    );
  }

  const getCoordinate =({val, setVal}) => {
    return (
      <View
        style={{
          flex:1,
          flexDirection:"row",
          alignItems: "center",
          justifyContent: "space-between",
          width:"100%",
          paddingLeft:10,
          paddingRight:10,
      }}>
        <TextInput 
          style={styles.subTitleText}
          keyboardType='numeric'
          value={val.toString()}
          onChangeText={text => {setVal(text)}}
        />
      </View>
    );
  }

  return (
    <ScrollView>
    <View
      style={{
        flex: 1,
        justifyContent: "center",
        alignItems: "center"
      }}>
        <View
          style={{
            justifyContent: "center",
            alignItems: "center",
            flexDirection:"row",
            padding:10,
          }}>
            <FontAwesome5
              style={{
                width:"25%",
                alignItems:"center",
                alignSelf:"center",
                alignContent:"center",
                textAlign:"center",
                paddingLeft:20
              }}
              name="weight-hanging" 
              size={25} 
              color={Colors.fith}
            />
            <TextInput
              style={[{
                width:"75%",
                textAlign:"center",
              }, styles.titleText]}
              placeholder="Enter Name Here"
              placeholderTextColor={Colors.first}
              value={name}
              onChangeText={text => setName(text)}
            />
        </View>

        <View
          style={{
            height: 1,
            alignItems: 'center',
            alignSelf: 'center',
            width: "90%",
            backgroundColor: "#607D8B", 
        }}/>
          <View
            style={{
              alignItems: "flex-start",
              justifyContent:"flex-start",
              width:"90%",
              paddingTop:30,
            }}
          >

            <Text style={styles.titleText}>
              Major Value
            </Text>
            {plusMinus({
              val:major, 
              setVal:setMajor, 
              plusComparison: function(val){
                return true;
              },
              minusComparison: function(val){
                return val > 0;
              },
            })}
    
            <Text style={styles.titleText}>
              Minor Value
            </Text>
            {plusMinus({
              val:minor, 
              setVal:setMinor, 
              plusComparison: function(val){
                return true;
              },
              minusComparison: function(val){
                return val > 0;
              },
            })}

            <Text style={styles.titleText}>
              Set X Position (Metres)
            </Text>
            {getCoordinate({val:x, setVal:setX})}

            <Text style={styles.titleText}>
              Set Y Position (Metres)
            </Text>
            {getCoordinate({val:y, setVal:setY})}

            <Text style={styles.titleText}>
              Set Z Position (Metres)
            </Text>
            {getCoordinate({val:z, setVal:setZ})}

          </View>

        <View
          style={{
            alignItems: "flex-start",
            justifyContent:"flex-start",
            width:"90%",
            paddingTop:30,
          }}
        >
          <View style={{padding:30, alignSelf:"center"}}>
            <TouchableOpacity style={[styles.stylePrimaryButton, {alignSelf:"center"}]} onPress={handleSubmit}>
              {isLoading ? 
                  // <LoadingSpinner colour={Colors.second}/> : 
                  <Text style={styles.primaryButtonText}>Loading</Text>
                    :
                  <Text style={styles.primaryButtonText}>Save</Text>
              }
            </TouchableOpacity>
          </View>
        </View>
    </View>
    </ScrollView>
  )
}
export default CreateBeacon;