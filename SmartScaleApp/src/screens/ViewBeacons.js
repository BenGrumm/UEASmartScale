import React, {useEffect, useState} from 'react';
import { Text, View, FlatList } from 'react-native';
import { List, ListItem } from "react-native-elements";
import { useIsFocused } from "@react-navigation/native";
import { FAB } from 'react-native-paper';

import {styles, Colors} from '../components/styles';
import {get_beacons} from "../Auth";
import BeaconListComponent from '../components/BeaconListComponent';

const Home = ({navigation}) => {

  const isFocused = useIsFocused();

  const [beacons, setBeacons] = useState([]);
  const [isFetching, setIsFetching] = useState(false);

  useEffect(() => {
    if(isFocused){
      refresh();
    }
  }, [isFocused]);

  async function refresh(){
    setIsFetching(true);
    get_beacons()
    .then(response => response.json()).then(data => {
        setBeacons(data);
        setIsFetching(false);
    }).catch(error=>{
      setIsFetching(false);
    });
  }

  const ItemDivider = () => {
    return (
      <View style={styles.divider}/>
    );
  }

  return (
    <View
      style={{
        flex: 1,
        justifyContent: "center",
        alignItems: "center",
        alignSelf: "stretch",
      }}>
        {
          beacons.length === 0 ? 
          <Text style={styles.titleText}> There Are No Beacons Registered To You </Text>
          :
          <FlatList
            style={{alignSelf: "stretch"}}
            ItemSeparatorComponent={ItemDivider}
            data={beacons}
            keyExtractor={item=>item.id}
            renderItem={({item})=><BeaconListComponent navigation={navigation} beacon={item} header={false}/>}
            onRefresh={() => refresh()}
            refreshing={isFetching}
          />
        }
        <FAB
          style={styles.fab}
          medium
          icon="plus"
          color={Colors.first}
          onPress={() => navigation.navigate('CreateBeacon', {"beacon":{"name":"", "major":null, "minor":null, "xPos":0, "yPos":0, "zPos":0}})}
        />
    </View>
  )
}
export default Home;