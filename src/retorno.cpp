#include "ros/ros.h"
#include "retorno_autonomo/ret.h"
#include "retorno_autonomo/tray.h"
#include "retorno_autonomo/trayArray.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include "math.h"

    retorno_autonomo::trayArray camino;
    retorno_autonomo::tray datos;
    nav_msgs::Odometry cam;
    geometry_msgs::Twist mover;
    int f=0; float cte=1.5;
    float PI = 3.14159265;
    float v_l; float v_a; float v_c;
    float t_l=0.15; float t_a=0.015; 
    int i=0; int c; int c_p;
    float x; float y;

void retorno(const retorno_autonomo::ret &r){
    c=r.datos;
    c=c-2;
    c_p=c/10;
    f=r.ret;
}

void trayectoria(const retorno_autonomo::trayArray &t){
  camino=t;
}

void posicion(const nav_msgs::Odometry &pos){
    cam=pos;
}

//funciones Euclides
float eu_lineal(float x, float y){
  v_l=sqrt(pow((x-cam.pose.pose.position.x),2)+pow((y-cam.pose.pose.position.y),2));
  return v_l;
}

float eu_angular(float x, float y){
  v_a=atan2(y-cam.pose.pose.position.y,x-cam.pose.pose.position.x);
  return v_a;
}

float conv(float x){
  v_c=x/PI;
  return v_c;
}

int main(int argc, char **argv){
  ros::init(argc, argv, "retorno");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel",1000);
  ros::Subscriber sub_cam = nh.subscribe("/camino", 1000, trayectoria);
  ros::Subscriber sub_pos = nh.subscribe("/RosAria/pose", 1000, posicion);
  ros::Subscriber sub_ret = nh.subscribe("/retornar",1000, retorno);
  ros::Rate loop_rate(10000);
  ros::Rate rate(1);
  c=0;
  datos.x=c;
  datos.y=c;
  datos.theta=c;
  camino.trayectoria.push_back(datos);
  while(ros::ok()){
    ros::spinOnce();
    //moverse a las cordenadas...
    if(f==1){
    x=camino.trayectoria[c].x;
    y=camino.trayectoria[c].y;
      if(abs(conv(eu_angular(x, y))-cam.pose.pose.orientation.z)>t_a){
        mover.linear.x=0;
        if(eu_angular(x, y)<0){
          mover.angular.z=1;
        }
        else {
          mover.angular.z=-1;
        } 
      }
      else {
        mover.angular.z=0;
        if((eu_lineal(x, y))>=t_l){
         mover.linear.x=1; 
        }
        else {
          mover.linear.x=0;
          c=c-c_p;
          if(c<=15){
            if(c==1){
              c=0;
            }
            else{
              c=1;
            }
          }
        }
      }
      if(c==0){
        mover.angular.z=1;
        if(cam.pose.pose.orientation.z<=0.1){
          mover.linear.x=0;
          mover.angular.z=0;
          pub.publish(mover);
          ros::shutdown();
      }
      }
      
    pub.publish(mover);
    }
    else{
    }
    ROS_INFO("voy en %d y coordenadas son x=%f y=%f",c, x, y);
    loop_rate.sleep();
  }
return 0; 
}

