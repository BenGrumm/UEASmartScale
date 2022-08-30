import React, {useEffect, useState} from 'react';
import { Text, View, FlatList, TouchableOpacity } from 'react-native';
import { List, ListItem } from "react-native-elements";
import { FontAwesome5, AntDesign } from '@expo/vector-icons'; 

import {styles, Colors} from '../components/styles';
import {get_scales} from "../Auth";

const ScaleListComponent = ({navigation, scale, header, scales}) => {

  const getIcon = () => {

    let icon = null;

    if(scale.numStored <= scale.minNumItems){
      icon = <AntDesign name="closecircle" size={24} color="red" />
    }else if(scale.numStored <= scale.okNumItems){
      icon = <AntDesign name="minuscircle" size={24} color="orange" />
    }else{
      icon = <AntDesign name="checkcircle" size={24} color="green" />
    }

    return(
      icon
    );
  }

  const getName = () => {
    if(header){
      return "Scale ID/Name";
    }else if(scale.name !== undefined && scale.name !== ""){
      return `Name: ${scale.name}`;
    }else{
      return `ID: ${scale.scaleID}`;
    }
  }

  return (
    <TouchableOpacity

      onPress={() => {
          if(!header){
            console.log("Clicked on " + scale.scaleID);
            navigation.navigate('ViewScale', {scale:scale, scales:scales});
          }
        }
      }  

      style={[{
        flex: 1,
        paddingTop: 10,
        paddingBottom: 10,
        justifyContent: "center",
        alignItems: "center",
        flexDirection: "row",
        flexWrap: "wrap",
        alignSelf: "stretch",
        // backgroundColor:'#FFFF00',
      }, header ? styles.listHeaderStyle : {}]}>
      <View style={{
        flex:1,
        flexWrap:"wrap",
        alignSelf: "stretch",
        justifyContent:'center',
        alignItems:'center',
        alignContent:'center',
        // backgroundColor:'#FF00FF',
      }}>
        {
          header ?
          <FontAwesome5 
            name="tools" 
            size={20} 
            color={Colors.fith} 
          />
          :
          getIcon()
        }
      </View>
      <View style={{
          flex:3,
          padding:15, 
          alignItems:'flex-start',
          // backgroundColor:'#FF00FF',
          textAlign:'center',
      }}>
        <Text style={{
          justifyContent:'center',
          textAlign:'center',}, 
          styles.styledBoxText
        }>
          {getName()}
        </Text>
      </View>
      <View style={{
        flex:1.5,
        flexDirection:"row",
        alignContent:'center',
        alignItems:'center',
        paddingRight:20
      }}>
        <View style={{
          padding:5,
          justifyContent:'flex-start',
        }}>
          <FontAwesome5 name="cloudscale" size={20} color={Colors.fith}/>
        </View>
        <Text style={[
          styles.styledBoxText,
          {
            justifyContent:'center',
            paddingRight: 10,
          }
        ]}>
          {header ? "Number Of Items" : `${scale.numStored}`}
        </Text>
      </View>
    </TouchableOpacity>
  )
}
export default ScaleListComponent;