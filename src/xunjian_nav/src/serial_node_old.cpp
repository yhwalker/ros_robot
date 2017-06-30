#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include<std_msgs/Float64.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Int16.h>
#include<sensor_msgs/Range.h> //for ultrasonic sensor
#include <stdio.h>
#include"xunjian_nav/Wheelvelocity.h"
#include"xunjian_nav/Sensor.h"
#include"xunjian_nav/Ultrasound.h"
#include<iostream>
#define PI 3.1415926
serial::Serial ser;
using namespace std;
/*void write_callback(const std_msgs::String::ConstPtr& msg){
    ROS_INFO_STREAM("Writing to serial port" << msg->data);
    ser.write(msg->data);
}*/


/*void vr_callback(const std_msgs::Int16& v_r){
    ROS_INFO_STREAM("Writing to serial port" << v_r.data);
    wheel_speed.right_speed = v_r.data;
    set_robot_speed();
}
*/

struct WHEEL_SPEED {
    long left_speed;
    long right_speed;
};

struct DRIVE_INFO {
    double yaw;
    long left_odm;
    long right_odm;
};

struct ULTRA{
	unsigned char ultra_1;
	unsigned char ultra_2;
	unsigned char ultra_3;
	unsigned char ultra_4;
	unsigned char ultra_5;
	unsigned char ultra_6;
};
struct ULTRA  ultra_info;
struct DRIVE_INFO  drive_info;
struct WHEEL_SPEED wheel_speed;



int frame_decode(unsigned char *data_buf)
{
    unsigned char buff_len = 0;
    unsigned char CMD = 0;

    // not ros frame type
    if (data_buf[2] != 0xF8)
    return (-1);

    buff_len = data_buf[1];
    CMD = data_buf[3];

    switch (CMD) {
        case 0xAA:
        drive_info.yaw = data_buf[4] + data_buf[5] * 256 ;

        drive_info.left_odm = data_buf[6] + data_buf[7] * 256 + data_buf[8] * 65536 + data_buf[9] * 16777216;

        drive_info.right_odm = data_buf[10] + data_buf[11] * 256 + data_buf[12] * 65536 + data_buf[13] * 16777216;

        //printf("yaw: %.1f \t\n",drive_info.yaw);
       //printf("right_odm: %d \t\n",drive_info.right_odm);
       //printf("left_odm: %d \t\n",drive_info.left_odm);
        break;
	case 0xBB:
	 ultra_info.ultra_1 = data_buf[4];
	 ultra_info.ultra_2 = data_buf[5];
	 ultra_info.ultra_3 = data_buf[6];
	 ultra_info.ultra_4 = data_buf[7];
	 ultra_info.ultra_5 = data_buf[8];
	 ultra_info.ultra_6 = data_buf[9];
         /*printf("%d, %d, %d, %d, %d, %d\n",ultra_info.ultra_1,ultra_info.ultra_2
					,ultra_info.ultra_3,ultra_info.ultra_4
					,ultra_info.ultra_5,ultra_info.ultra_6);*/

	break;
        default:
        break;
    }
    // checksum calculate

    return (0);
}

unsigned char robot_checksum(unsigned char *buff,unsigned char len)
{
    unsigned char chk_tmp = 0;
    unsigned char i = 0;
    for (i = 0; i < len; i++)
    chk_tmp += buff[i];
    return (chk_tmp);
}


int send_to_robot(unsigned char cmd,unsigned char *data,unsigned char len)
{
    unsigned char tmp_buf[200];
    tmp_buf[0] = 0x00;
    tmp_buf[1] = 0x00;
    tmp_buf[2] = 0x00;
    tmp_buf[3] = 0x10;
    tmp_buf[4] = len + 2;
    tmp_buf[5] = 0xF8;
    tmp_buf[6] = cmd;
    for (int i = 0; i < len; i++)
    tmp_buf[7 + i] = data[i];
    tmp_buf[7 + len] = robot_checksum(&tmp_buf[4],len + 3);
    ser.write(tmp_buf,7 + len + 1);
}

int set_robot_speed(void)
{
    unsigned char tmp_buf[10];
    tmp_buf[0] = (unsigned char)(wheel_speed.left_speed & 0xFF);
    tmp_buf[1] = (unsigned char)((wheel_speed.left_speed >> 8) & 0xFF);
    tmp_buf[2] = (unsigned char)((wheel_speed.left_speed >> 16) & 0xFF);
    tmp_buf[3] = (unsigned char)((wheel_speed.left_speed >> 24) & 0xFF);

    tmp_buf[4] = (unsigned char)(wheel_speed.right_speed & 0xFF);
    tmp_buf[5] = (unsigned char)((wheel_speed.right_speed >> 8) & 0xFF);
    tmp_buf[6] = (unsigned char)((wheel_speed.right_speed >> 16) & 0xFF);
    tmp_buf[7] = (unsigned char)((wheel_speed.right_speed >> 24) & 0xFF);
    send_to_robot(0x55,tmp_buf,8);
}

unsigned char buffer[100];
unsigned char data_index = 0;

//订阅的回调函数
void v_callback(const xunjian_nav::Wheelvelocity& v){
    //ROS_INFO_STREAM("Writing to serial port" << v.vl<<v.vr);
    wheel_speed.left_speed = v.vl;
    wheel_speed.right_speed =v.vr;
    printf("%d,%d\n",wheel_speed.left_speed,wheel_speed.right_speed);
    set_robot_speed();
}

void encoderClean(const std_msgs::Empty& cmd_msg){
	unsigned char tmp_buf[1];
	tmp_buf[0] = 0x01;
	send_to_robot(0x56,tmp_buf,1);
}



int main (int argc, char** argv){
    ros::init(argc, argv, "serial_node");
    ros::NodeHandle nh;
    ros::Subscriber encoder_sub=nh.subscribe("encoder_reset",1000,encoderClean);
    ros::Subscriber v_sub = nh.subscribe("wheel_speed", 1000, v_callback);
    ros::Publisher sensor_pub = nh.advertise<xunjian_nav::Sensor>("encoder", 1000);
    ros::Publisher range_pub=nh.advertise<xunjian_nav::Ultrasound>("range_dist",1000);

    xunjian_nav::Sensor msg;
    xunjian_nav::Ultrasound range_msg;

    msg.yawAngle=0.0;
    msg.leftEncoder=0;
    msg.rightEncoder=0;
    //ros::Subscriber vR_sub = nh.subscribe("vR", 1000, vr_callback);
    //ros::Publisher read_pub = nh.advertise<struct DRIVE_INFO>("read", 1000);

    //ros::Publisher read_pub1=n.advertise<std_msgs::Float64>("yaw_angle",1000);
    //ros::Publisher read_pub2=n.advertise<std_msgs::Int32>("left_encoder",1000);
    //ros::Publisher read_pub3=n.advertise<std_msgs::Int32>("right_encoder",1000);


    //std_msgs::Float64 msg1;//创建一个64位的Float型消息变量，用于发布偏航角。
    //std_msgs::Int32   msg2;//创建一个32位整形的消息变量，用于发布左轮的编码器值；
    //std_msgs::Int32   msg3;//创建一个32位整形的消息变量，用于发布右轮编码器值；
    //msg1.data=0.0;
    //msg2.data=0;
    //msg3.data=0;

    try
    {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen()){
        ROS_INFO_STREAM("Serial Port initialized");
    }else{
        return -1;
    }

    ros::Rate loop_rate(500);
    while(ros::ok()){

        // read_pub2.publish(msg2)；
        //read_pub3.publish(msg3);
        //消息被发布出去之后，需要进行订阅，就是在写odom节点时需要发布里程计信息同时也需要订阅编码器和陀螺仪信息
        unsigned char buf_tmp;

        // read byte data from serial buffer
        if(ser.available()){
            ser.read(&buf_tmp,1);
            buffer[data_index] = buf_tmp;
            if (buffer[0] != 0x10) {
                data_index = 0;
            } else {
                data_index++;
                if ((data_index >= 2) && (data_index ==buffer[1] + 3)) {
                    if (buffer[data_index - 1] == robot_checksum(&buffer[1], buffer[1] + 1)) {
                        frame_decode(buffer);
                    }
                    data_index = 0;
                }
            }

        msg.yawAngle=drive_info.yaw*PI/180.0;
        msg.leftEncoder=drive_info.left_odm;
        msg.rightEncoder=drive_info.right_odm;
        range_msg.ultra_1=ultra_info.ultra_1;
        range_msg.ultra_2=ultra_info.ultra_2;
        range_msg.ultra_3=ultra_info.ultra_3;
        range_msg.ultra_4=ultra_info.ultra_4;
        range_msg.ultra_5=ultra_info.ultra_5;
        range_msg.ultra_6=ultra_info.ultra_6;

	sensor_pub.publish(msg);
	range_pub.publish(range_msg);

      }

        ros::spinOnce();
        loop_rate.sleep();

    }
}
