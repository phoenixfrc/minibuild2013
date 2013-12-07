#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	enum BlockerState {Down, Up};
	enum KickerState {Forward, Back};
	
	RobotDrive myRobot; // robot drive system
	Joystick leftStick; // only joystick
	Joystick rightStick;
	Joystick gamePad;
	DriverStationLCD *lcd;
	Servo cameraPivotMotor;
	Servo cameraElevateMotor;
	float cameraElevateAngle;
	float cameraPivotAngle;
	Compressor compressor;
	Relay kicker;
	Relay blocker;
	BlockerState blockerState;
	KickerState kickerState;
public:
	RobotDemo(void):
		myRobot(3, 4, 1, 2),	// these must be initialized in the same order
		// change to 1, 2, 3, 4 on myRobot
		leftStick(2),		// as they are declared above.
		rightStick(1),
		gamePad(3),
		lcd(DriverStationLCD::GetInstance()),
		cameraPivotMotor(9),
		cameraElevateMotor(10),
		compressor(9,1),
		kicker(2),
		blocker(3),
		blockerState(Up),
		kickerState(Back)
	{
		myRobot.SetExpiration(0.1);

		//myRobot.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		//myRobot.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		//myRobot.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		//myRobot.SetInvertedMotor(RobotDrive::kRearRightMotor, true);
		compressor.Start();
	}
	void RobotInit(){
		cameraElevateAngle =
							(cameraElevateMotor.GetMaxAngle()-cameraElevateMotor.GetMinAngle()) * 2/3;
		cameraPivotAngle = 0;
		cameraPivotMotor.SetAngle(cameraPivotAngle);
		cameraElevateMotor.SetAngle(cameraElevateAngle);
		blocker.Set(Relay::kReverse);
		kicker.Set(Relay::kOff);
		
	}
	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.3, 0.0); 	// drive forwards half speed
		Wait(1.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		myRobot.SetSafetyEnabled(true);
		lcd->PrintfLine(DriverStationLCD::kUser_Line5, "Hi there from phoenix1");
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "%s", "in OpCtl - rev 1");
		lcd->UpdateLCD();
		int loopcount = 0;
		int stateHoldCount = 50;
		while (IsOperatorControl())
		{
			if (loopcount%400 == 0) {
				lcd->PrintfLine(DriverStationLCD::kUser_Line2, "js val %f %f %d",
						leftStick.GetAxis(Joystick::kXAxis),
						rightStick.GetAxis(Joystick::kXAxis),
						loopcount);
				//lcd->UpdateLCD(); //now updating below
			}
			
			myRobot.TankDrive(leftStick, rightStick); // drive with arcade style (use right stick)
			if (gamePad.GetY() > 0.05 ||
					gamePad.GetY() < -0.05) {
				cameraElevateAngle += gamePad.GetY();
				if (cameraElevateAngle < cameraElevateMotor.GetMinAngle())
					cameraElevateAngle = cameraElevateMotor.GetMinAngle();
				if (cameraElevateAngle > cameraElevateMotor.GetMaxAngle())
					cameraElevateAngle = cameraElevateMotor.GetMaxAngle();
			}
			if (gamePad.GetX() > 0.05 ||
					gamePad.GetX() < -0.05) {
				cameraPivotAngle += gamePad.GetX();
				if (cameraPivotAngle < cameraPivotMotor.GetMinAngle())
					cameraPivotAngle = cameraPivotMotor.GetMinAngle();
				if (cameraPivotAngle > cameraPivotMotor.GetMaxAngle())
					cameraPivotAngle = cameraPivotMotor.GetMaxAngle();
			}
			cameraPivotMotor.SetAngle(cameraPivotAngle);
			cameraElevateMotor.SetAngle(cameraElevateAngle);
			KickerState newKickerState = gamePad.GetRawButton(8)? Forward:Back;
			if(newKickerState != kickerState){
				if (newKickerState == Forward){
					kicker.Set(Relay::kReverse);
				}
				else{
					kicker.Set(Relay::kOff);
				}
				kickerState = newKickerState;
			}
			//Detrmine if State button is pressed
			bool blockerButtonPressed = gamePad.GetRawButton(7);
			if (stateHoldCount >= 50){
				//Change state if button pressed
				if (blockerState == Up && blockerButtonPressed)
				{
					stateHoldCount = 0;
					blockerState = Down;
				}
				else if(blockerState == Down && blockerButtonPressed){
					stateHoldCount = 0;
					blockerState = Up;
				}
				//else no change to state
			}
			else{ //holding for 50 counts
				stateHoldCount++;
			}
			//Set blocker output based on state
			char *blockerStateString = "Blocker is up.";
			bool timeToDisplayBlockerState = loopcount%40 == 0;
			if (blockerState == Up){
				blocker.Set(Relay::kReverse);
			}
			else{//blocker is down
				blocker.Set(Relay::kOff);
				blockerStateString = "Blocker is down.";
			}
			if (timeToDisplayBlockerState) {
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, blockerStateString);
				lcd->UpdateLCD();
			}
			Wait(0.005);				// wait for a motor update time
			loopcount++;
		}
	}
	
	/**
	 * Runs during test mode
	 * Blue X = button 1
	 * Green A = button 2
	 * Red B = button 3
	 * Yellow Y = button 4
	 * LB = button 5
	 * RB = button 6
	 * LT = button 7
	 * RT = button 8
	 */
	void Test() {
		int loopcount = 0;
		while (IsTest())
		{
			bool button1 = gamePad.GetRawButton(1);
			bool button2 = gamePad.GetRawButton(2);
			bool button3 = gamePad.GetRawButton(3);
			bool button4 = gamePad.GetRawButton(4);
			bool button5 = gamePad.GetRawButton(5);
			bool button6 = gamePad.GetRawButton(6);
			bool button7 = gamePad.GetRawButton(7);
			bool button8 = gamePad.GetRawButton(8);
			lcd->PrintfLine(DriverStationLCD::kUser_Line3,"Test Mode");
			lcd->PrintfLine(DriverStationLCD::kUser_Line4,"%d", loopcount/10);
			lcd->PrintfLine(DriverStationLCD::kUser_Line5, "gamepad = %c%c%c%c %c%c%c%c", 
					button1 ? '1': '0',
					button2 ? '1': '0',
					button3 ? '1': '0',
					button4 ? '1': '0',
					button5 ? '1': '0',
					button6 ? '1': '0',
					button7 ? '1': '0',
					button8 ? '1': '0');
			lcd->UpdateLCD();
			Wait(0.1); // wait for to minimize display output
			loopcount++;
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

