/* * * * * * * * * * * * * * * * * * * *
 * ========== VOICE CONTROL ========== *
 *         Juan Manuel PEREZ RUA       *
 *        University of Burgundy       *
 * =================================== *
 * * * * * * * * * * * * * * * * * * * */

// ======================================== //

#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES
#include <string>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

// ======================================== //
class VoiceAction{
public:
    VoiceAction();
    void actionLoop();
    void stopRobot();
private:
	geometry_msgs::Twist curVel;
	ros::Publisher vel_pub_;
    ros::Subscriber voicem_;
    ros::NodeHandle nh_;
    std::string command;
    bool ack;

    void incoming(const std_msgs::String::ConstPtr& msg);
};

VoiceAction::VoiceAction():ack(false){
	vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    voicem_ = nh_.subscribe("recognizer/output", 5, &VoiceAction::incoming, this);
}

void VoiceAction::incoming(const std_msgs::String::ConstPtr &msg){
    command = msg->data.c_str();
    ack=true;
    std::cout<<"COM: "<<command<<std::endl;
}

void VoiceAction::actionLoop(){
    const int rate = 30; //Hz
    const float lin_vel = 0.3;
    const float ang_vel = 1.0;
    ros::Rate r(rate);

    bool locked=true;
    while (ros::ok()){

        if (ack){
            if (command.compare("university of burgundy")==0){
                std::cout<<"ACT: UNLOCKED"<<std::endl;
                locked=false;
                curVel.angular.z=0;
                curVel.linear.x=0;
            }

            if (!locked && command.find("turtlebot forward")!=std::string::npos){
                std::cout<<"ACT: MOVING FORWARD"<<std::endl;
                curVel.angular.z=0;
                curVel.linear.x=lin_vel;
            }

            if (!locked && command.find("turtlebot backward")!=std::string::npos){
                std::cout<<"ACT: MOVING BACKWARD"<<std::endl;
                curVel.angular.z=0;
                curVel.linear.x=-lin_vel;
            }

            if (!locked && command.find("turtlebot right")!=std::string::npos){
                std::cout<<"ACT: ROTATING RIGHT"<<std::endl;
                curVel.angular.z=ang_vel;
                curVel.linear.x=0;
            }

            if (!locked && command.find("turtlebot left")!=std::string::npos){
                std::cout<<"ACT: ROTATING LEFT"<<std::endl;
                curVel.angular.z=-ang_vel;
                curVel.linear.x=0;
            }

            if (!locked && (command.find("halt")!=std::string::npos || (command.find("turtlebot stop")!=std::string::npos))){
                std::cout<<"ACT: HALTING"<<std::endl;
                curVel.angular.z=0;
                curVel.linear.x=0;
            }

            ack=false; //Last command attended.
        }
        vel_pub_.publish(curVel);
        ros::spinOnce();
        r.sleep();
    }
    stopRobot();
}

void VoiceAction::stopRobot(){
    if (ros::ok()){
        for (int i=1; i<5; i++){
            curVel.angular.z = 0.0;
            curVel.linear.x  = 0.0;
            vel_pub_.publish(curVel);
            ros::spinOnce();
        }
    }
}

int main(int argc, char** argv)
{
    // Initialize classes and configurations
    ros::init(argc, argv, "tvcontrol");

    VoiceAction va;
    va.actionLoop();
	return 0;
}

