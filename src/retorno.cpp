#include "ros/ros.h"
#include "retorno_autonomo/ret.h"
#include "retorno_autonomo/tray.h"
#include "retorno_autonomo/trayArray.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include "math.h"
#include "std_msgs/Int16.h"

    retorno_autonomo::trayArray camino;
    retorno_autonomo::tray datos;
    nav_msgs::Odometry cam;
    geometry_msgs::Twist mover;
    std_msgs::Int16 data;
    int f=0; float cte=1.5;
    float PI = 3.14159265;
    float v_l; float v_a; float v_c;
    float t_l=0.10; float t_a=0.05; 
    int i=0; int c; int c_p; int stop=0;
    float x; float y; float z; float w;
    float flag;


void retorno(const retorno_autonomo::ret &r){
    c=r.datos;
    c=c-15;
    c_p=4;
    f=r.ret;
    stop=1;
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

//funcion angular
float eu_angular(float x, float y){
  v_a=atan2(y-cam.pose.pose.position.y,x-cam.pose.pose.position.x);
  return v_a;
}
//

//transformar coordenadas rotatorias del robot a radianes
float conv(float z, float w){ 
  float r_min; float r_max; float rad_min; float rad_max; float m;
    if(z*w>0){
        if(w<=1 && w>=0.7){
            r_min=1; r_max=0.7; rad_min=0; rad_max=PI/2;
            m=(rad_max-rad_min)/(r_max-r_min);
            v_c=m*(abs(w)-r_min)+rad_min; 
        }
        if(w<0.7 && w>=0){
            r_min=0.7; r_max=0; rad_min=PI/2; rad_max=PI;
            m=(rad_max-rad_min)/(r_max-r_min);
            v_c=m*(abs(w)-r_min)+rad_min;  
        }
    }
    else{
        if(abs(w)<=1 && abs(w)>=0.7){
            r_min=1; r_max=0.7; rad_min=0; rad_max=PI/2;
            m=(rad_max-rad_min)/(r_max-r_min);
            v_c=m*(abs(w)-r_min)+rad_min; 
            v_c=v_c*-1;
        }  
        if(abs(w)<0.7 && abs(w)>=0){
            r_min=0.7; r_max=0; rad_min=PI/2; rad_max=PI;
            m=(rad_max-rad_min)/(r_max-r_min);
            v_c=m*(abs(w)-r_min)+rad_min; 
            v_c=v_c*-1;
        }  
    }   
    if(abs(v_c)==PI){
        v_c=PI;
    }
  return v_c;
}
//

int main(int argc, char **argv){
  ros::init(argc, argv, "retorno");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel",1000);
  ros::Publisher pub_flag = nh.advertise<std_msgs::Int16>("/reboot",1000);
  ros::Subscriber sub_cam = nh.subscribe("/camino", 1000, trayectoria);
  ros::Subscriber sub_pos = nh.subscribe("/RosAria/pose", 1000, posicion);
  ros::Subscriber sub_ret = nh.subscribe("/retornar",1000, retorno);
  ros::Rate loop_rate(1000);//rate del while
  ros::Rate robot(20);// publicacion para moverse
  c=0;
  datos.x=c;
  datos.y=c;
  datos.theta=c;
  camino.trayectoria.push_back(datos);
  while(ros::ok()){
    ros::spinOnce();  
    //moverse a las cordenadas...
    if(f==1){//
    x=camino.trayectoria[c].x;
    y=camino.trayectoria[c].y;
    z=cam.pose.pose.orientation.z;
    w=cam.pose.pose.orientation.w;
    //empieza secuencia de retorno
    switch(stop){
      case 1://rotamos para llegar al angulo correcto
        //funcion para rotar
        if(abs(eu_angular(x, y)-conv(z, w))>t_a){
          mover.linear.x=0;
          
          if((y-cam.pose.pose.position.y)>0){
            mover.angular.z=-0.3;
          }
          else {
            mover.angular.z=0.3;
          }
          //freno
          if(abs(eu_angular(x, y)-conv(z, w))<=t_a){
            mover.linear.x=0;
            mover.angular.z=0;
            pub.publish(mover);
            robot.sleep();
          }
        }
        else{
          stop=2;
        }
        pub.publish(mover);
        robot.sleep();
      break;
         
      case 2://avanzamos hasta avanzar la distancia determinada
      //funcion para avanzar
        if((eu_lineal(x, y))>t_l){
          mover.linear.x=0.25; 
          mover.angular.z=0;   
          //freno
          if((eu_lineal(x, y))<=t_l){
            mover.linear.x=0;
            mover.angular.z=0;
            pub.publish(mover);
            robot.sleep();
          }
        }
        else{
          stop=3;
        }
        pub.publish(mover);
        robot.sleep();
      break;

      case 3://cambiamos coordenadas de trayectoria
        //funcion para recorrer matriz de coordenadas
          c=c-c_p;
          if(c>25){
            stop=1;
          }
          else{
            if(c<=5){
              if(c<=1){
                c=0;
                stop=4;
              }
              else{
                c=1;
                stop=1;
              }
            }
          }
      break;

      case 4://rotamos al angulo de origen
        //funcion para rotar robot a su posicion original
          if(abs(conv(z, w))>0.01){
            mover.linear.x=0;
            mover.angular.z=0.5;
            //freno
          if(abs(conv(z, w))<=0.01){
            mover.linear.x=0;
            mover.angular.z=0;
            pub.publish(mover);
            robot.sleep();
          }
          }
          else{
            mover.linear.x=0;
            mover.angular.z=0;            
            stop=5;
          }
        pub.publish(mover);
        robot.sleep();
      break;

     case 5://FIN
        flag=1;
        data.data=flag;
        pub_flag.publish(data);
        f=6;
      break;

     //hacer default para no hacer nada y esperar denuevo otro retorno
    }
//
    ROS_INFO("Estoy en la secuencia %d, voy en %d y coordenadas son x=%f y=%f y el angulo es %f" , stop, c, x, y, eu_angular(x, y));
    loop_rate.sleep();
    }
  }
return 0; 
}
