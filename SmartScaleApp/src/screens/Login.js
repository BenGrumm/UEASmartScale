import React, {useState} from 'react';
import {StatusBar} from 'expo-status-bar';
import { Text, View, TouchableOpacity, TextInput } from 'react-native';
import {styles, Colors} from '../components/styles';
import {Formik} from 'formik';
import {AuthContext} from "../../App";
import {isAuthenticated, loginForReturn, saveData} from "../Auth";

// colors
const {brandCol, darkLight} = Colors;

const Login = ({navigation}) => {

  const [isLoading, setLoading] = useState(false);
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [hidePassword, setHidePassword] = useState(true);
  const [signUpErrors, setSignUpErrors] = useState({});
  const {signIn} = React.useContext(AuthContext);  

  let handle_login = async () => {
    try{

      let json = {"username":username, "password":password};

      let response = await loginForReturn(json);  
      let jsonResponse = await response.json();
  
      console.log(jsonResponse);
  
      setLoading(false);
  
      if(response.status === 200){
          saveData(json, jsonResponse);
          console.log(await isAuthenticated());
          signIn(jsonResponse.token);
      }else{
          setSignUpErrors(jsonResponse);
      }
    }catch(error){
      console.log(error);
      setLoading(false);
    }
  };

  return (
    <View
      style={{
        flex: 1,
        justifyContent: "center",
        alignItems: "center"
      }}>

      <StatusBar style="dark"/>

      <Text style={styles.titleText}>Smart Scale App</Text>
      <Text style={styles.subTitleText}>Account Login</Text>

       <View>
           <TextInput
               label="Username"
               placeholder="Enter Username"
               placeholderTextColor={darkLight}
               onChangeText={setUsername}
               value={username}
               style={[styles.styledBox, styles.styledBoxText]}
           />
   
           <Text>
               {signUpErrors ? signUpErrors.username : null}
           </Text>

            <TextInput
                label="Password"
                placeholder="Enter password"
                placeholderTextColor={darkLight}
                onChangeText={setPassword}
                value={password}
                secureTextEntry={true}
                style={[styles.styledBox, styles.styledBoxText]}
            />
    
            <Text>
                {signUpErrors ? signUpErrors.password : ""}
            </Text>
         
            <Text>
                {signUpErrors ? signUpErrors.non_field_errors : ""}
            </Text>
         
            <TouchableOpacity style={styles.stylePrimaryButton} onPress={handle_login}>
                {isLoading ? 
                    // <LoadingSpinner colour={Colors.second}/> : 
                    <Text style={styles.primaryButtonText}>Loading</Text>
                      :
                    <Text style={styles.primaryButtonText}>Login</Text>
                }
            </TouchableOpacity>
            <View style={styles.viewStyle}>
                <Text style={styles.extraTextStyle}>Haven't got an account? </Text>
                <TouchableOpacity onPress={() => {navigation.navigate("Register")}}>
                    <Text style={styles.linkText}>Sign up</Text>
                </TouchableOpacity>
            </View>
        </View>
    </View>
  )
}
export default Login;