#include "ros/ros.h"
#include "retorno_autonomo/ret.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "reloj");
  ros::NodeHandle nh;
  ros::Publisher pub_ret = nh.advertise<retorno_autonomo::ret>("/retornar", 1000);
  retorno_autonomo::ret r;
  r.ret=0;
  int cont=0;
  ros::Rate loop_rate(5);
  while (ros::ok()){
    if(cont==100){
        r.ret=1;
        r.datos=cont;
        pub_ret.publish(r);
        ros::shutdown();
    }
    ros::spinOnce();
    loop_rate.sleep();
    pub_ret.publish(r);
    r.ret=0;
    cont=cont+1;
    ROS_INFO("%d",cont);
  }

  return 0;
}
