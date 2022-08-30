import React from "react";
import {createMaterialBottomTabNavigator} from "@react-navigation/material-bottom-tabs";

// ERROR IN IMPORT STACK NAVIGATORS
import {HomeStack, SettingsStack, BeaconStack} from './StackNavigators'
import {Colors} from "../components/styles";
import {Ionicons, MaterialCommunityIcons} from '@expo/vector-icons';
import Settings from "../screens/Settings"

const Tab = createMaterialBottomTabNavigator();

const TabNavigator = () => {

    return (
        <Tab.Navigator
            initialRouteName="Home"
            activeColor={Colors.first}
            inactiveColor={Colors.third}
            barStyle={{backgroundColor: Colors.second}}
        >

            <Tab.Screen
                name="Home"
                component={HomeStack}
                options={{
                    tabBarLabel: 'Home',
                    tabBarIcon: ({color}) => (
                        <Ionicons name="earth-outline" color={color} size={25}/>
                    ),
                    tabBarOptions: {}
                }}
            />

            <Tab.Screen
                name="Beacons"
                component={BeaconStack}
                options={{
                    tabBarLabel: 'Beacons',
                    tabBarIcon: ({color}) => (
                       <MaterialCommunityIcons name="signal-distance-variant" size={25} color={color} />
                    ),
                    tabBarOptions: {}
                }}
            />

            <Tab.Screen
                name="Settings"
                component={SettingsStack}
                options={{
                    tabBarLabel: 'Settings',
                    tabBarIcon: ({color}) => (
                        <Ionicons name="settings-outline" color={color} size={25}/>
                    ),
                    tabBarOptions: {}
                }}
            />

        </Tab.Navigator>
    );
}

export default TabNavigator;