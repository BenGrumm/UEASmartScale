import React, {useEffect, useState} from 'react';
import { Text, View, FlatList, TouchableOpacity } from 'react-native';
import { List, ListItem } from "react-native-elements";
import { FontAwesome5, AntDesign } from '@expo/vector-icons'; 
import Checkbox from 'expo-checkbox';

import {styles, Colors} from '../components/styles';
import {get_scales} from "../Auth";

const BeaconListComponent = ({navigation, beacon, header}) => {

  const [toggleCheckBox, setToggleCheckBox] = useState(false);

  return (
    <View
      style={{
        flex: 1,
        paddingTop: 5,
        paddingBottom: 5,
        justifyContent: "space-evenly",
        alignItems: "center",
        flexDirection: "row",
        flexWrap: "wrap",
        alignSelf: "stretch",
        // backgroundColor:'#FFFF00',
      }}
    >
      <TouchableOpacity
        style={{padding:10, flexDirection: "row", alignSelf: "stretch", justifyContent:"space-evenly", width:"70%",}}
        onPress={() => {
          navigation.navigate('CreateBeacon', {"beacon":beacon});
        }}>
        <View style={{justifyContent:"center", width:"60%"}}>
          <Text style={[styles.styledBoxText, {alignSelf:"flex-start", textAlign:"left", alignContent:"center", justifyContent:"flex-start"}]}>{beacon.name === undefined ? "ID:" : "Name:"}</Text>
          <Text style={styles.titleIshText}>{beacon.name === undefined ? `${beacon.id}` : `${beacon.name}`}</Text>
        </View>
        <View>
          <Text style={styles.styledBoxText}>X: {beacon.xPos}</Text>
          <Text style={styles.styledBoxText}>Y: {beacon.yPos}</Text>
          <Text style={styles.styledBoxText}>Z: {beacon.zPos}</Text>
        </View>
      </TouchableOpacity>
      <Checkbox
        value={toggleCheckBox}
        onValueChange={setToggleCheckBox}
      />
    </View>
  )
}
export default BeaconListComponent;