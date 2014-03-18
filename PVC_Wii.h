// File: 		PVC_Wii.h
// Author:		Travis Bueter
// Description:	Class Definition of WiiMote interaction with ParaView

#ifndef PVC_WII_H
#define PVC_WII_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiicpp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <sstream>
#include <cmath>
#include <stdint.h>
#include <json.h>

#include "PVC_json.h"
#include "PVC_Interactors.h"
#include "PVC_Socket.h"

#ifndef PI
#define PI 3.14159265  //Definition of PI for calculations
#endif

using namespace std;

// Used to determine interactor to modify
enum interactor_t { PAN = 0, ROTATE, ROLL, ZOOM, AVIMODE, NONE };

// Used for Wiimote. Controls lighting LEDS to determine synced order
int LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};

class PVC_Wii
{
public:
	// Description:
	// Constructor for PVC_Wii object.
	PVC_Wii();
	
	// Description:
	// Deconstructor for PVC_Wii object.
	~PVC_Wii();

	// Description:
	// Prints Wiimote instructions to terminal. Need to find method for
	// printing to ParaView window.
	void PrintInstructions(CWiimote &wm);
	
	// Description:
	// Detects Wiimotes and connects them to read input. Need to force it
	// to only accept a single wiimote at a time.
	void FindWiimote();
	
	// Description:
	// Mainloop for reading Wiimotes. Could probably be optimized.
	void Run();
	
	// Description:
	// Event controller from Wiimote and calls appropriate function.  
	// Functions include:
	//		(Un)pausing sending camera data
	//		Pan
	//		Zoom/Dolly
	//		Rotate
	//		Roll
	//		Print instructions to screen
	//		Modify interactor rates
	void HandleEvent(CWiimote &wm);
	
	void GetData();
	
	// Description:
	// Unused. Would allow users to invert x- & y-axis of Interactors. 
	// Replaced by InteractionRatesMenu().
	void SetInvertion();
	
	// Description:
	// Unused. Would allow users to set deadzone of joystick. Value 
	// currently hard-set to 0.12.
	void SetDeadzone();
	
	// Description:
	// Unused. Would allow user to switch between Dolly() and Zoom()
	// functions. See functions for difference. Zoom() always used.
	void ToggleZoomingMode();
	
	// Description:
	// Prints list of Interactors and their rates. Shows selected rate via
	// '>' character. Also prints 'MAX' and 'MIN' when highest and lowest
	// values have been reached, respectively. Called when D-pad is used.
	void InteractionRatesMenu();
	
	// Description:
	// Calculates change in x and y coordinates based on angle and magnitude 
	// of joystick and passes values to Pan(dx,dy). Called when joystick is 
	// moved without Z or C buttons held. Should be merged with respected
	// interactor function.
	void Pan(CNunchuk& nc);
	
	// Description:
	// Unreachable in code. Not a true zoom function. Moves camera forward.
	void Dolly(CNunchuk& nc);
	
	// Description:
	// Calculates change in y coordinates based on angle and magnitude of 
	// joystick and passes value to Zoom(dx,dy). Called when joystick is
	// moved with only Z button held. Should be merged with respected 
	// interactor function.
	void Zoom(CNunchuk& nc);
	
	// Description:
	// Calculates change in x and y coordinates based on angle and magnitude
	// of joystick and passes values to Rotate(dx,dy). Called when joystick is
	// moved with only C button held. Should be merged with respected
	// interactor function.
	void Rotate(CNunchuk& nc);
	
	// Description:
	// Calculates change in x and y coordinates based on angle and magnitude
	// of joystick and passes values to Roll(dx). Called when joystick is
	// moved with both Z and C button held. Should be merged with respected
	// interactor function.
	void Roll(CNunchuk& nc);
	
	// Description:
	// Prints battery status
	void HandleStatus(CWiimote& wm);
	
	// Description:
	// Prints ID of Wiimote when disconnected
	void HandleDisconnect(CWiimote& wm);
	
	// Description: 
	// Prints ID of Wiimote being read from
	void HandleReadData(CWiimote& wm);
	
	// Description:
	// Does nothing.  Intended to print ID of Wiimote if Nunchuk is connected
	void HandleNunchukInserted(CWiimote& wm);
	
	// MISC
	float ScaleModifier();
	
private:
	/// Wii Dependents ///
	CWii wii;
	std::vector<CWiimote> wiimotes;
	std::vector<CWiimote>::iterator i;
	int reloadWiimotes;
	int numFound;
	int index;
	bool render;
	
	/// Socket ///
	PVC_Socket socket;
	
	/// Wii-side Interaction Dependents ///
	bool trigger;		//Janky way of preventing the instructions from printing twice upon activation.
	float mod;			//Modifier for all functions that allow for exponential Interaction.
	float magnitude;
	float angle;
	
	int d_invert;		//Variable that inverts the default Dolly controls when negative.
	int z_invert;		//Variable that inverts the default Zoom controls when negative.
	int p_xinvert;		//Variable that inverts the default Pan x-axis controls when negative.
	int p_yinvert;		//Variable that inverts the default Pan y-axis controls when negative.
	int r_xinvert;		//Variable that inverts the default Rotate x-axis controls when negative.
	int r_yinvert;		//Variable that inverts the default Rotate y-axis controls when negative.
	int s_invert;		//Variable that inverts the default Spin controls when negative.
	
	int toggle;			//Variable that toggles between Dolly/Zoom functions when positive/negative respectively.
	
					// For all Modifiers: Values must be positive. 
					// Value > 1: Magnifies rate; 1 > Value > 0: Miniturizes Rate.
	interactor_t selectedMod;
	float interactionMods[4]; //Contains modifiers for  Pan, Rotate, Roll, and Zoom, respectively.
							   //Dolly not included yet.
	float d_mod;		//Modifier that changes the rate of the Dolly function.
	float z_mod;		//Modifier that changes the rate of the Zoom function.
	float s_mod;		//Modifier that changes the rate of the Spin function.
	
	float deadzone;		//Modifies the dead zone of the joystick. Value must be between 0.05 & 1.1.
	bool temp;
	
	///Interaction Dependents ///
	PVC_Camera *camera;
	float cam_angle;
	//float Center[3];
	bool haltActive;
	float MaxSize;
	
};

#include "PVC_Wii.cxx"

#endif // PVCONTROLLER_H
