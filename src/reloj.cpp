#include "ros/ros.h"
#include "retorno_autonomo/ret.h"
#include "std_msgs/Int16.h"


int f=0; int complete=0;
void chatterCallback(const std_msgs::Int16  &msg){
  if(msg.data==1){
    f=1;
  }
  if(msg.data==2){
    if(complete==0){
      f=2;
    }
    else{
      f=0;
    }
  }
  if(msg.data==3){
    f=0;
  }
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "reloj");
  ros::NodeHandle nh;
  ros::Publisher pub_ret = nh.advertise<retorno_autonomo::ret>("/retornar", 1000);
  ros::Subscriber sub = nh.subscribe("/signal", 1000, chatterCallback);
  retorno_autonomo::ret r;
  r.ret=0;
  int cont=0;
  ros::Rate loop_rate(1.8);
while (ros::ok()){
    ros::spinOnce();
    switch(f){
      case 0://stand by

      break;

      case 1://seguimiento
        r.ret=0;
        pub_ret.publish(r);
        cont=cont+1;
      break;

      case 2://retorno
        r.ret=1;
        r.datos=cont;
        pub_ret.publish(r);
        complete=1;
      break; 

      case 3://finish
        f=0;
      break;
    }
    loop_rate.sleep();
    ROS_INFO("VALOR DE RET ES %d", r.ret);
    }
  return 0;
}
