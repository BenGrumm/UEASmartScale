import React, {useState} from 'react';
import {StatusBar} from 'expo-status-bar';
import { Text, View, TouchableOpacity, TextInput } from 'react-native';
import {styles, Colors} from '../components/styles';
import {AuthContext} from "../../App";
import {isAuthenticated, loginForReturn, saveData} from "../Auth";

const Settings = () => {
  const {signOut} = React.useContext(AuthContext);

  return (
    <View
      style={{
        flex: 1,
        justifyContent: "center",
        alignItems: "center"
      }}>
        <TouchableOpacity style={styles.stylePrimaryButton} onPress={() => {signOut()}}>
          <Text style={styles.primaryButtonText}>Sign Out</Text>
        </TouchableOpacity>
    </View>
  )
}
export default Settings;