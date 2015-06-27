/*
NOTE: add concentric circles brightest in the middle
	  darker as u go out, will make it look like a drop of water
	  maybe about 6 circles. And dont erase at first

DWORD GetCurrentThreadId(VOID)

HDESK GetThreadDesktop(
  DWORD dwThreadId   // thread identifier
);

 BOOL CloseDesktop(
  HDESK hDesktop  // handle to desktop to close
);


*/
//Draws falling balls on the desktop
// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h> //needed to play a sound using windows also need to insert winmm.lib
					  //C:\Program Files\Microsoft Visual Studio\VC98\Lib


// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"  // class name
#define WINDOW_WIDTH  300              // size of window
#define WINDOW_HEIGHT 150
#define BALL_RADIUS5 10
#define BALL_RADIUS4 8
#define BALL_RADIUS3 6
#define BALL_RADIUS2 4
#define BALL_RADIUS1 2

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// PROTOTYPES /////////////////////////////////////////////
void BouncingBall(HDC hdc, int & iStartPos, int iEndPos, int iXPos, float & iVelocity);

// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[80],
	 buffer2[80];                // used to print text
HPEN blue_pen, red_pen1, red_pen2, red_pen3, red_pen4, red_pen5;
HBRUSH blue_brush, red_brush1, red_brush2,red_brush3,red_brush4,red_brush5;
float gravity= 0.1f,
	  friction= 0.9995f;
int  delay=2;
bool startOver=false;

// FUNCTIONS //////////////////////////////////////////////

//\\\\\\\\\\\\\\\\\\\\\\\\\ WinProc ///////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   //creates a handle to the device context

	//find out what the message is
	switch(msg)
	{	
	
	case WM_CREATE: //called when window is created
		{// do initialization stuff here
			blue_pen = CreatePen(PS_SOLID, 0, RGB(0,0,255));
			blue_brush = CreateSolidBrush(RGB(0,0,255));
			red_pen5= CreatePen(PS_SOLID, 0, RGB(255,0,0));
			red_pen4= CreatePen(PS_SOLID, 0, RGB(255,50,50));
			red_pen3= CreatePen(PS_SOLID, 0, RGB(255,100,100));
			red_pen2= CreatePen(PS_SOLID, 0, RGB(255,150,150));
			red_pen1= CreatePen(PS_SOLID, 0, RGB(255,200,200));
			
			red_brush5 = CreateSolidBrush(RGB(255,0,0));
			red_brush4 = CreateSolidBrush(RGB(255,50,50));
			red_brush3 = CreateSolidBrush(RGB(255,100,100));
			red_brush2 = CreateSolidBrush(RGB(255,150,150));
			red_brush1 = CreateSolidBrush(RGB(255,200,200));

			return(0);
		} break;

	case WM_PAINT: //called when window needs repainting
		{//simply validate the window
		    
			//ValidateRect(hwnd, NULL);
			hdc = BeginPaint(hwnd,&ps);
		    EndPaint(hwnd,&ps);
			
			return(0);
		} break;

	case WM_KEYDOWN :
		{
			int virtual_code= (int)wparam;
			int key_bits= (int)lparam;
			
			switch(virtual_code)
			{
				case VK_LEFT :
				{
					gravity += 0.01f; 
					break; //left arrow
				}
				case VK_RIGHT : 
				{
					gravity -= 0.01f;
					break; //right arrow
				}
				case VK_UP : 
				{
					friction += 0.0001f;
					break; //up arrow
				}
				case VK_DOWN : 
				{
					friction -= 0.0001f;
					break; //down arrow
				}
				case 0x30 :
				{
					delay = 0;
					break;
				}
				case 0x31 :
				{
					delay = 1;
					break;
				}
				case 0x32 :
				{
					delay = 2;
					break;
				}
				case 0x33 :
				{
					delay = 3;
					break;
				}
				case 0x34 :
				{
					delay = 4;
					break;
				}
				case 0x35 :
				{
					delay = 5;
					break;
				}
				case 0x36 :
				{
					delay = 6;
					break;
				}
				case 0x53 :
				{
					startOver= true;
					break;
				}


			}//end switch on virtual_key

			return(0);//let windows know u handled the message
		}
	case WM_DESTROY: 
		{// kill the application			
			
			DeleteObject(blue_pen);
			DeleteObject(blue_brush);
			DeleteObject(red_pen1);
			DeleteObject(red_pen2);
			DeleteObject(red_pen3);
			DeleteObject(red_pen4);
			DeleteObject(red_pen5);
			
			DeleteObject(red_brush1);
			DeleteObject(red_brush2);
			DeleteObject(red_brush3);
			DeleteObject(red_brush4);
			DeleteObject(red_brush5);
			
			//close the program
			PostQuitMessage(0);
			return(0);
		} break;

	default:break;

    } // end main switch

	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

//\\\\\\\\\\\\\\\\\\\\\\\\ WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{

	WNDCLASS winclass;	// this will hold the class created
	HWND	 hwnd;		// generic window handle
	MSG		 msg;		// generic message
	HDC      hdc;       // generic dc
	RECT rect;
	float velocity[15];
	int ball_y[15],
		ball_x[15],
		iEndPos[15],
		index=0;
	GetThreadDesktop(GetCurrentThreadId());

	//fill in the window class stucture
	winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDR_MENU1);
	winclass.lpszClassName	= WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
							  "Bouncing on the desktop",	 // title
							  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						 	  0,600,	   // x,y
							  WINDOW_WIDTH,  // width
						      WINDOW_HEIGHT, // height
							  NULL,	   // handle to parent 
							  NULL,	   // handle to menu
							  hinstance,// instance
							  NULL)))	// creation parms
	{
		MessageBox(hwnd, "Window Could not be Created", NULL, MB_OK); //NULL is default for Error
		return(0);
	}

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance      = hinstance;

	for(index=0; index<=14; index++) //start balls at random positions, set velocity = 0
	{
		ball_y[index]= -10;
		ball_x[index]= rand()%1000;
		iEndPos[index]= rand()%900;
		velocity[index]= 0;
	}
	
	index=0; //reset index

	// enter main event loop
	while(1)
	{
		//check messages
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			// test if this is a quit message
			if (msg.message == WM_QUIT)
				break;
	
			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if
		if(KEY_DOWN(VK_ESCAPE)) //if hit the Esc key quit the program
			PostQuitMessage(0);

    	hdc= GetDC(hwnd);
		
		//--------Begin Text Info---------------------
		SetBkMode(hdc, TRANSPARENT); //set background color
		//first create the eraser string for variable text
		SetTextColor(hdc,RGB(0,255,0)); //set text color black
		TextOut(hdc, 30, 60, buffer, strlen(buffer)); //print text
		TextOut(hdc, 30, 75, buffer2, strlen(buffer2)); //print text
		
		//unvarieing text
		SetTextColor(hdc,RGB(0,255,0)); //set text color black
		TextOut(hdc, 30, 15,"Keys 0-6 set the delay time", strlen("Keys 0-6 set the delay time")); //print text
		TextOut(hdc, 30, 30,"Left and right arrows change gravity", strlen("Left and right arrows change gravity")); //print text
		TextOut(hdc, 30, 45,"Up and down arrows change friction", strlen("Up and down arrows change friction")); //print text
		TextOut(hdc, 30, 90,"S key starts over", strlen("S key starts over")); //print text
		
		//variable text
		sprintf(buffer,"Friction= ", friction);
		sprintf(buffer2,"Gravity= ", gravity);
		TextOut(hdc, 30, 60, buffer, strlen(buffer)); //print text
		TextOut(hdc, 30, 75, buffer2, strlen(buffer2)); //print text
		//--------End Text Info-------------------------

		ReleaseDC(hwnd, hdc);
		
		
		hdc= GetWindowDC(GetThreadDesktop(GetCurrentThreadId()) );
		
		//-------------Erasing ball----------------
		//fill in rect struct for erasing ball
		//rect.left = ball_x[index] - BALL_RADIUS5;
		//rect.right = ball_x[index] + BALL_RADIUS5;
		//rect.top = ball_y[index] - BALL_RADIUS5;
		//rect.bottom = ball_y[index] + BALL_RADIUS5;
		
		//Draw erasing ball in the old position
		//SelectObject(hdc, red_pen5);
		//SelectObject(hdc, red_brush5);
		//Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		
		//------------End Erasing ball----------------

		//-------------Test to see whats going on-----------
		//if the ball hits the ground, bounce back. The reason its here is because the erasing ball
		//needs the old position to erase the drawing ball.
		if(ball_y[index] >= iEndPos[index]) 
		{
			velocity[index] = -velocity[index];
			ball_y[index] = iEndPos[index]; //need to rest the y position because of >=, the y could be > 370
		}
		
		//allows user to restart balls in a new possition if they want to
		//the reason its put here is so that if the user decides to restart, the erasing ball
		//will erase the drawing ball, and won't leave ball marks all over.
		if(startOver) 
		{
			for(index=0; index<=14; index++) //start balls at random positions, set velocity = 0
			{
				ball_y[index]= -10;
				ball_x[index]= rand()%1000;
				iEndPos[index]= rand()%900;
				velocity[index]= 0;
			}//end for loop on index
			
			startOver= false; //reset startOver back to false 
		}//end if on startOver

		//--------------End Test to see whats going on
		
		//-------------- Move the ball--------------
		velocity[index]+= gravity;
		velocity[index]*= friction;
		ball_y[index]+= (int) velocity[index];
		//-------------End Move the ball------------

		//---------------Draw the Ball------------------
		//Redraw the ball in its new position
	
		//fill in rect struct for drawing ball
		//ball 5
		rect.left = ball_x[index] - BALL_RADIUS5;
		rect.right = ball_x[index] + BALL_RADIUS5;
		rect.top = ball_y [index] - BALL_RADIUS5;
		rect.bottom = ball_y[index] + BALL_RADIUS5;
		
		//Draw the ball in its new spot
		SelectObject(hdc, red_pen5);
		SelectObject(hdc, red_brush5);
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		//end ball 5

		//ball 4
		rect.left = ball_x[index] - BALL_RADIUS4;
		rect.right = ball_x[index] + BALL_RADIUS4;
		rect.top = ball_y [index] - BALL_RADIUS4;
		rect.bottom = ball_y[index] + BALL_RADIUS4;
		
		//Draw the ball in its new spot
		SelectObject(hdc, red_pen4);
		SelectObject(hdc, red_brush4);
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		//end ball 4

		//ball 3
		rect.left = ball_x[index] - BALL_RADIUS3;
		rect.right = ball_x[index] + BALL_RADIUS3;
		rect.top = ball_y [index] - BALL_RADIUS3;
		rect.bottom = ball_y[index] + BALL_RADIUS3;
		
		//Draw the ball in its new spot
		SelectObject(hdc, red_pen3);
		SelectObject(hdc, red_brush3);
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		//end ball 3

		//ball 2
		rect.left = ball_x[index] - BALL_RADIUS2;
		rect.right = ball_x[index] + BALL_RADIUS2;
		rect.top = ball_y [index] - BALL_RADIUS2;
		rect.bottom = ball_y[index] + BALL_RADIUS2;
		
		//Draw the ball in its new spot
		SelectObject(hdc, red_pen2);
		SelectObject(hdc, red_brush2);
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		//end ball 2

		//ball 1
		rect.left = ball_x[index] - BALL_RADIUS1;
		rect.right = ball_x[index] + BALL_RADIUS1;
		rect.top = ball_y [index] - BALL_RADIUS1;
		rect.bottom = ball_y[index] + BALL_RADIUS1;
		
		//Draw the ball in its new spot
		SelectObject(hdc, red_pen1);
		SelectObject(hdc, red_brush1);
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		//end ball 1
		//----------------End draw the ball---------------

		//if index increments to 14 (or last ball) start drawing from the
		//begining again over again (meaning draw ball b0, b1,...b14 start back at b1)
		index++;
		if(index >=14)
			index=0;
		
		//slow things down a bit
		Sleep(delay);
				
		//Release device context
		ReleaseDC(GetThreadDesktop(GetCurrentThreadId()), hdc);
		
	} // end while

	

	// return to Windows like this
	return(msg.wParam);

} // end WinMain