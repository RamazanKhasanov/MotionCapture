using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class Receiver : MonoBehaviour {
	public string getcom;
	SerialPort serial1;
	void Start () {
		serial1=new SerialPort("COM8", 9600);
		if (!serial1.IsOpen)
			serial1.Open ();
	}
	
	// Update is called once per frame
	void Update () {
		getcom = serial1.ReadLine ();
		Debug.Log ("get from port= ");
		Debug.Log (getcom);
	}
}
