#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int16.h>
#include <sensor_msgs/Range.h> //for ultrasonic sensor
#include<geometry_msgs/Twist.h>
#include <stdio.h>
#include "xunjian_nav/Wheelvelocity.h"
#include "xunjian_nav/Sensor.h"
#include "xunjian_nav/Ultrasound.h"
#include <iostream>
#define PI 3.1415926
#define HEADER	0x7A
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
    signed short V;
    signed short W;
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
int cmd_vel_vx, cmd_vel_w;

xunjian_nav::Sensor msg;
xunjian_nav::Ultrasound range_msg;

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

	drive_info.V = data_buf[14] | ((data_buf[15] << 8) & 0xFF00);
	drive_info.W = data_buf[16] | ((data_buf[17] << 8) & 0xFF00);

        msg.yawAngle=drive_info.yaw*PI/180.0;
        msg.leftEncoder=drive_info.left_odm;
        msg.rightEncoder=drive_info.right_odm;
	msg.vx = drive_info.V;
	msg.w = drive_info.W;
	
        //printf("%i,%ld,%ld,%d,%d\n",ros::Time::now(),drive_info.left_odm,drive_info.right_odm,drive_info.V,drive_info.W);

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
    tmp_buf[3] = HEADER;
    tmp_buf[4] = len + 2;
    tmp_buf[5] = 0xF8;
    tmp_buf[6] = cmd;
    for (int i = 0; i < len; i++)
    tmp_buf[7 + i] = data[i];
    tmp_buf[7 + len] = robot_checksum(&tmp_buf[4],len + 3);
    ser.write(tmp_buf,7 + len + 1);
}

int set_robot_speed(int vx, int w)
{
    unsigned char tmp_buf[10];
    tmp_buf[0] = (unsigned char)(vx & 0xFF);
    tmp_buf[1] = (unsigned char)((vx >> 8) & 0xFF);
    tmp_buf[2] = (unsigned char)((vx >> 16) & 0xFF);
    tmp_buf[3] = (unsigned char)((vx >> 24) & 0xFF);

    tmp_buf[4] = (unsigned char)(w & 0xFF);
    tmp_buf[5] = (unsigned char)((w >> 8) & 0xFF);
    tmp_buf[6] = (unsigned char)((w >> 16) & 0xFF);
    tmp_buf[7] = (unsigned char)((w >> 24) & 0xFF);
    send_to_robot(0x55,tmp_buf,8);
}

//订阅的回调函数
void v_callback(const geometry_msgs::Twist& vel){
	//ROS_INFO_STREAM("Writing to serial port" << v.vl<<v.vr);
	//wheel_speed.left_speed = v.vl;
	//wheel_speed.right_speed =v.vr;
	cmd_vel_vx = vel.linear.x * 1000;
	cmd_vel_w = vel.angular.z * 1000;
	set_robot_speed(cmd_vel_vx,cmd_vel_w);
	//printf(",,,,,%i,%d,%d\n",ros::Time::now(),cmd_vel_vx,cmd_vel_w);
}

const float data_v[461][2] = { 15,-19.39655172, 30,-38.79310345, 45,-58.18965517, 60,-77.5862069, 75,-96.98275862, 90,-116.3793103, 105,-135.7758621, 120,-155.1724138, 135,-169.7327587, 150,-184.2931036, 165,-198.8534485, 180,-213.4137935, 195,-211.2674829, 210,-209.1211723, 225,-206.9748617, 240,-204.8285511, 255,-189.6440445, 270,-174.4595379, 285,-159.2750313, 300,-144.0905247, 315,-127.9900802, 330,-111.8896356, 345,-95.78919114, 360,-79.68874664, 375,-67.23738155, 390,-54.78601647, 405,-42.33465138, 420,-29.8832863, 435,-19.25646275, 450,-8.629639204, 465,1.997184342, 480,12.62400789, 495,16.00945302, 500,17.13793473, 500,17.13793473, 500,17.13793473, 500,0, 500,0, 500,0, 500,0, 500,0, 500,0, 500,0, 500,0, 500,-20, 500,-40, 500,-51.72413793, 500,-51.72413793, 485,-50.43103448, 470,-49.13793103, 455,-47.84482759, 440,-46.55172414, 420.2934363,-26.55172414, 400.5868726,-6.551724138, 380.8803089,13.44827586, 361.1737452,33.44827586, 366.8958368,13.44827586, 372.6179284,-6.551724138, 378.34002,-26.55172414, 384.0621116,-46.55172414, 375.6178928,-66.55172414, 373.1428632,-72.4137931, 373.1428632,-72.4137931, 373.1428632,-72.4137931, 350.6428632,-84.13151805, 340.5000061,-89.41379403, 340.5000061,-89.41379403, 340.5000061,-89.41379403, 339.5734866,-59.41379403, 338.6469672,-29.41379403, 338.500002,-24.65517256, 338.500002,-24.65517256, 316.000002,-44.10834331, 302.2857155,-55.96551474, 302.2857155,-55.96551474, 302.2857155,-55.96551474, 297.9973609,-25.96551474, 294.2857163,0, 294.2857163,0, 294.2857163,0, 294.2857163,0, 309.2857163,13.75359207, 324.2857163,27.50718414, 339.2857163,41.26077621, 354.2857163,54.41896598, 369.2857163,67.57715576, 384.2857163,80.73534554, 399.2857163,93.89353531, 414.2857163,107.5139404, 429.2857163,121.1343454, 444.2857163,134.7547505, 459.2857163,148.3751556, 471.3085446,168.3751556, 483.3313729,188.3751556, 495.3542012,208.3751556, 500,216.1034517, 495.1556514,186.1034517, 490.3113028,156.1034517, 485.4669542,126.1034517, 484.6428582,121.0000038, 478.9757974,91.00000381, 473.3087367,61.00000381, 467.6416759,31.00000381, 461.9746152,1.000003815, 451.3507465,21.00000381, 440.7268778,41.00000381, 430.1030091,61.00000381, 419.4791404,81.00000381, 405.3576996,101.0000038, 391.2362588,121.0000038, 377.1148181,141.0000038, 362.9933773,161.0000038, 340.4933773,175.8143234, 317.9933773,190.6286431, 295.4933773,205.4429627, 272.9933773,220.2572823, 250.4933773,235.071602, 227.9933773,249.8859216, 205.4933773,264.7002412, 182.9933773,279.5145608, 167.9933773,264.6989547, 152.9933773,249.8833486, 137.9933773,235.0677425, 122.9933773,220.2521364, 107.9933773,205.4365302, 92.99337729,190.6209241, 77.99337729,175.805318, 62.99337729,160.9897119, 47.99337729,146.1741058, 32.99337729,131.3584996, 17.99337729,116.5428935, 2.99337729,101.7272874, -12.00662271,86.91168128, -27.00662271,72.09607516, -42.00662271,57.28046904, -57.00662271,42.46486291, -72.00662271,27.64925679, -87.00662271,12.83365067, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -100,0, -95,20, -90,40, -85,60, -80,80, -65,69.57559682, -50,59.15119363, -35,48.72679045, -20,38.30238727, -10.36259653,58.30238727, -0.725193055,78.30238727, 8.912210418,98.30238727, 18.54961389,118.3023873, 33.54961389,136.5130551, 48.54961389,154.723723, 63.54961389,172.9343909, 78.54961389,191.1450587, 93.54961389,202.0341084, 108.5496139,212.9231581, 123.5496139,223.8122078, 138.5496139,234.7012576, 152.7632138,254.7012576, 166.9768136,274.7012576, 181.1904135,294.7012576, 195.4040134,314.7012576, 192.4853534,334.7012576, 189.5666933,354.7012576, 186.6480333,374.7012576, 183.7293732,394.7012576, 197.0213322,364.7012576, 210.3132912,334.7012576, 223.6052502,304.7012576, 236.8972092,274.7012576, 247.5641661,244.7012576, 258.2311231,214.7012576, 268.89808,184.7012576, 279.565037,154.7012576, 288.6806184,124.7012576, 297.7961999,94.70125755, 306.9117813,64.70125755, 316.0273628,34.70125755, 331.0273628,16.12174453, 346.0273628,-2.457768488, 361.0273628,-21.03728151, 376.0273628,-39.61679453, 386.3473751,-59.61679453, 396.6673875,-79.61679453, 406.9873998,-99.61679453, 417.3074121,-119.6167945, 421.5493666,-139.6167945, 425.791321,-159.6167945, 430.0332755,-179.6167945, 434.27523,-199.6167945, 437.1640632,-219.6167945, 440.0528964,-239.6167945, 442.9417296,-259.6167945, 445.8305628,-279.6167945, 444.4491871,-299.6167945, 443.0678113,-319.6167945, 441.6864355,-339.6167945, 440.3050597,-359.6167945, 426.2857148,-362.4827587, 426.2857148,-362.4827587, 426.2857148,-362.4827587, 426.2857148,-362.4827587, 428.000004,-343.3448319, 428.000004,-343.3448319, 428.000004,-343.3448319, 428.000004,-343.3448319, 405.500004,-334.4289968, 383.000004,-325.5131618, 368.9999968,-319.9655282, 368.9999968,-319.9655282, 383.9999968,-324.1034592, 398.9999968,-328.2413902, 413.9999968,-332.3793212, 428.9999968,-336.5172522, 443.9999968,-335.4754851, 458.9999968,-334.4337179, 473.9999968,-333.3919508, 488.9999968,-332.3501837, 493.1407847,-302.3501837, 497.2815727,-272.3501837, 500,-252.6551839, 500,-252.6551839, 500,-252.6551839, 477.5,-240.1717015, 455,-227.6882191, 432.5,-215.2047368, 410,-202.7212544, 424.5603281,-222.7212544, 439.1206562,-242.7212544, 453.6809843,-262.7212544, 431.1809843,-249.6917724, 408.6809843,-236.6622904, 386.1809843,-223.6328084, 363.6809843,-210.6033264, 341.1809843,-197.5738443, 318.6809843,-184.5443623, 296.1809843,-171.5148803, 273.6809843,-158.4853983, 251.1809843,-145.4559163, 228.6809843,-132.4264343, 206.1809843,-119.3969523, 183.6809843,-106.3674703, 161.1809843,-93.33798826, 138.6809843,-80.30850625, 116.1809843,-67.27902424, 93.68098434,-54.24954223, 71.18098434,-41.22006022, 48.68098434,-28.19057821, 26.18098434,-15.1610962, 3.680984342,-2.131614189, 18.68098434,-17.8597511, 33.68098434,-33.58788802, 48.68098434,-49.31602493, 63.68098434,-65.04416184, 78.68098434,-70.40830066, 93.68098434,-75.77243949, 108.6809843,-81.13657831, 123.6809843,-86.50071713, 138.6809843,-87.0437837, 153.6809843,-87.58685027, 168.6809843,-88.12991683, 183.6809843,-88.6729834, 197.4957442,-108.6729834, 211.3105041,-128.6729834, 225.125264,-148.6729834, 238.9400239,-168.6729834, 253.9400239,-180.0176486, 268.9400239,-191.3623138, 283.9400239,-202.706979, 298.9400239,-214.0516441, 313.9400239,-226.7655829, 328.9400239,-239.4795217, 343.9400239,-252.1934605, 358.9400239,-264.9073993, 373.5234866,-284.9073993, 388.1069493,-304.9073993, 402.690412,-324.9073993, 417.2738747,-344.9073993, 432.2738747,-364.2567814, 447.2738747,-383.6061635, 462.2738747,-402.9555457, 477.2738747,-422.3049278, 490.42666,-442.3049278, 500,-456.8620573, 500,-456.8620573, 500,-456.8620573, 500,-426.8620573, 500,-396.8620573, 500,-366.8620573, 500,-336.8620573, 488.3467731,-307.2069004, 488.3467731,-307.2069004, 488.3467731,-307.2069004, 488.3467731,-307.2069004, 465.8467731,-284.3459816, 457.5307273,-275.8965396, 457.5307273,-275.8965396, 457.5307273,-275.8965396, 439.6861456,-245.8965396, 421.8415638,-215.8965396, 411.5040228,-198.5172457, 411.5040228,-198.5172457, 389.0040228,-183.4053028, 380.6994304,-177.8275905, 380.6994304,-177.8275905, 380.6994304,-177.8275905, 368.8597828,-147.8275905, 357.0201353,-117.8275905, 345.1804877,-87.8275905, 339.2606623,-72.82758633, 316.7606623,-58.01752378, 311.9143801,-54.82757965, 311.9143801,-54.82757965, 311.9143801,-54.82757965, 289.4143801,-47.66341546, 287.0059455,-46.89655232, 287.0059455,-46.89655232, 287.0059455,-46.89655232, 264.5059455,-35.32376532, 264.4128063,-35.27585957, 264.4128063,-35.27585957, 264.4128063,-35.27585957, 258.4541283,-15.27585957, 252.4954502,4.724140434, 246.5367721,24.72414043, 241.6568213,41.1034472, 223.460356,61.1034472, 222.8328917,61.79310238, 222.8328917,61.79310238, 222.8328917,61.79310238, 205.6047825,72.4137931, 205.6047825,72.4137931, 205.6047825,72.4137931, 205.6047825,72.4137931, 189.9045674,62.34482866, 189.9045674,62.34482866, 189.9045674,62.34482866, 189.9045674,62.34482866, 175.4181356,59.65517271, 175.4181356,59.65517271, 175.4181356,59.65517271, 175.4181356,59.65517271, 167.1127247,79.65517271, 158.8073137,99.65517271, 158.2345267,101.0344831, 158.2345267,101.0344831, 148.801537,71.03448306, 146.069223,62.34482866, 146.069223,62.34482866, 146.069223,62.34482866, 134.9431953,53.27585879, 134.9431953,53.27585879, 134.9431953,53.27585879, 134.9431953,53.27585879, 130.6548491,73.27585879, 126.3665029,93.27585879, 124.7029196,101.0344831, 124.7029196,101.0344831, 120.0656178,71.03448306, 115.5242603,41.65517349, 115.5242603,41.65517349, 115.5242603,41.65517349, 112.2134692,61.65517349, 108.902678,81.65517349, 105.5918869,101.6551735, 102.8804637,118.0344803, 80.38046366,92.22029055, 57.88046366,66.40610085, 35.38046366,40.59191114, 12.88046366,14.77772144, 0,0, 0,0, 0,0, 0,0, 0,-20, 0,-40, 0,-60, 0,-80, 0,-100, 0,-120, 0,-140, 0,-160, 0,-180, 0,-200, 0,-220, 0,-240, 0,-260, 0,-280, 0,-300, 0,-320, 0,-340, 0,-360, 0,-380, 0,-400, 0,-420, 0,-440, 0,-460, 0,-480, 0,-500, 0,-520, 0,-540, 0,-560, 0,-530, 0,-509.6208569, 0,-509.6208569, 0,-509.6208569, 0,-479.6208569, 0,-449.6208569, 0,-419.6208569, 0,-404.5133644, 0,-400, 0,-400, 0,-400, 0,-400, 0,-400, 0,-400, 0,-400, 0,-370, 0,-340, 0,-310, 0,-280, 0,-250, 0,-220, 0,-190, 0,-160, 0,-130, 0,-100, 0,-70, 0,-40, 0,-10, 0,0};

static unsigned int indexxx = 0;
void test(void)
{
	if (indexxx < 460) {
        set_robot_speed(data_v[indexxx][0],data_v[indexxx][1]);
		indexxx++;
    }
	// else
	// 	indexxx = 0;
}

void encoderClean(const std_msgs::Empty& cmd_msg){
	unsigned char tmp_buf[1];
	tmp_buf[0] = 0x01;
	send_to_robot(0x56,tmp_buf,1);
}


unsigned char buf_tmp[2048];
int read_bytes = 0;
unsigned char buffer[2048];
unsigned int data_index = 0;
int count_num = 0;

/*
tmp_buf[2] = 0x00;
tmp_buf[3] = HEADER;
tmp_buf[4] = len + 2;
tmp_buf[5] = 0xF8;
tmp_buf[6] = cmd;
for (int i = 0; i < len; i++)
tmp_buf[7 + i] = data[i];
tmp_buf[7 + len] = robot_checksum(&tmp_buf[4],len + 3);
0x00 0x00 0x7a 0x04 0xf8 0xxx 0x00 0x00   0xyy 0x00

i = 2

2 + 7 == 9

*/

#define MIN_FRAME_LEN  6
// 移除数组的某个索引之前包括该索引的数据
void rm_data(unsigned char* buf,unsigned int *len, unsigned int idx)
{
    if (idx == 0) return;
    for (int i = 0; i < *len - idx - 1; i++)
      buf[i] = buf[idx + i + 1];

    *len -= (idx + 1);
}

void data_cache(unsigned char *buf, unsigned int len)
{
      unsigned int  i = 0;
      unsigned char get_frame = 0;
      unsigned int  rm_idx = 0;
	  //  缓存数据
      for (i = 0; i < len; i++)
            buffer[data_index++] = buf[i];
     i = 0;
     // 寻找完整帧
      while(1) {
          if (data_index - i > MIN_FRAME_LEN) {         /* 数据量至少满足最小帧长度 */
                // 存在疑似完整帧
                if ((buffer[i] == HEADER) && (buffer[i + 1] + 3 <= data_index - i)){   /* 根据帧长度信息进一步判断 */
    		        unsigned int frame_len = buffer[i + 1] + 3;
    		        if (buffer[i + frame_len - 1] == robot_checksum(&buffer[i + 1], buffer[i + 1] + 1)) {
    		            frame_decode(&buffer[i]);
    		        } else {
    		            printf("checksum err\n");
    		        }
                    rm_idx = i + frame_len - 1;
    		        // rm_data(buffer, &data_index, i + frame_len - 1);
                    get_frame = 1;
                }
            } else {
                rm_data(buffer, &data_index, rm_idx);
                return;
            }


            i++;
            if (get_frame == 1) {
                get_frame = 0;
                i = rm_idx + 1;
            }
        }
}

int main (int argc, char** argv){
    ros::init(argc, argv, "serial_node");
    ros::NodeHandle nh;
    ros::Subscriber encoder_sub=nh.subscribe("encoder_reset",1,encoderClean);
    ros::Subscriber v_sub = nh.subscribe("smoother_cmd_vel", 1, v_callback);

    ros::Publisher sensor_pub = nh.advertise<xunjian_nav::Sensor>("encoder", 50);
    ros::Publisher range_pub=nh.advertise<xunjian_nav::Ultrasound>("range_dist",50);

    msg.yawAngle=0.0;
    msg.leftEncoder=0;
    msg.rightEncoder=0;

    try
    {
        ser.setPort("/dev/xunjian_base");
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

    ros::Rate loop_rate(20);
    while(ros::ok()){
	ros::spinOnce();//check subscribed topic and process

        read_bytes = ser.read(buf_tmp,ser.available());

        // read byte data from serial buffer
        // for (int i = 0; i < read_bytes; i++) {
        //     buffer[data_index] = buf_tmp[i];
        //     if (buffer[0] != HEADER) {
        //         data_index = 0;
        //     } else {
        //         data_index++;
        //         if ((data_index >= 2) && (data_index >= buffer[1] + 3)) {
        //             if (buffer[data_index - 1] == robot_checksum(&buffer[1], buffer[1] + 1)) {
        //                 frame_decode(buffer);
        //             } else {
			  //                   printf("checksum err\n");
		    //             }
        //             data_index = 0;
        //         }
        //     }
        // }
        data_cache(buf_tmp,read_bytes);

    	if (read_bytes == 0) {
    		printf("no data\n");
    	}

	//test();
        range_msg.ultra_1=ultra_info.ultra_1;
        range_msg.ultra_2=ultra_info.ultra_2;
        range_msg.ultra_3=ultra_info.ultra_3;
        range_msg.ultra_4=ultra_info.ultra_4;
        range_msg.ultra_5=ultra_info.ultra_5;
        range_msg.ultra_6=ultra_info.ultra_6;

	//static unsigned long tm_cnt = 0;
	//tm_cnt++;
	//if ((tm_cnt % 5) == 0){
	//	tm_cnt = 0;
        	sensor_pub.publish(msg);
        	range_pub.publish(range_msg);
	//}

        loop_rate.sleep();

    }
}
