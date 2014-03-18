//Code based on jstest.c, copyright of Vojtech Pavlik

#ifndef PVC_XBOX_H
#define PVC_XBOX_H

#include <arpa/inet.h>
#include <cmath>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <json.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "axbtnmap.c" 

#include "PVC_Camera.h"

#ifndef PI
#define PI 3.14159265  //Definition of PI for calculations
#endif

using namespace std;

char *axis_names[ABS_MAX + 1] = {
"X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder", 
"Wheel", "Gas", "Brake", "?", "?", "?", "?", "?",
"Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat3Y",
"?", "?", "?", "?", "?", "?", "?", 
};

char *button_names[KEY_MAX - BTN_MISC + 1] = {
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9", "?", "?", "?", "?", "?", "?",
"LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", "ExtraBtn", "ForwardBtn", "BackBtn", "TaskBtn", "?", "?", "?", "?", "?", "?", "?", "?",
"Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn", "BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6", "BtnDead",
"BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode", "BtnThumbL", "BtnThumbR", "?",
"?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", 
"WheelBtn", "Gear up",
};

enum BUTTON 
{
	A =	0, 
	B, 		// 1
	X, 		// 2
	Y, 		// 3
	LB, 	// 4
	RB, 	// 5
	START, 	// 6
	BACK,  	// 7
	HOME,  	// 8
	LJ,  	// 9
	RJ, 	// 10 
	DL,  	// 11
	DR,  	// 12
	DU,  	// 13
	DD 		// 14
};

enum AXIS
{
	LJH = 0,
	LJV, 	// 1
	LT, 	// 2
	RJH, 	// 3
	RJV, 	// 4
	RT, 	// 5
};

#define NAME_LENGTH 128

// Used to determine interactor to modify
enum interactor_t { PAN = 0, ROTATE, ROLL, ZOOM, AVIMODE, NONE };

// Control sending and receiving data
enum pThread_arg { Send = 0, Get = 1 };

///////////////////////////////////////////////////////////////////////////
/// Class:  PVC_Xbox
/// Brief:  This class contains the functions for reading data from an 
///			Xbox controller, translate it, and update ParaView with new
///			RenderView camera data.
///////////////////////////////////////////////////////////////////////////

class PVC_Xbox
{
public:
	
	// Constructor
	PVC_Xbox();
	
	// Deconstructor
	~PVC_Xbox();
	
	// Primary Functions //
	int  Run(int argc, char** argv);
	void HandleDisconnect();
	void Handle_Events();
	void Update_Status(struct js_event &js);
	void Print_Status(unsigned char axes, unsigned char buttons);
	void call_Pan(struct js_event &js);
	void call_Rotate(struct js_event &js);
	
	// Socket Functions //
	void SocketConnect();
	void Create_pThread(pThread_arg data);
	bool receiveReadyCommand(int socket);
	int  Receive(const int socket, void* data, int len);
	void* GetData();
	void* SendData();
	static void *SendDataHelper(void *arg) { return ((PVC_Xbox *)arg)->SendData(); }
	static void *GetDataHelper(void *arg) { return ((PVC_Xbox *)arg)->GetData(); }	
	
	// Interactors //
	void Pan(float dx, float dy);
//	void Dolly(...);
//	void Rotate(...);
//	void Roll(...);
	
	float ScaleModifier();
	
/// JSON Functions ///
	// Description:
	// Parses Json object for keyword and places values in the array.
	// CAUTION: Assumes that array passed in is large enough to hold all values.
	void json_get_array_values(json_object *jobj, char *key, float a[]);
	
	// Description:
	// Parses Json object and prints out every array element's type.
	// NOTE: Only used for debugging.
	void json_parse(json_object * jobj);
	
	// Description:
	// 
	void json_parse_array( json_object *jobj, char *key);
	void print_json_value(json_object *jobj);
	
private:
/// Controller Dependencies ///
	struct js_event js;
	bool turn_off;
	unsigned int deadzone;
	/*Buttons*/
    char* button;
	/*Axices*/
    int* axis;
    
/// Socket Dependents ///
	int s;
	sockaddr_in svr;
	pthread_t thread;
	bool sending;
	
/// Interaction Dependents ///
	PVC_Camera *camState;
	float cam_angle;
	float Center[3];
	bool haltActive;
	float MaxSize;
	
};

#endif // PVC_XBOX_H