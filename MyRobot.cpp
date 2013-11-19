#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class PhoenixRobot : public SimpleRobot
{
	//CANJaguar leftRear;
	//CANJaguar leftFront;
	//CANJaguar rightRear;
	//CANJaguar rightFront;
	RobotDrive driveTrain; // robot drive system
	Joystick leftStick;
	Joystick rightStick;

public:
	PhoenixRobot(void):
		driveTrain(2,3, 4,5),	// these must be initialized in the same order
		//leftRear(2),
		//leftFront(3),
		//rightRear(4),
		//rightFront(5),
		leftStick(1),		// as they are declared above.
		rightStick(2)
	{
		driveTrain.SetExpiration(0.1);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		driveTrain.SetSafetyEnabled(false);
		driveTrain.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		driveTrain.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering.
	 */
	void OperatorControl(void)
	{
		driveTrain.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			//leftFront.Set(leftStick.GetAxis(Joystick::kXAxis));
			//leftRear.Set(leftStick.GetAxis(Joystick::kXAxis));
			//rightFront.Set(rightStick.GetAxis(Joystick::kXAxis));
			//rightRear.Set(rightStick.GetAxis(Joystick::kXAxis));
			driveTrain.TankDrive(leftStick, rightStick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(PhoenixRobot);

