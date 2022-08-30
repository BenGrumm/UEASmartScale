import React, {useEffect, useState} from 'react';
import { Text, View, FlatList } from 'react-native';
import { List, ListItem } from "react-native-elements";
import { useIsFocused } from "@react-navigation/native";

import {styles, Colors} from '../components/styles';
import {get_scales} from "../Auth";
import ScaleListComponent from "../components/ScaleListComponent";

const Home = ({navigation}) => {

  const isFocused = useIsFocused();

  const [scales, setScales] = useState([]);
  const [isFetching, setIsFetching] = useState(false);

  useEffect(() => {
    if(isFocused){
      refresh();
    }
  }, [isFocused]);

  async function refresh(){
    setIsFetching(true);
    get_scales(false)
    .then(response => response.json()).then(data => {
        console.log(data);
        setScales(data);
        setIsFetching(false);
    }).catch(error=>{
      setIsFetching(false);
    });
  }

  const ItemDivider = () => {
    return (
      <View
        style={{
          height: 1,
          alignItems: 'center',
          alignSelf: 'center',
          width: "90%",
          backgroundColor: "#607D8B",
        }}
      />
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
          scales.length === 0 ? 
          <Text style={styles.titleText}> There Are No Devices Registered To You </Text>
          :
          <FlatList
            ListHeaderComponent={<ScaleListComponent header={true}/>}
            style={{alignSelf: "stretch"}}
            ItemSeparatorComponent={ItemDivider}
            data={scales}
            keyExtractor={item=>item.scaleID}
            renderItem={({item})=><ScaleListComponent navigation={navigation} scale={item} header={false} scales={scales}/>}
            onRefresh={() => refresh()}
            refreshing={isFetching}
          />
        }
    </View>
  )
}
export default Home;