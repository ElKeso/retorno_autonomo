#include "ros/ros.h"
#include "geometry_msgs/Twist.h"


int flag=0; int a;
geometry_msgs::Twist mover;

int main(int argc, char **argv){
  ros::init(argc, argv, "freno");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel",1000);
  ros::Rate loop_rate(10);
  while (ros::ok()){
    switch(flag){
        case 0:
            for(int i=0; i<51; i=i+1){
                a=50-i;
                ROS_INFO("voy a detenerme en %d" ,a);
                mover.linear.x=0.1;
                pub.publish(mover);               
                loop_rate.sleep();
            }
            flag=1;
        break;
        case 1:
           //for(int i=0; i<30; i=i+1){
           //     mover.linear.x=-0.1;
           //     pub.publish(mover);
           //   loop_rate.sleep();
           //     }
            flag = 2;
        break;
        case 2:
                for(int i=0; i<1000; i=i+1){
                mover.linear.x=0;
                pub.publish(mover);
                }
              flag = 3;
        break;  
      case 3:
        ros::shutdown();  
      break;
    }
  }
  return 0;
}
