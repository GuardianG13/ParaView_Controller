// File: 		PVController.cpp
// Author:		Travis Bueter
// Description:	Class Definition of WiiMote interaction with ParaView

PVC_Wii::PVC_Wii()
{
	reloadWiimotes = 0;
	trigger = false;
	mod = 90*exp(-10);
	d_invert = 1;
	z_invert = 1;
	p_xinvert = 1;
	p_yinvert = 1;
	r_xinvert = 1;
	r_yinvert = 1;
	s_invert = 1;
	toggle = 0;
	d_mod = 1;
	s_mod = 1;
	interactionMods[0] = 1;
	interactionMods[1] = 1;
	interactionMods[2] = 1;
	interactionMods[3] = 1;
	selectedMod = NONE;
	deadzone = 0.12;

	haltActive = true;

	temp = true;
	
	MaxSize = 1;
	
	this->cam_angle = 0.0;
	
	camera = new PVC_Camera;
}

PVC_Wii::~PVC_Wii()
{
	camera->Delete();
}

void PVC_Wii::FindWiimote()
{
	cout << "Searching for wiimotes... Turn them on!" << endl;

	//Find the wiimote
	numFound = wii.Find(5);

	// Search for up to five seconds;

	cout << "Found " << numFound << " wiimotes" << endl;
	cout << "Connecting to wiimotes..." << endl;

	// Connect to the wiimote
	wiimotes = wii.Connect();

	cout << "Connected to " << (unsigned int)wiimotes.size() << " wiimote." << endl;

	
	// Setup the wiimotes
	for(index = 0, i = wiimotes.begin(); i != wiimotes.end(); ++i, ++index)
	{
		// Use a reference to make working with the iterator handy.
		CWiimote & wiimote = *i;

		//Set Leds
		wiimote.SetLEDs(LED_MAP[index]);

		//Rumble for 0.2 seconds as a connection ack
		wiimote.SetRumbleMode(CWiimote::ON);
		usleep(200000);
		wiimote.SetRumbleMode(CWiimote::OFF); 
	}
}

void PVC_Wii::GetData()
{
	string data;
	float cam[10];
	
	socket.ReceiveData(data);
		
	json_object * jobj = json_tokener_parse(data.c_str());
	json_get_array_values(jobj, "Renderers", cam);
	
	cam_angle = cam[0];
	camera->SetFocalPoint(cam[1], cam[2], cam[3]);
	camera->SetViewUp(cam[4], cam[5], cam[6]);
	camera->SetPosition(cam[7], cam[8], cam[9]);
	camera->SetCenter();
}

void PVC_Wii::Run()
{
	socket.SocketConnect(40000);
	
	GetData();
	
	do
	{
		if(reloadWiimotes)
		{
			wiimotes = wii.GetWiimotes();
			reloadWiimotes = 0;
			trigger = false;
		}
		if(wii.Poll())
		{
			for(i = wiimotes.begin(); i != wiimotes.end(); ++i)
			{
				// Use a reference to make working with the iterator handy.
				CWiimote & wiimote = *i;

				switch(wiimote.GetEvent())
				{
		
					case CWiimote::EVENT_EVENT:
						 HandleEvent(wiimote);
						 break;
		
					case CWiimote::EVENT_STATUS:
						 if(!trigger)
						 {
							 PrintInstructions(wiimote);
							 trigger = true;
						 }
						 
						 break;
		
					case CWiimote::EVENT_DISCONNECT:
					case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
						 HandleDisconnect(wiimote);
						 reloadWiimotes = 1;
						 break;
		
					case CWiimote::EVENT_READ_DATA:
						 HandleReadData(wiimote);
						 break;
		
					case CWiimote::EVENT_NUNCHUK_INSERTED:
						 HandleNunchukInserted(wiimote);
						 reloadWiimotes = 1;
						 break;
		
					case CWiimote::EVENT_NUNCHUK_REMOVED:
					case CWiimote::EVENT_MOTION_PLUS_REMOVED:
						 cout << "An expansion was removed." << endl;
						 HandleStatus(wiimote);
						 reloadWiimotes = 1;
						 break;
		
					default:
						break;
				}
			}
		}
	}while(wiimotes.size());
}

void PVC_Wii::Pan(CNunchuk &nc)
{
	nc.Joystick.GetPosition(angle,magnitude);
	if(magnitude >= deadzone)
	{
		float x = sin(angle*PI/180);
		float y = cos(angle*PI/180);
		magnitude += .1 - deadzone;
		float scale = ScaleModifier();
		float multiplier = interactionMods[0]*mod*exp(magnitude)*pow(magnitude+.56,6.5)*(magnitude/10*pow(scale,.66));
		float dx = p_xinvert*multiplier*x;
		float dy = -p_yinvert*multiplier*y;
		//cout << "X: " << dx << " | Y: " << dy << endl;
		PVC_Interactors::Pan(camera,dx,dy);
		socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	}
}

void PVC_Wii::Dolly(CNunchuk &nc)
{
	nc.Joystick.GetPosition(angle, magnitude);
	if(magnitude >= deadzone)
	{
		float y = cos(angle*PI/180);
		magnitude += .1 - deadzone;
		float multiplier = .1*mod*exp(magnitude)*pow(magnitude+.5,5);
		float dy = d_mod*d_invert*80*multiplier*y;
		PVC_Interactors::Dolly(camera,dy);
		socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	}
}

void PVC_Wii::Zoom(CNunchuk &nc)
{
	nc.Joystick.GetPosition(angle, magnitude);
	if(magnitude >= deadzone)
	{
		float y = cos(angle*PI/180);
		magnitude += .1 - deadzone;
		float scale = ScaleModifier();
		float multiplier = .1*mod*exp(magnitude)*pow(magnitude+.5,5)*(pow(scale,.9)/100);
		float dy = interactionMods[3]*z_invert*multiplier*y;
		PVC_Interactors::Zoom(camera,dy);
		socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	}
}

void PVC_Wii::Rotate(CNunchuk &nc)
{
	nc.Joystick.GetPosition(angle, magnitude);
	if(magnitude >= deadzone)
	{
		float x = sin(angle*PI/180);
		float y = cos(angle*PI/180);
		magnitude += .1 - deadzone;
		float multiplier = interactionMods[1]*mod*exp(magnitude)*pow(magnitude+.5,5);
		float dx = -r_xinvert*multiplier*x;
		float dy = -r_yinvert*multiplier*y;
		PVC_Interactors::Rotate(camera,dx,dy);
		socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	}
}

void PVC_Wii::Roll(CNunchuk &nc)
{
	nc.Joystick.GetPosition(angle, magnitude);
	if(magnitude >= deadzone)
	{
		float x = sin(angle*PI/180);
		magnitude += .1 - deadzone;
		float multiplier = interactionMods[2]*mod*.5*exp(magnitude)*pow(magnitude+.5,5);
		float dx = s_invert*multiplier*x;
		PVC_Interactors::Roll(camera,dx);
		socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
	}
}

void PVC_Wii::HandleEvent(CWiimote &wm)
{
    int exType = wm.ExpansionDevice.GetType();
    if(haltActive)
    {
        if(wm.Buttons.isJustPressed(CButtons::BUTTON_B))
    	{
        	
        	haltActive = false;
        	cout << "Unpaused" << endl;
        	return;
    	}
    }
    else
    {
		if(exType == wm.ExpansionDevice.TYPE_NUNCHUK)
		{
			float angle, magnitude;
			CNunchuk &nc = wm.ExpansionDevice.Nunchuk;
			if(!nc.Buttons.isHeld(CNunchukButtons::BUTTON_Z) && !nc.Buttons.isHeld(CNunchukButtons::BUTTON_C))
			{
				Pan(nc);
			}
			if(nc.Buttons.isHeld(CNunchukButtons::BUTTON_Z) && !nc.Buttons.isHeld(CNunchukButtons::BUTTON_C))
			{
				if(toggle)
					Dolly(nc);
				else
					Zoom(nc);
				return;
			}
			if(nc.Buttons.isHeld(CNunchukButtons::BUTTON_C) && !nc.Buttons.isHeld(CNunchukButtons::BUTTON_Z))
			{
				Rotate(nc);
				return;
			}
			if(nc.Buttons.isHeld(CNunchukButtons::BUTTON_C) && nc.Buttons.isHeld(CNunchukButtons::BUTTON_Z))
			{
				Roll(nc);
				return;
			}
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_A))
		{
			PrintInstructions(wm);
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_MINUS))
		{
			if(temp)
			{
				mod = mod/4;
				temp = false;
			}
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_PLUS))
		{
			if(!temp)
			{
				mod = mod*4;
				temp = true;
			}
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_B))
		{
			//pthread_join(thread, NULL); //TODO: Figure out why I'm using this 
										  //	  and what to do about it
			socket.join_thread();
			haltActive = true;
			GetData();
			cout << "Paused" << endl;
			return;
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_DOWN)) 
		{
			switch(selectedMod)
			{
			case PAN:
				selectedMod = ROTATE;
				break;
			case ROTATE:
				selectedMod = ROLL;
				break;
			case ROLL:
				selectedMod = ZOOM;
				break;
			case ZOOM:
				selectedMod = AVIMODE;
				break;
			case AVIMODE:
				selectedMod = NONE;
				break;
			case NONE:
				selectedMod = PAN;
				break;
			default:
				cout << "ERROR: Default case hit, Down" << endl;
				exit(1);
				break;
			}
			InteractionRatesMenu(); // SetInvertion(); Needs alternate button suggest (+)/(-)
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_RIGHT) && (selectedMod != NONE))
		{
			unsigned int x = selectedMod;
			if(x == 3)
			{
				r_yinvert *= -1;
				p_yinvert *= -1;
			}
			else if(interactionMods[x] < 1)
			{
				interactionMods[x] += 0.1;
			}
			else if(interactionMods[x] < 10)
			{
				interactionMods[x] += 1.0;
			}
			InteractionRatesMenu();
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_LEFT) && (selectedMod != NONE)) 
		{
			unsigned int x = selectedMod;
			if(x == 4)
			{
				r_yinvert *= -1;
				p_yinvert *= -1;
			}
			else if(interactionMods[x] < 0.2) //Weird bug causes interactionMods[x] = 0.2 -= 0.1 => 0.099999... 
										 //this is a dirty fix
			{
				interactionMods[x] = 0.1;
			}
			else if(interactionMods[x] <= 1)
			{
				interactionMods[x] -= 0.1;
			}
			else
			{
				interactionMods[x] -= 1.0;
			}
			InteractionRatesMenu();
		}
		if(wm.Buttons.isJustPressed(CButtons::BUTTON_UP))
		{
			switch(selectedMod)
			{
			case PAN:
				selectedMod = NONE;
				break;
			case ROTATE:
				selectedMod = PAN;
				break;
			case ROLL:
				selectedMod = ROTATE;
				break;
			case ZOOM:
				selectedMod = ROLL;
				break;
			case AVIMODE:
				selectedMod = ZOOM;
				break;
			case NONE:
				selectedMod = AVIMODE;
				break;
			default:
				cout << "ERROR: Default case hit, Up" << endl;
				exit(1);
				break;
			}
			InteractionRatesMenu();
		}
    }
}

void PVC_Wii::SetInvertion()
{
	char function;
	printf("\n[Pan = p : Dolly = d : Zoom = z : Rotate = r : Spin = s] Enter 'c' to cancel at any time.\n");
	cout << "Which Function to Invert? ";
	cin >> function;
	switch(function)
	{
		case 'P':
		case 'p':
			cout << "\n Pan: X-coord [x], Y-coord [y], or both [b]? ";
			cin >> function;
			switch(function)
			{
				case 'X':
				case 'x':
					p_xinvert *= -1;
					printf("\nPan X-coord inverted.\n");
					break;
				case 'Y':
				case 'y':
					p_yinvert *= -1;
					printf("\nPan Y-coord inverted.\n");
					break;
				case 'B':
				case 'b':
					p_xinvert *= -1;
					p_yinvert *= -1;
					printf("\nPan X-coord and Y-coord inverted;\n");
					break;
				case 'C':
				case 'c':
					printf("\nCancelled.\n");
					break;
				default:
					printf("\nERROR: Invalid input.\n");
					break;
			}
			break;
		case 'D':
		case 'd':
			d_invert *= -1;
			printf("\nDolly Inverted.\n");
			break;
		case 'Z':
		case 'z':
			z_invert *= -1;
			printf("\nZoom Inverted.\n");
			break;
		case 'R':
		case 'r':
			cout << "\nRotate: X-coord [x], Y-coord [y], or both [b]? ";
			cin >> function;
			switch(function)
			{
				case 'X':
				case 'x':
					r_xinvert *= -1;
					printf("\nRotate X-coord inverted.\n");
					break;
				case 'Y':
				case 'y':
					r_yinvert *= -1;
					printf("\nRotate Y-coord inverted.\n");
					break;
				case 'B':
				case 'b':
					r_xinvert *= -1;
					r_yinvert *= -1;
					printf("\nRotate X-coord and Y-coord inverted;\n");
					break;
				case 'C':
				case 'c':
					printf("\nCancelled.\n");
					break;
				default:
					printf("\nERROR: Invalid input.\n");
					break;
			}
			break;
		case 'S':
		case 's':
			s_invert *= -1;
			printf("\nSpin inverted.\n");
			break;
		case 'C':
		case 'c':
			printf("\nCancelled.\n");
			break;
		default:
			printf("\nERROR: Invalid input.\n");
			break;
	}
}

void PVC_Wii::SetDeadzone()
{
	float value;
	printf("\nDefault Deadzone is '0.12'. Greater value equals greater deadzone and vice versa.\n");
	cout << "Set deadzone to [0.05 <-> 1.1]: ";
	cin >> value;
	if(!(value >= 0.05 && value <= 1.1))
	{
		printf("\nInvalid input.\n");
		return;
	}
	deadzone = value; 
}

void PVC_Wii::ToggleZoomingMode()
{
	toggle *= -1;
	if(toggle > 0)
	{
		printf("\nDolly mode active.\n");
	}
	else
	{
		printf("\nZoom mode active.\n");
	}
}

void PVC_Wii::InteractionRatesMenu()
{
	switch(selectedMod)
	{
		case PAN:
			std::system ( "clear" );
			cout << ">Pan rate value:    " << interactionMods[0]; 
			
			if(interactionMods[0] == 10) cout << " MAX" << endl;
			else if(interactionMods[0] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Rotate rate value: " << interactionMods[1];
			
			if(interactionMods[1] == 10) cout << " MAX" << endl;
			else if(interactionMods[1] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Roll rate value:   " << interactionMods[2];
						
			if(interactionMods[2] == 10) cout << " MAX" << endl;
			else if(interactionMods[2] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Zoom rate value:   " << interactionMods[3];
			
			if(interactionMods[3] == 10) cout << " MAX" << endl;
			else if(interactionMods[3] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Aviation Mode:     "; 
			
			if(r_yinvert == -1 && p_yinvert == -1) cout << "TRUE" << endl; else cout << "FALSE" << endl;
				
			cout << " Close" << endl;
			break;
		case ROTATE:
			std::system ( "clear" );

			cout << " Pan rate value:    " << interactionMods[0]; 
			
			if(interactionMods[0] == 10) cout << " MAX" << endl;
			else if(interactionMods[0] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << ">Rotate rate value: " << interactionMods[1];
			
			if(interactionMods[1] == 10) cout << " MAX" << endl;
			else if(interactionMods[1] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Roll rate value:   " << interactionMods[2];
						
			if(interactionMods[2] == 10) cout << " MAX" << endl;
			else if(interactionMods[2] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Zoom rate value:   " << interactionMods[3];
			
			if(interactionMods[3] == 10) cout << " MAX" << endl;
			else if(interactionMods[3] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Aviation Mode:     "; 
			
			if(r_yinvert == -1 && p_yinvert == -1) cout << "TRUE" << endl; else cout << "FALSE" << endl;
				
			cout << " Close" << endl;
			break;
		case ROLL:
			std::system ( "clear" );
			cout << " Pan rate value:    " << interactionMods[0]; 
			
			if(interactionMods[0] == 10) cout << " MAX" << endl;
			else if(interactionMods[0] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Rotate rate value: " << interactionMods[1];
			
			if(interactionMods[1] == 10) cout << " MAX" << endl;
			else if(interactionMods[1] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << ">Roll rate value:   " << interactionMods[2];
						
			if(interactionMods[2] == 10) cout << " MAX" << endl;
			else if(interactionMods[2] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Zoom rate value:   " << interactionMods[3];
			
			if(interactionMods[3] == 10) cout << " MAX" << endl;
			else if(interactionMods[3] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Aviation Mode:     "; 
			
			if(r_yinvert == -1 && p_yinvert == -1) cout << "TRUE" << endl; else cout << "FALSE" << endl;
				
			cout << " Close" << endl;
			break;
		case ZOOM:
			std::system ( "clear" );
			cout << " Pan rate value:    " << interactionMods[0]; 
			
			if(interactionMods[0] == 10) cout << " MAX" << endl;
			else if(interactionMods[0] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Rotate rate value: " << interactionMods[1];
			
			if(interactionMods[1] == 10) cout << " MAX" << endl;
			else if(interactionMods[1] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Roll rate value:   " << interactionMods[2];
						
			if(interactionMods[2] == 10) cout << " MAX" << endl;
			else if(interactionMods[2] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << ">Zoom rate value:   " << interactionMods[3];
			
			if(interactionMods[3] == 10) cout << " MAX" << endl;
			else if(interactionMods[3] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Aviation Mode:     "; 
			
			if(r_yinvert == -1 && p_yinvert == -1) cout << "TRUE" << endl; else cout << "FALSE" << endl;
				
			cout << " Close" << endl;
			break;
		case AVIMODE:
			std::system ( "clear" );
			cout << " Pan rate value:    " << interactionMods[0]; 
			
			if(interactionMods[0] == 10) cout << " MAX" << endl;
			else if(interactionMods[0] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Rotate rate value: " << interactionMods[1];
			
			if(interactionMods[1] == 10) cout << " MAX" << endl;
			else if(interactionMods[1] < 0.15) cout << " MIN" << endl;
			else cout << endl;

			cout << " Roll rate value:   " << interactionMods[2];
						
			if(interactionMods[2] == 10) cout << " MAX" << endl;
			else if(interactionMods[2] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << " Zoom rate value:   " << interactionMods[3];
			
			if(interactionMods[3] == 10) cout << " MAX" << endl;
			else if(interactionMods[3] < 0.15) cout << " MIN" << endl;
			else cout << endl;
			
			cout << ">Aviation Mode:     "; 
			
			if(r_yinvert == -1 && p_yinvert == -1) cout << "TRUE" << endl; else cout << "FALSE" << endl;
				
			cout << " Close" << endl;
			break;
		case NONE:
			std::system ( "clear" );
			break;
		default:
			cout << "ERROR: Default case hit, InteractionRatesMenu" << endl;
			exit(1);
			break;
	}
}

void PVC_Wii::HandleStatus(CWiimote &wm)
{
	if(wm.GetBatteryLevel()<.2)
	{
		printf("CAUTION: Battery Level at %f%%\n\n", wm.GetBatteryLevel()*100);
	}
}

void PVC_Wii::HandleDisconnect(CWiimote &wm)
{
    printf("\n");
    printf("--- DISCONNECTED [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void PVC_Wii::HandleReadData(CWiimote &wm)
{
    printf("\n");
    printf("--- DATA READ [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void PVC_Wii::HandleNunchukInserted(CWiimote &wm)
{
    //printf("Nunchuk inserted on controller %i.\n", wm.GetID());
}

void PVC_Wii::PrintInstructions(CWiimote &wm)
{
	printf("-----------------------------------------------------------------\n");
	printf("Battery Level: %f %%\n\n", wm.GetBatteryLevel());
	printf("NUNCHUK COMMANDS:\n");
	printf("	Joystick w/ no buttons pressed  -  Pan\n");
	printf("	Joystick w/ 'C' button pressed  -  Rotate\n");
	printf("	Joystick w/ 'Z' button pressed  -  Zoom\n");
	printf("	Joystick w/ 'C' & 'Z'  pressed  -  Roll\n");
	printf("\nWIIMOTE COMMANDS:\n");
	printf("	D-pad Up     -  Go up Interactor-Modifier menu\n");
	printf("	D-pad Down   -  Go down Interactor-Modifier menu\n");
	printf("	D-pad Left   -  Increase selected modifier value\n");
	printf("	D-pad Right  -  Decrease selected modifier value\n");
	printf("	A Button     -  Re-print Instructions\n");
	printf("-----------------------------------------------------------------\n\n");
}

//void PVC_Wii::Dolly(float dy)
//{
//	if(camera == NULL)
//		return;
//	
//	float pos[3], fp[3], vu[3], Ld[3];
//	camera->GetPosition(pos);
//	camera->GetFocalPoint(fp);
//	camera->GetViewUp(vu);
//	
//	Ld[0] = fp[0]-pos[0];
//	Ld[1] = fp[1]-pos[1];
//	Ld[2] = fp[2]-pos[2];
//	
//	pos[0] = pos[0]-Ld[0]*dy*0.2;
//	pos[1] = pos[1]-Ld[1]*dy*0.2;
//	pos[2] = pos[2]-Ld[2]*dy*0.2;
//	
//	socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
//	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
//}
//
//void PVC_Wii::Zoom(float dy)
//{
//	if(camera == NULL)
//		return;
//	
//	float pos[3], fp[3], *norm, k, tmp;
//	camera->GetPosition(pos);
//	camera->GetFocalPoint(fp);
//	norm = camera->GetDirectionOfProjection();
//	k = dy * 1000;
//	
//	tmp = k * norm[0];
//	pos[0] += tmp;
//	fp[0] += tmp;
//  
//	tmp = k*norm[1];
//	pos[1] += tmp;
//	fp[1] += tmp;
//  
//	tmp = k * norm[2];
//	pos[2] += tmp;
//	fp[2] += tmp;
//	
//	camera->SetFocalPoint(fp[0], fp[1], fp[2]);
//	camera->SetPosition(pos[0], pos[1], pos[2]);
//	
//	socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
//	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
//}
//
//void PVC_Wii::Pan(float dx, float dy)
//{
//	if(camera == NULL)
//		return;
//	
//	float pos[3], fp[3]; 
//	camera->GetPosition(pos);
//	camera->GetFocalPoint(fp);
//	
//	camera->OrthogonalizeViewUp();
//	float *up = camera->GetViewUp();
//	float *vpn = camera->GetViewPlaneNormal();
//	float right[3];
//	float scale, tmp;
//	camera->GetViewUp(up);
//	camera->GetViewPlaneNormal(vpn);
//	PVC_Math::Cross(vpn, up, right);
//	
//	scale = camera->GetParallelScale();
//	dx *= scale * 2.0;
//	dy *= scale * 2.0;
//	
//	tmp = (right[0]*dx + up[0]*dy);
//	pos[0] += tmp;
//	fp[0] += tmp; 
//	tmp = (right[1]*dx + up[1]*dy); 
//	pos[1] += tmp;
//	fp[1] += tmp; 
//	tmp = (right[2]*dx + up[2]*dy); 
//	pos[2] += tmp;
//	fp[2] += tmp; 
//	camera->SetPosition(pos[0], pos[1], pos[2]);
//	camera->SetFocalPoint(fp[0], fp[1], fp[2]);
//	
//	socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
//	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
//}
//
//void PVC_Wii::Roll(float dx)
//{
//
//	if(camera == NULL)
//			return;
//		
//	PVC_Transform *transform = new PVC_Transform();
//	
//	float axis[3];
//	
//	float* fp = camera->GetFocalPoint();
//	float* pos = camera->GetPosition();
//	axis[0] = fp[0] - pos[0];
//	axis[1] = fp[1] - pos[1];
//	axis[2] = fp[2] - pos[2];
//
//	// translate to center
//	transform->Identity();
//	transform->Translate(this->Center[0], this->Center[1], this->Center[2]);
//
//	transform->RotateWXYZ(-360.0*dx, axis[0], axis[1], axis[2]);
//
//	// translate back
//	transform->Translate(-this->Center[0], -this->Center[1], -this->Center[2]);
//	camera->ApplyTransform(transform);
//	camera->OrthogonalizeViewUp();
//	
//	socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
//	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
//	
//	transform->Delete();
//}
//
//void PVC_Wii::Rotate(float dx, float dy)
//{	
//	
//	if(camera == NULL)
//		return;
//	
//	PVC_Transform *transform = new PVC_Transform();
//	float scale = PVC_Math::Norm(camera->GetPosition());
//	if(scale<=0.0)
//	{
//		scale = PVC_Math::Norm(camera->GetFocalPoint());
//		if(scale<=0.0)
//		{
//			scale = 1.0;
//		}
//	}
//	
//	float* temp = camera->GetFocalPoint();
//	camera->SetFocalPoint(temp[0]/scale, temp[1]/scale, temp[2]/scale);
//	temp = camera->GetPosition();
//	camera->SetPosition(temp[0]/scale, temp[1]/scale, temp[2]/scale);
//
//	float v2[3];
//	// translate to center
//	transform->Identity();
//	transform->Translate(this->Center[0]/scale, this->Center[1]/scale, this->Center[2]/scale);
//
//	//azimuth
//	camera->OrthogonalizeViewUp();
//	float *viewUp = camera->GetViewUp();
//	transform->RotateWXYZ(360.0*dx, viewUp[0], viewUp[1], viewUp[2]);
//
//	//elevation
//	PVC_Math::Cross(camera->GetDirectionOfProjection(), viewUp, v2);
//	transform->RotateWXYZ(-360.0*dy, v2[0], v2[1], v2[2]);
//
//	// translate back
//	transform->Translate(-this->Center[0]/scale, -this->Center[1]/scale, -this->Center[2]/scale);
//	camera->ApplyTransform(transform);
//	camera->OrthogonalizeViewUp();
//
//	temp = camera->GetFocalPoint();
//	camera->SetFocalPoint(temp[0]*scale, temp[1]*scale, temp[2]*scale);
//	temp = camera->GetPosition();
//	camera->SetPosition(temp[0]*scale, temp[1]*scale, temp[2]*scale);
//	
//	socket.pThread_Send((void *)&camera->camState, sizeof(CameraState));
//	//Create_pThread(Send, (void *)&camera->camState, sizeof(CameraState));
//	
//	transform->Delete();
//}

float PVC_Wii::ScaleModifier()
{
	float distance = camera->GetDistance();
	if(distance < MaxSize/2)
		return 0.5;
	else if(distance > 100000)
		return 100000;
	else
		return distance;
}



