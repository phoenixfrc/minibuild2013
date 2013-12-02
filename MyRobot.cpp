#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
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
public:
	RobotDemo(void):
		myRobot(1, 2, 3, 4),	// these must be initialized in the same order
		// change to 1, 2, 3, 4 on myRobot
		leftStick(2),		// as they are declared above.
		rightStick(1),
		gamePad(3),
		lcd(DriverStationLCD::GetInstance()),
		cameraPivotMotor(9),
		cameraElevateMotor(10),
		compressor(9,1),
		kicker(2),
		blocker(3)
	{
		myRobot.SetExpiration(0.1);
		compressor.Start();
	}
	void RobotInit(){
		cameraElevateAngle =
							(cameraElevateMotor.GetMaxAngle()-cameraElevateMotor.GetMinAngle()) * 2/3;
		cameraPivotAngle = 0;
		cameraPivotMotor.SetAngle(cameraPivotAngle);
		cameraElevateMotor.SetAngle(cameraElevateAngle);
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
		while (IsOperatorControl())
		{
			if (loopcount%400 == 0) {
				lcd->PrintfLine(DriverStationLCD::kUser_Line2, "js val %f %f %d",
						leftStick.GetAxis(Joystick::kXAxis),
						rightStick.GetAxis(Joystick::kXAxis),
						loopcount);
				lcd->UpdateLCD();
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
			if (gamePad.GetRawButton(8)){
				kicker.Set(Relay::kReverse);
				blocker.Set(Relay::kReverse);
			}
			else{
				kicker.Set(Relay::kOff);
				blocker.Set(Relay::kOff);
			}
			Wait(0.005);				// wait for a motor update time
			loopcount++;
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

