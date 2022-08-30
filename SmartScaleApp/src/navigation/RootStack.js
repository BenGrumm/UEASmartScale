import React from "react";
import {Colors} from "../components/styles";

import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';

// screens
import Login from '../screens/Login';
import Register from '../screens/Register'
import Home from "../screens/Home";

const Stack = createStackNavigator();

const RootStack = () => {
    return (
          <Stack.Navigator
            screenOptions={{
                headerTintColor: Colors.third,
                headerShown: false,
                headerTransparent: true,
                headerTitle: '',
                headerLeftContainerStyle: {
                    paddingLeft: 20
                },

            }}
            initialRouteName={"Login"}
          >
              <Stack.Screen name="Login" component={Login}/>
              <Stack.Screen name="Register" component={Register}/>
          </Stack.Navigator>
    );
}

export default RootStack;