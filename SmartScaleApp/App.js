import React from 'react';

// react native navigation tabs/stacks
import { createStackNavigator, StackView } from '@react-navigation/stack';
import {NavigationContainer} from "@react-navigation/native";

import RootStack from './src/navigation/RootStack';
import TabNavigator from './src/navigation/TabNavigator';

import {
  getUserName, 
  getPassword, 
  isAuthenticated, 
  deleteUserName, 
  deletePassword, 
  getAccessToken, 
  deleteAccessToken, 
  reauthenticate
} from './src/Auth';

export const AuthContext = React.createContext();
const Stack = createStackNavigator();

export default function App({navigation}) {
    const [state, dispatch] = React.useReducer(
      (prevState, action) => {
        switch (action.type) {

          case 'RESTORE_USERNAME':
            return {
              ...prevState,
              userName: action.name,
              userToken: action.token,
              isLoading: false,
            };
          
          case 'SIGN_IN':
            return {
              ...prevState,
              isSignout: false,
              userToken: action.token,
              userName: action.name,
            };
          
          case 'SIGN_OUT':
            return {
              ...prevState,
              isSignout: true,
              userToken: null,
              userName: null,
            };
        }
      },
      {
        isLoading: true,
        isSignout: false,
        userToken: null,
        userName: null,
    });

    /* checks token on local storage exists */
  React.useEffect(() => {

    console.log("react");

    const bootstrapAsync = async () => {
      
      let localUserName = await getUserName();
      let localPassword = await getPassword();
  
      /* checks if token has expired and requests new one */
      if(!((await isAuthenticated()).valueOf())){
        // If username and password are not null or empty
        if(localUserName !== null && localUserName.res !== "" && localPassword !== null && localPassword.res !== ""){
          // Retry auth
          if(!(await reauthenticate())){
            return;
          }

        }else{
          return;
        }
      }

      dispatch({type: 'RESTORE_USERNAME', token: (await getAccessToken()).res});
    }
    bootstrapAsync();
  }, []);

  const authContext = React.useMemo(() =>({
      signIn: async data => {
          dispatch({type: 'SIGN_IN', token: data, name: getUserName()});
      },
      signOut: () => { console.log("Sign Out Called"); deleteAccessToken(); deleteUserName(); deletePassword(); dispatch({type: 'SIGN_OUT'})},
      signUp: async data => {
        if(data)
            dispatch({type: 'SIGN_IN', token:data, name: getUserName()});
      }
  }), []);

  return (
    <AuthContext.Provider value={authContext}>
      <NavigationContainer>
        { console.log("Return - " + state.userToken)}
        {state.userToken === null ? (<RootStack/>) : (<TabNavigator/>)}
      </NavigationContainer>
    </AuthContext.Provider>
  )
}
