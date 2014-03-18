#include "PVC_Xbox.h"

PVC_Xbox::PVC_Xbox()
{
	axis = NULL;
	button = NULL;
	turn_off = false;
	deadzone = 6000;
	sending = false;
	haltActive = true;
	MaxSize = 1;
	this->Center[1] = 0.0;
	this->Center[2] = 0.0;
	this->Center[3] = 0.0;
	
	this->cam_angle = 0.0;
	
	camState = new PVC_Camera;
}


PVC_Xbox::~PVC_Xbox()
{
	free(axis);
	free(button);
}


int PVC_Xbox::Run(int argc, char **argv)
{
	int fd, i, it;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	uint16_t btnmap[BTNMAP_SIZE];
	uint8_t axmap[AXMAP_SIZE];
	int btnmapok = 1;
	
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("jstest");
		return 1;
	}
	
	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);
	
	getaxmap(fd, axmap);
	getbtnmap(fd, btnmap);
	
	printf("Driver version is %d.%d.%d.\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);

	/* Determine whether the button map is usable. */
	for (i = 0; btnmapok && i < buttons; i++) {
		if (btnmap[i] < BTN_MISC || btnmap[i] > KEY_MAX) {
			btnmapok = 0;
			break;
		}
	}
	if (!btnmapok) {
		/* btnmap out of range for names. Don't print any. */
		puts("Xbox Controller mode is not fully compatible with your kernel. Unable to retrieve button map!");
		printf("Xbox Controller (%s) has %d axes ", name, axes);
		printf("and %d buttons.\n", buttons);
	} else {
		printf("Xbox Controller (%s) has %d axes (", name, axes);
		for (i = 0; i < axes; i++)
			printf("%s%s", i > 0 ? ", " : "", axis_names[axmap[i]]);
		puts(")");

		printf("and %d buttons (", buttons);
		for (i = 0; i < buttons; i++) {
			printf("%s%s", i > 0 ? ", " : "", button_names[btnmap[i] - BTN_MISC]);
		}
		puts(").");
	}
	
	axis = (int*)malloc(axes * sizeof(*axis));
	button = (char*)malloc(buttons * sizeof(*button));
	it = 0;
	if (!strcmp("--nonblock", argv[1])) {

		struct js_event js;

		fcntl(fd, F_SETFL, O_NONBLOCK);

		while (!turn_off) {
			
			while(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {
				switch(js.type & ~JS_EVENT_INIT) {
				case JS_EVENT_BUTTON:
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
				}
			}
			printf("\r");
			
			Handle_Events();

			Print_Status(axes,buttons);
			
			if (errno != EAGAIN) {
				perror("\njstest: error reading");
				return 1;
			}

			usleep(10000);
		}
	}
	
	return 0;
}


void PVC_Xbox::HandleDisconnect()
{
	
}


void PVC_Xbox::Handle_Events()
{
	double LJ_x = axis[0];
	double LJ_y = axis[1];
	double RJ_x = axis[3];
	double RJ_y = axis[4];
	if(abs(LJ_x) > 6000 || abs(LJ_y) > 6000)
	{
		
	}
	if(abs(RJ_x) > 6000 || abs(RJ_y) > 6000)
	{
		printf("Rotate ");
	}

}


void PVC_Xbox::Print_Status(unsigned char axes, unsigned char buttons)
{
	int i;

	if (axes) {
		printf("Axes: ");
		for (i = 0; i < axes; i++)
			printf("%2d:%6d ", i, axis[i]);
	}

	if (buttons) {
		printf("Buttons: ");
		for (i = 0; i < buttons; i++)
			printf("%2d:%s ", i, button[i] ? "on " : "off");
	}
	fflush(stdout);
}


//void call_Pan(struct js_event &js)
//{
//	double x = axis[LJH];
//	double y = axis[LJV];
//	if(x > deadzone && y > deadzone)
//	{
//		
//	}	
//}

void PVC_Xbox::SocketConnect()
{
	int ready = 0;
	int command = 2;
	int size;
	
	svr.sin_family = AF_INET;
	svr.sin_port = htons(40000);
	inet_aton("127.0.1.1", &svr.sin_addr);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == s)
	{
		printf("Socket creation failed. Now exiting. \n");
		exit(1);
	}
	else
		printf("Socket created. \n");

	if(connect(s, (const sockaddr*)&svr, sizeof(sockaddr_in)) == -1)
	{
		printf("Connection failed. Exiting. \n");
		exit(1);
	}
	else
		printf("Connection successful. \n");
	
	Create_pThread(Get);
}


void PVC_Xbox::Create_pThread(pThread_arg data)
{
	if(data == Send && !sending)
	{
		sending = true;
		int rc = pthread_create(&thread, NULL, &PVC_Xbox::SendDataHelper, this);
		if (rc)
		{
			cout << "Error: unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	else if(data == Get)
	{
		pthread_join(thread, NULL);
		int rc = pthread_create(&thread, NULL, &PVC_Xbox::GetDataHelper, this);
		if (rc)
		{
			cout << "Error: unable to create thread," << rc << endl;
			exit(-1);
		}
	}
}


bool PVC_Xbox::receiveReadyCommand(int socket)
{
	int command = 0;
	Receive(socket, &command, 4);
	if(command == 1)
		return true;
	else
		return false;
}


int PVC_Xbox::Receive(const int socket, void* data, int len)
{
	char* buffer = (char*)data;
	int total = 0;
	do
	{
		int nRecvd = recv(socket, buffer+total, len-total, 0);
		if(nRecvd == 0)
		{
			return 0;
		}
		else if(nRecvd == -1)
		{
			printf("Immediate receive Failure\n");
			return -1;
		}
		total += nRecvd;
	}while(total < len);
	return total;
}


void* PVC_Xbox::GetData(CameraState &camera)
{
	int ready = 0;
	int command = 2;
	int size;
	float cam[10];

	while(ready != 1)
	{
		if((size = Receive(this->s, &ready, sizeof(ready))) != 4)
			printf("Size not 4 bytes. Continue anyways.\n");
	}

	if(-1 == send(this->s, &command, sizeof(command), 0))
	{
		printf("Command Send Fail. \n");
		exit(1);
	}

	unsigned long long length;
	if(!Receive(this->s, &length, sizeof(unsigned long long)))
	{
		printf("Receive Failure\n");
	}

	char metadata[length+1];

	if(!Receive(this->s, &metadata, length))
	{
		printf("MetaData not received.\n");
	}

	metadata[length] = '\0';
	
	json_object * jobj = json_tokener_parse((char*)metadata);
		
	json_get_array_values(jobj, "Renderers", cam);
	
	this->cam_angle = cam[0];
	this->camState->SetFocalPoint(cam[1], cam[2], cam[3]);
	this->camState->SetViewUp(cam[4], cam[5], cam[6]);
	this->camState->SetPosition(cam[7], cam[8], cam[9]);
	this->camState->GetFocalPoint(this->Center);

	haltActive = false;
	pthread_exit(NULL);
}


void* PVC_Xbox::SendData()
{
	int command = 4;
	if(receiveReadyCommand(s))
	{
		if(-1 == send(s, &command, sizeof(command), 0))
		{
			printf("Command Send Fail. \n");
			exit(1);
		}
		if(-1 == send(s, &camState->camera, sizeof(CameraState), 0))
		{
			printf("Data Send Fail. \n");
			exit(1);
		}
	}
	sending = false;
	pthread_exit(NULL);
}


void PVC_Xbox::Pan(float dx, float dy)
{
	if(camState == NULL)
		return;
	
	float pos[3], fp[3]; 
	camState->GetPosition(pos);
	camState->GetFocalPoint(fp);
	
	camState->OrthogonalizeViewUp();
	float *up = camState->GetViewUp();
	float *vpn = camState->GetViewPlaneNormal();
	float right[3];
	float scale, tmp;
	camState->GetViewUp(up);
	camState->GetViewPlaneNormal(vpn);
	PVC_Math::Cross(vpn, up, right);
	
	scale = camState->GetParallelScale();
	dx *= scale * 2.0;
	dy *= scale * 2.0;
	
	tmp = (right[0]*dx + up[0]*dy);
	pos[0] += tmp;
	fp[0] += tmp; 
	tmp = (right[1]*dx + up[1]*dy); 
	pos[1] += tmp;
	fp[1] += tmp; 
	tmp = (right[2]*dx + up[2]*dy); 
	pos[2] += tmp;
	fp[2] += tmp; 
	camState->SetPosition(pos[0], pos[1], pos[2]);
	camState->SetFocalPoint(fp[0], fp[1], fp[2]);
	
	Create_pThread(Send);
}


float PVC_Xbox::ScaleModifier()
{
	float distance = camState->GetDistance(this->Center[0],this->Center[1],this->Center[2]);
	if(distance < MaxSize/2)
		return 0.5;
	else if(distance > 100000)
		return 100000;
	else
		return distance;
}


void PVC_Xbox::json_get_array_values( json_object *jobj, char *key, float a[]) {
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_get_array_values(jvalue, NULL, a);
    }
    else if (type != json_type_object) {
    	  a[i] = json_object_get_double(jvalue);
      }
    else {
      json_get_array_values(jvalue, "LookAt", a);
      return;
    }
  }
}


void PVC_Xbox::json_parse(json_object * jobj) {
  enum json_type type;
  json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
	printf("Key = %s\n",key);    
    type = json_object_get_type(val);
    switch (type) {
      case json_type_boolean: 
      case json_type_double: 
      case json_type_int: 
      case json_type_string: print_json_value(val);
                           break; 
      case json_type_object: printf("json_type_object\n");
                           jobj = json_object_object_get(jobj, key);
                           json_parse(jobj); 
                           break;
      case json_type_array: printf("type: json_type_array, ");
                          json_parse_array(jobj, key);
                          break;
    }
  }
}


void PVC_Xbox::json_parse_array( json_object *jobj, char *key) {
  enum json_type type;
  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  printf("Array Length: %d\n",arraylen);
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      char* check = "LookAt";
      if(key == check)
      {
    	  printf("HERE");
      }
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object) {
      printf("value[%d]: ",i);
      print_json_value(jvalue);
    }
    else {
      json_parse(jvalue);
    }
  }
}


void PVC_Xbox::print_json_value(json_object *jobj){
  enum json_type type;
  printf("type: ",type);
  type = json_object_get_type(jobj); /*Getting the type of the json object*/
  switch (type) {
    case json_type_boolean: printf("json_type_boolean\n");
                         printf("value: %s\n", json_object_get_boolean(jobj)? "true": "false");
                         break;
    case json_type_double: printf("json_type_double\n");
                        printf("          value: %lf\n", json_object_get_double(jobj));
                         break;
    case json_type_int: printf("json_type_int\n");
                        printf("          value: %d\n", json_object_get_int(jobj));
                         break;
    case json_type_string: printf("json_type_string\n");
                         printf("          value: %s\n", json_object_get_string(jobj));
                         break;
  }
}
