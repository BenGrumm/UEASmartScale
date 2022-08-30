import React, {useState, useEffect} from 'react';
import { Text, View, TouchableOpacity, TextInput, ScrollView, Dimensions  } from 'react-native';
import { FontAwesome5, AntDesign } from '@expo/vector-icons'; 
import { Dropdown } from 'react-native-element-dropdown';
import ScatterChart from 'react-native-scatter-chart';

import {styles, Colors} from '../components/styles';
import {AuthContext} from "../../App";
import {isAuthenticated, update_scale, get_location} from "../Auth";

const ViewScale = ({route}) => {

  // For dropdown
  const [value, setValue] = useState(null);
  const [isFocus, setIsFocus] = useState(false);

  // For button
  const [isLoading, setLoading] = useState(false);

  const [name, setName] = useState(route.params.scale.name);
  const [minNumItems, setMinNumItems] = useState(route.params.scale.minNumItems);
  const [okNumItems, setOkNumItems] = useState(route.params.scale.okNumItems);
  const [numItemsPerX, setNumItemsPerX] = useState(route.params.scale.numItemsPerWeight);
  const [weightPerXItems, setWeightPerXItems] = useState(route.params.scale.weightGramsPerXItmes);

  const [chartData, setChartData] = useState([]);
  const [minMaxData, setMinMaxData] = useState([]);

  const [scales, setScales] = useState(route.params.scales);

  const [errorMsg, setErrorMsg] = useState("");

  useEffect(() => {
    for(let i = 0; i < scales.length; i++){
      if(scales[i].name === undefined || scales[i].name === ""){
        scales[i].name = scales[i].scaleID.toString();
      }
    }

    if(
      route.params.scale.beaconOne !== null || route.params.scale.beaconOne !== undefined 
        &&
      route.params.scale.beaconTwo !== null || route.params.scale.beaconTwo !== undefined
        &&
      route.params.scale.beaconThree !== null || route.params.scale.beaconThree !== undefined
        &&
      route.params.scale.beaconFour !== null || route.params.scale.beaconFour !== undefined
      ){
      getScaleLocation();
    }

    if(name === ""){
      setName(route.params.scale.scaleID.toString());
    }
  }, []);

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

  const getIcon = () => {

    let icon = null;

    if(route.params.scale.numStored <= minNumItems){
      icon = <AntDesign name="closecircle" size={24} color="red" />
    }else if(route.params.scale.numStored <= okNumItems){
      icon = <AntDesign name="minuscircle" size={24} color="orange" />
    }else{
      icon = <AntDesign name="checkcircle" size={24} color="green" />
    }

    return(
      icon
    );
  }

  const getScaleLocation = async () => {
    console.log("Get Scale Location");
    console.log(route.params.scale);
    let resp = await get_location(route.params.scale.scaleID);

    if(resp.status === 200){
      console.log("Got return");
      let json = await resp.json();

      let chartData = [
        {
            color: 'red',
            values: [
              [route.params.scale.beaconOne.xPos, route.params.scale.beaconOne.yPos],
              [route.params.scale.beaconTwo.xPos, route.params.scale.beaconTwo.yPos],
              [route.params.scale.beaconThree.xPos, route.params.scale.beaconThree.yPos],
              [route.params.scale.beaconFour.xPos, route.params.scale.beaconFour.yPos]]
        },
        {
            color: 'black',
            values: [
              [
                json.location[0][0], 
                json.location[1][0]
              ]
            ]
        }
      ];

      let minX = json.location[0][0];
      let minY = json.location[1][0];
      let maxX = json.location[0][0];
      let maxY = json.location[1][0];

      for(let i = 0; i < chartData[0].values.length; i++){
        if(chartData[0].values[i][0] < minX){
          minX = chartData[0].values[i][0];
        }else if(chartData[0].values[i][0] > maxX){
          maxX = chartData[0].values[i][0];
        }

        if(chartData[0].values[i][1] < minY){
          minY = chartData[0].values[i][1];
        }else if(chartData[0].values[i][1] > maxY){
          maxY = chartData[0].values[i][1];
        }
      }

      console.log("Set Chart Data");
      console.log(chartData);

      setMinMaxData([minX, maxX, minY, maxY]);
      setChartData(chartData);
    }else{
      console.log("!= 200")
    }
  }

  const handleSubmit =() => {
    console.log('Submit');
    console.log("ID: " + route.params.scale.scaleID + ", Name: " + name + ", Min Num: " + minNumItems + ", Ok Num: " + okNumItems + ", Num Items Per: " + numItemsPerX + ", Weight Per: " + weightPerXItems);
    if(minNumItems > 0 && okNumItems > minNumItems && numItemsPerX > 0 && weightPerXItems > 0){

      setLoading(true);

      let postObj = {
        "scales":[]
      };

      let newObj = {"scaleID":route.params.scale.scaleID, "updateSettings":true};
      let shouldUpdate = false;

      if(name != route.params.scale.scaleID.toString()){
        newObj["name"] = name;
        shouldUpdate = true;
      }

      if(route.params.scale.minNumItems !== minNumItems){
        newObj["minNumItems"] = minNumItems;
        shouldUpdate = true;
      }

      if(route.params.scale.okNumItems != okNumItems){
        newObj["okNumItems"] = okNumItems;
        shouldUpdate = true;
      }

      if(route.params.scale.numItemsPerWeight != numItemsPerX){
        newObj["numItemsPerWeight"] = numItemsPerX;
        shouldUpdate = true;
      }

      if(route.params.scale.weightGramsPerXItmes != weightPerXItems){
        newObj["weightGramsPerXItmes"] = weightPerXItems;
        shouldUpdate = true;
      }

      postObj.scales.push(newObj);

      if(shouldUpdate){
        // TODO Post
        console.log("Update");
        update_scale(postObj)
        .then(response => response.json())
        .then(data => console.log(data))
        .finally(() => {setLoading(false); setErrorMsg("");});
      }else{
        setLoading(false);
        setErrorMsg("");
      }
    }else{
      let localErrorMsg = "";

      if(minNumItems === null || minNumItems <= 0){
        localErrorMsg = "Min Num Items Must Be Positive Value";
      }

      if(okNumItems === null || okNumItems <= minNumItems){
        localErrorMsg = "Ok Num Items Must Be More Than Min Num";
      }

      if(numItemsPerX === null || numItemsPerX <= 0){
        localErrorMsg = "Num Items Per X Must Be Positive";
      }

      if(weightPerXItems === null || weightPerXItems <= 0){
        localErrorMsg = "Weight Per X Items Must Be Positive";
      }

      setErrorMsg(localErrorMsg);
    }
  }

  const plusMinus = ({val, setVal, plusComparison, minusComparison}) => {

    let valToUse = "";

    if(val !== null){
      valToUse = val.toString();
    }

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
          value={valToUse}
          onChangeText={text => {
            if(text != ""){
              let parsed = parseInt(text.replace(/[^0-9]/g, ''));
              setVal(parsed);
            }else{
              setVal(null);
            }
          }}
        />
        <TouchableOpacity style={{backgroundColor:Colors.second, borderRadius:5}} onPress={() => {plusOne({val, setVal, plusComparison});}}>
          <AntDesign name="plus" size={30} color={Colors.first} style={{padding:2}}/>
        </TouchableOpacity>
      </View>
    );
  }

  const getPositionData = () => {
    if(chartData.length === 0){
      return(
        <Text style={styles.subTitleText}>
          No Position Data Found
        </Text>
      );
    }else{
      return(
        <View>
          <View>
            <ScatterChart
              backgroundColor='#0000000A'
              data={chartData}
              unitY='meters'
              unitX='meters'
              minX={(minMaxData[0] - 1)}
              maxX={(minMaxData[1] + 1)}
              minY={(minMaxData[2] - 1)}
              maxY={(minMaxData[3] + 1)}
            />
          </View>
    
          <View style={{flex:1, flexDirection:"row", justifyContent: "space-around"}}>
            <Text style={styles.subTitleText}>
              X: {(Math.round(chartData[1].values[0][0] * 100) / 100).toFixed(2)}m
            </Text>
            <Text style={styles.subTitleText}>
              Y: {(Math.round(chartData[1].values[0][1] * 100) / 100).toFixed(2)}m
            </Text>
          </View>
        </View>
      );
    }
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
          ID
        </Text>
        <Text style={styles.subTitleText}>{route.params.scale.scaleID}</Text>
        <Text style={styles.titleText}>
          Number Of Items
        </Text>
        <View
          style={{
            flexDirection:"row",
            alignItems:"center",
            justifyContent: 'center',
            textAlignVertical:"center",
            paddingLeft:15,
          }}>
          {
            getIcon()
          }
          <Text style={[styles.subTitleText, {paddingLeft:15}]}>{route.params.scale.numStored}</Text>
        </View>
        <Text style={styles.titleText}>
          Minimum Number Of Items
        </Text>
        {plusMinus({
          val:minNumItems, 
          setVal:setMinNumItems, 
          plusComparison: function(val){
            return true;
          },
          minusComparison: function(val){
            return val > 1;
          },
        })}
        <Text style={styles.titleText}>
          Ok Number Of Items
        </Text>
        {plusMinus({
          val:okNumItems, 
          setVal:setOkNumItems, 
          plusComparison: function(val){
            return true;
          },
          minusComparison: function(val){
            return val > 1;
          },
        })}
        <Text style={styles.titleText}>
          Number Of Items Per {weightPerXItems}g
        </Text>
        {plusMinus({
          val:numItemsPerX, 
          setVal:setNumItemsPerX, 
          plusComparison: function(val){
            return true;
          },
          minusComparison: function(val){
            return val > 1;
          },
        })}
        <Text style={styles.titleText}>
          Weight Per {numItemsPerX} Items (grams)
        </Text>
        {plusMinus({
          val:weightPerXItems, 
          setVal:setWeightPerXItems, 
          plusComparison: function(val){
            return true;
          },
          minusComparison: function(val){
            return val > 1;
          },
        })}

        <View style={{padding:15, paddingTop:30, alignSelf:"center"}}>
          <TouchableOpacity style={[styles.stylePrimaryButton, {alignSelf:"center"}]} onPress={handleSubmit}>
            {isLoading ? 
                // <LoadingSpinner colour={Colors.second}/> : 
                <Text style={styles.primaryButtonText}>Loading</Text>
                  :
                <Text style={styles.primaryButtonText}>Save</Text>
            }
          </TouchableOpacity>
        </View>

        <Text style={[styles.subTitleText, {alignSelf:"center", textAlign:"center"}]}>
          {errorMsg}
        </Text>

        <Text style={[styles.titleText, {alignSelf:"center", textAlign:"center"}]}>
          Apply profile of another scale
        </Text>
        <View style={{width:"100%", padding:15}}>
          <Dropdown
            style={[styles.dropdown, isFocus && { borderColor: 'blue' }]}
            placeholderStyle={styles.placeholderStyle}
            selectedTextStyle={styles.selectedTextStyle}
            inputSearchStyle={styles.inputSearchStyle}
            iconStyle={styles.iconStyle}
            data={scales}
            search
            maxHeight={500}
            labelField="name"
            valueField="name"
            placeholder={!isFocus ? 'Select settings profile' : '...'}
            searchPlaceholder="Search..."
            value={value}
            onFocus={() => setIsFocus(true)}
            onBlur={() => setIsFocus(false)}
            onChange={item => {
              console.log("Set settings from dropdown");
              console.log(item);
              setValue(item);
              setMinNumItems(item.minNumItems);
              setOkNumItems(item.okNumItems);
              setNumItemsPerX(item.numItemsPerWeight);
              setWeightPerXItems(item.weightGramsPerXItmes);
              setIsFocus(false);
            }}
            renderLeftIcon={() => (
              <AntDesign
                style={styles.icon}
                color={isFocus ? 'blue' : 'black'}
                name="Safety"
                size={20}
              />
            )}
          />
        </View>
      </View>
    </View>

    <Text style={styles.titleText}>
      Scale Location
    </Text>

    {getPositionData()}

    </ScrollView>
  );
}
export default ViewScale;