import React from "react";
import {Colors} from "../components/styles";

import {createStackNavigator} from '@react-navigation/stack';

import Home from '../screens/Home';
import ViewScale from '../screens/ViewScale';
import ViewBeacons from '../screens/ViewBeacons';
import Settings from '../screens/Settings';
import CreateBeacon from '../screens/CreateBeacon';

const Stack = createStackNavigator();

const screenOptionStyle = {
    headerStyle: {
        backgroundColor: Colors.second,
    },
    headerTintColor: Colors.first,
    headerTitleStyle: {
        fontWeight: 'bold',
    },
}

const HomeStack = () => {
    return (
        <Stack.Navigator
            screenOptions={screenOptionStyle}
            initialRouteName={"Home_List"}
        >
            <Stack.Screen name="Home_List" component={Home} options={{ title: 'Your Scales' }}/>
            <Stack.Screen name="ViewScale" component={ViewScale} options={{ title: 'Update Device Settings' }}/>
        </Stack.Navigator>
    );
}

const BeaconStack = () => {
    return (
        <Stack.Navigator
            screenOptions={screenOptionStyle}
            initialRouteName={"Beacons"}
        >
            <Stack.Screen name="BeaconList" component={ViewBeacons} options={{ title: 'Your Beacons' }}/>
            <Stack.Screen name="CreateBeacon" component={CreateBeacon} options={{title:'Create Beacon'}}/>
            <Stack.Screen name="EditBeacon" component={CreateBeacon} options={{title:'Edit Beacon'}}/>
        </Stack.Navigator>
    );
}

const SettingsStack = () => {
    return (
        <Stack.Navigator
            screenOptions={screenOptionStyle}
            initialRouteName={"Settings"}
        >
            <Stack.Screen name="Settings_Stack" component={Settings} options={{ title: 'Settings' }}/>
        </Stack.Navigator>
    );
}


export {HomeStack, SettingsStack, BeaconStack};