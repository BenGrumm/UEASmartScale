import {Formik} from 'formik'
import {Octicons, Ionicons} from '@expo/vector-icons';
import React, {useState} from "react";
import {Platform, TouchableOpacity, View, Text, TextInput} from 'react-native';
import {styles, Colors, StatusBarHeight} from '../components/styles';
import {useNavigation} from '@react-navigation/native';

import {login, registerForReturn, saveData} from '../Auth';

import {StatusBar} from "expo-status-bar";
import {AuthContext} from "../../App"

function Register() {
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [isLoading, setLoading] = useState(false);
    const [signUpErrors, setSignUpErrors] = useState({});
    const {signUp} = React.useContext(AuthContext);
    const nav = useNavigation();

    let handle_signup = async () =>{
        setLoading(true);

        let json = {"username":username, "password":password};

        let response = await registerForReturn(json);

        let jsonResponse = await response.json();

        setLoading(false);

        if(response.status === 201){
            saveData(json, jsonResponse);
            signUp(jsonResponse.token);
        }else if(response.status === 400){
            console.log("Errors ");
            console.log(jsonResponse);
            // Show error & stop spinner?
            setSignUpErrors(jsonResponse);
        }else{
            console.log("Response = " + response.status);
            console.log(jsonResponse);
            setText("Error Signing Up");
        }
    }

    return(
        <View
            style={{
              flex: 1,
              justifyContent: "center",
              alignItems: "center"
            }}>
            
            <StatusBar style="dark"/>

            <Text style={styles.titleText}>Smart Scale App</Text>
            <Text style={styles.subTitleText}>Register Account</Text>

                <View>
                    <TextInput
                       label="Username"
                       placeholder="Enter Username"
                       placeholderTextColor={Colors.fourth}
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
                        placeholderTextColor={Colors.fourth}
                        onChangeText={setPassword}
                        value={password}
                        secureTextEntry={true}
                        style={[styles.styledBox, styles.styledBoxText]}
                    />

                    <Text>
                        {signUpErrors ? signUpErrors.password : null}
                    </Text>
         
                    <Text>
                        {signUpErrors ? signUpErrors.non_field_errors : null}
                    </Text>
                                
                    <TouchableOpacity style={styles.stylePrimaryButton} onPress={handle_signup}>
                        {isLoading ? 
                            // <LoadingSpinner colour={Colors.second}/> 
                            <Text style={styles.primaryButtonText}>Loading</Text>
                            : 
                            <Text style={styles.primaryButtonText}>Register</Text>
                        }
                    </TouchableOpacity>

                    <View style={styles.viewStyle}>
                        <Text style={styles.extraTextStyle}>Already have an account? </Text>
                        <TouchableOpacity onPress={() => nav.goBack()}>
                            <Text style={styles.linkText}>Login</Text>
                        </TouchableOpacity>
                    </View>

                </View>
        </View>
    );
}

export default Register;

