#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "retorno_autonomo/ret.h"
#include "retorno_autonomo/tray.h"
#include "retorno_autonomo/trayArray.h"
#include "math.h"

retorno_autonomo::trayArray cam;
retorno_autonomo::tray datos;
retorno_autonomo::ret r_t;
retorno_autonomo::ret c;
nav_msgs::Odometry pos_t;
int count=0;

void posicion(const nav_msgs::Odometry &pos){
  pos_t=pos;
}

void accion(const retorno_autonomo::ret &r){
  r_t=r;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "trayectoria");
  ros::NodeHandle nh;
  ros::Subscriber sub_pos = nh.subscribe("RosAria/pose", 1000, posicion);
  ros::Subscriber sub_ret = nh.subscribe("/retornar", 1000, accion);
  ros::Publisher pub_cam = nh.advertise<retorno_autonomo::trayArray>("/camino",1000);
  ros::Rate loop(5);
  while(ros::ok()){
    ros::spinOnce();
    datos.x=pos_t.pose.pose.position.x;
    datos.y=pos_t.pose.pose.position.y;
    datos.theta=pos_t.pose.pose.orientation.z;
    if(r_t.ret==0){
      cam.trayectoria.push_back(datos);
      r_t.ret=2;
      count=count+1;
    }
    if(r_t.ret==1){
    cam.trayectoria.push_back(datos);
    count=count+1;
    pub_cam.publish(cam);
    ROS_INFO("los datos son %d",count);
    ros::shutdown();
    }
 
  }  
  return 0;
}
