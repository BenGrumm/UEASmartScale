import {Dimensions, StyleSheet, View, Text, Image, TextInput, TouchableOpacity} from 'react-native';
import Constants from 'expo-constants';

export const StatusBarHeight = Constants.statusBarHeight;
export const DIMENSIONS = Dimensions.get('window');

// color palette
export const Colors = {
    first: '#ffffff',
    second: '#3F76BF',
    third: '#001128',
    fourth: '#8F8F8F',
    fith: '#3F76BF'
}

// Some styles reused from following codebase
/***************************************************************************************
*    Title: WhatMattersToYou
*    Authors: Benjamin Grummitt, Rene Pupala, Dominic Seager and James Atkinson
*    Date: 2021
***************************************************************************************/


export const styles = StyleSheet.create({
    touchableStyled:{
        justifyContent: 'center',
        alignItems: 'center',
    },
    lineView:{
        height: 1,
        width: '100%',
        backgroundColor: Colors.fourth,
        marginVertical: 13,
    },
    container:{
        flex: 1,
        padding: 25,
        backgroundColor: Colors.first,
    },
    titleText:{
        fontSize: 25,
        textAlign: 'center',
        fontWeight: 'bold',
        color: Colors.fith,
        padding: 5,
    },
    titleIshText:{
        fontSize: 20,
        fontWeight: 'bold',
        color: Colors.fith,
    },
    subTitleText:{
        fontSize: 18,
        letterSpacing: 1,
        fontWeight: 'bold',
        color: Colors.third,
        padding: 5,
    },
    stylePrimaryButton:{
        padding: 15,
        backgroundColor: Colors.fith,
        justifyContent: 'center',
        alignItems: 'center',
        borderRadius: 10,
        marginVertical: 5,
        height: 50,
    },
    primaryButtonText:{
        color: Colors.first,
        fontSize: 16,
        fontWeight: 'bold',
    },
    viewStyle:{
        justifyContent: 'center',
        flexDirection: 'row',
        alignItems: 'center',
        padding: 10,
    },
    extraTextStyle:{
        justifyContent: 'center',
        alignItems: 'center',
        color: Colors.third,
        fontSize: 15,
    },
    linkText:{
        color: Colors.fith,
        fontSize: 15,
    },
    styledBox:{
        padding: 15,
        borderRadius: 10,
        height: 60,
        marginVertical: 3,
        borderWidth: 1,
        borderRadius: 5,
        borderColor: Colors.fourth,
        alignItems: 'center',
    },
    styledBoxText:{
        color: Colors.fourth,
        fontSize: 16,
        fontWeight: 'bold',
    },
    loginRightBox:{
        right: 15,
        top: 36,
        position: 'absolute',
        zIndex: 1,
    },
    listItemStyle:{
        backgroundColor: '#ffc600',
        fontSize: 24
    },
    listHeaderStyle:{
        borderColor:Colors.second,
        borderBottomWidth:1,
    },
    dropdown: {
      height: 50,
      borderColor: 'gray',
      borderWidth: 0.5,
      borderRadius: 8,
      paddingHorizontal: 8,
      width:"90%",
      alignSelf:"center",
    },
    icon: {
      marginRight: 5,
    },
    label: {
      position: 'absolute',
      backgroundColor: 'white',
      left: 22,
      top: 8,
      zIndex: 999,
      paddingHorizontal: 8,
      fontSize: 14,
    },
    placeholderStyle: {
      fontSize: 16,
    },
    selectedTextStyle: {
      fontSize: 16,
    },
    iconStyle: {
      width: 20,
      height: 20,
    },
    inputSearchStyle: {
      height: 40,
      fontSize: 16,
    },
    divider: {
        height: 1,
        alignItems: 'center',
        alignSelf: 'center',
        width: "90%",
        backgroundColor: "#607D8B",
    },
    fab: {
        position: 'absolute',
        margin: 16,
        right: 10,
        bottom: 10,
        backgroundColor: Colors.second,
    },
});
