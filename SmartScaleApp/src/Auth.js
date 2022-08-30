import * as SecureStore from 'expo-secure-store';

export const ADDRESS = "http://192.168.17.11:8000";

const USERNAME_ID = 'USERNAME';
const PASSWORD_ID = 'PASSWORD';
const ACCESS_TOKEN_ID = "ACCESS_TOKEN";
const REFRESH_TOKEN_ID = "REFRESH_TOKEN";

async function makeGetWithAuth(url, hasTried){
    let resp = await fetch(url, {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            Authorization: `JWT ${(await getAccessToken()).res}`
        },
    });
    
    if(!hasTried && resp.status === 401){
        await reauthenticate();
        return makeGetWithAuth(url, true);
    }else{
        return resp;
    }
}

async function makePost(url, json){
    console.log("Make Post to " + url);
    console.log(json);
    let resp = await fetch(url, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body : JSON.stringify(json)
    });

    return resp;
}

async function makeJSONPostWithAuth(url, json, hasTried){
    let resp = await fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            Authorization: `JWT ${(await getAccessToken()).res}`,
        },
        body: JSON.stringify(json),
    });
    
    if(!hasTried && resp.status === 401){
        await reauthenticate();
        return makeJSONPostWithAuth(url, json, true);
    }else{
        return resp;
    }
}

async function makeJSONDeleteWithAuth(url, json, hasTried){
    let resp = await fetch(url, {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
            Authorization: `JWT ${(await getAccessToken()).res}`,
        },
        body: JSON.stringify(json),
    });
    
    if(!hasTried && resp.status === 401){
        await reauthenticate();
        return makeJSONDeleteWithAuth(url, json, true);
    }else{
        return resp;
    }
}

async function login(json){
    let response = await makePost(url, {username: (await getUserName()).resp, password: (await getPassword()).res});
      
    if(response.status === 200){
        // Success store token continue
        let jsonResponse = await response.json();

        return jsonResponse.token;
    }else{
       return null;
    }

}

async function registerForReturn(json){
    return await makePost(ADDRESS + '/core/users/', json);
}

async function loginForReturn(json){
    return await makePost(ADDRESS + "/token-auth/", json);
}

async function reauthenticate(){

    let userName = await getUserName();
    let password = await getPassword();

    // Username or password does not exist so cant auth
    if(userName === null || password === null){
        return false;
    }

    let url = ADDRESS + "/token-auth/";

    let resp = await makePost(url, {username: userName.res, password: password.res});

    // If the login information is valid save
    if(resp.status === 200){
        await saveAccessToken(((await resp.json()).token));
        return true;
    }
    // Unsuccessful login
    else{
        return false;
    }
}

async function isAuthenticated(){
        let accessToken = await getAccessToken();

        // If no token try login with current username and password
        if(accessToken === null){
            return await reauthenticate();
        }

        // Try to login with current token
        let resp = await fetch(ADDRESS + "/core/current_user/", {
            method: 'GET',
            headers:
            {
                'Content-Type': 'application/json',
                Authorization: `JWT ${accessToken.res}`
            }
        });


        // Successful loging with existing token
        if(resp.status === 200){
            console.log("Return True");
            return true;
        }

        // Current token didnt work so try with stored username and password
        else{ 
            return await reauthenticate();
        }
       
}

async function get_scales(){
    let url = ADDRESS + "/core/scales/";
    return makeGetWithAuth(url, false);
}

async function get_beacons(){
    let url = ADDRESS + "/core/beacons/";
    return makeGetWithAuth(url, false);
}

async function update_scale(json){
    let url = ADDRESS + "/core/settings/";
    return makeJSONPostWithAuth(url, json, false);
}

async function update_beacon(json){
    console.log(json);
    let url = ADDRESS + "/core/beacons/";
    return makeJSONPostWithAuth(url, json, false);
}

async function delete_beacon(major, minor){
    let url = ADDRESS + "/core/beacons/";
    const obj = {
      "beacons":[
        {
            "major":major,
            "minor":minor
        }
      ]
    };
    return makeJSONDeleteWithAuth(url, obj, false);
}

async function get_location(scaleID){
    let address = ADDRESS + "/core/scale_location/" + scaleID + "/";
    return await makeGetWithAuth(address, false);
}

function saveData(inputJson, jsonResponse){
    saveAccessToken(jsonResponse.token);
    saveUserName(String(inputJson.username));
    savePassword(String(inputJson.password));
}

async function saveAccessToken(jwt){
    await SecureStore.setItemAsync(ACCESS_TOKEN_ID, jwt);
}

async function getAccessToken(){
    return await getItemFromStorage(ACCESS_TOKEN_ID);
}

async function saveRefreshToken(jwt){
    await SecureStore.setItemAsync(REFRESH_TOKEN_ID, jwt);
}

async function getRefreshToken(){
    return await getItemFromStorage(REFRESH_TOKEN_ID);
}

async function saveUserName(username){
    await SecureStore.setItemAsync(USERNAME_ID, username);
}

async function getUserName(){
    return getItemFromStorage(USERNAME_ID);
}

async function deleteUserName()
{
    SecureStore.deleteItemAsync(USERNAME_ID);
}

async function deleteAccessToken()
{
    SecureStore.deleteItemAsync(ACCESS_TOKEN_ID);
}
async function deleteRefreshToken()
{
    SecureStore.deleteItemAsync(REFRESH_TOKEN_ID);
}

async function savePassword(password){
    await SecureStore.setItemAsync(PASSWORD_ID, password);
}

async function deletePassword()
{
    SecureStore.deleteItemAsync(PASSWORD_ID);
}

async function getPassword(){
    return await getItemFromStorage(PASSWORD_ID);
}

async function getItemFromStorage(id){
    try{
        let result = await SecureStore.getItemAsync(id);

        // Check if item found
        if(result){
            return {res : result};
        }else{
            console.log("No item found with ID: " + id);
            return null;
        }
    }catch(error){
        console.log("Error");
        console.log(erorr);
    }
}

export{login, saveAccessToken, saveRefreshToken, getAccessToken, getRefreshToken, saveUserName, getUserName, savePassword, getPassword, isAuthenticated, 
    deleteUserName, registerForReturn, saveData, deleteAccessToken, deletePassword, reauthenticate, get_scales, update_scale, get_beacons, update_beacon, get_location, loginForReturn, delete_beacon};