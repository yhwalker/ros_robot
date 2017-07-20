// Generated by gencpp from file xunjian_nav/Sensor.msg
// DO NOT EDIT!


#ifndef XUNJIAN_NAV_MESSAGE_SENSOR_H
#define XUNJIAN_NAV_MESSAGE_SENSOR_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace xunjian_nav
{
template <class ContainerAllocator>
struct Sensor_
{
  typedef Sensor_<ContainerAllocator> Type;

  Sensor_()
    : yawAngle(0.0)
    , leftEncoder(0)
    , rightEncoder(0)
    , vx(0)
    , w(0)  {
    }
  Sensor_(const ContainerAllocator& _alloc)
    : yawAngle(0.0)
    , leftEncoder(0)
    , rightEncoder(0)
    , vx(0)
    , w(0)  {
  (void)_alloc;
    }



   typedef double _yawAngle_type;
  _yawAngle_type yawAngle;

   typedef int32_t _leftEncoder_type;
  _leftEncoder_type leftEncoder;

   typedef int32_t _rightEncoder_type;
  _rightEncoder_type rightEncoder;

   typedef int32_t _vx_type;
  _vx_type vx;

   typedef int32_t _w_type;
  _w_type w;




  typedef boost::shared_ptr< ::xunjian_nav::Sensor_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::xunjian_nav::Sensor_<ContainerAllocator> const> ConstPtr;

}; // struct Sensor_

typedef ::xunjian_nav::Sensor_<std::allocator<void> > Sensor;

typedef boost::shared_ptr< ::xunjian_nav::Sensor > SensorPtr;
typedef boost::shared_ptr< ::xunjian_nav::Sensor const> SensorConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::xunjian_nav::Sensor_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::xunjian_nav::Sensor_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace xunjian_nav

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'xunjian_nav': ['/home/exbot/ros/src/xunjian_nav/msg'], 'std_msgs': ['/opt/ros/indigo/share/std_msgs/cmake/../msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::xunjian_nav::Sensor_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::xunjian_nav::Sensor_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::xunjian_nav::Sensor_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::xunjian_nav::Sensor_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::xunjian_nav::Sensor_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::xunjian_nav::Sensor_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::xunjian_nav::Sensor_<ContainerAllocator> >
{
  static const char* value()
  {
    return "930310bf8f953403d26333dc05df3fb6";
  }

  static const char* value(const ::xunjian_nav::Sensor_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x930310bf8f953403ULL;
  static const uint64_t static_value2 = 0xd26333dc05df3fb6ULL;
};

template<class ContainerAllocator>
struct DataType< ::xunjian_nav::Sensor_<ContainerAllocator> >
{
  static const char* value()
  {
    return "xunjian_nav/Sensor";
  }

  static const char* value(const ::xunjian_nav::Sensor_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::xunjian_nav::Sensor_<ContainerAllocator> >
{
  static const char* value()
  {
    return "float64 yawAngle\n\
int32   leftEncoder\n\
int32   rightEncoder\n\
int32   vx\n\
int32   w\n\
";
  }

  static const char* value(const ::xunjian_nav::Sensor_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::xunjian_nav::Sensor_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.yawAngle);
      stream.next(m.leftEncoder);
      stream.next(m.rightEncoder);
      stream.next(m.vx);
      stream.next(m.w);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct Sensor_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::xunjian_nav::Sensor_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::xunjian_nav::Sensor_<ContainerAllocator>& v)
  {
    s << indent << "yawAngle: ";
    Printer<double>::stream(s, indent + "  ", v.yawAngle);
    s << indent << "leftEncoder: ";
    Printer<int32_t>::stream(s, indent + "  ", v.leftEncoder);
    s << indent << "rightEncoder: ";
    Printer<int32_t>::stream(s, indent + "  ", v.rightEncoder);
    s << indent << "vx: ";
    Printer<int32_t>::stream(s, indent + "  ", v.vx);
    s << indent << "w: ";
    Printer<int32_t>::stream(s, indent + "  ", v.w);
  }
};

} // namespace message_operations
} // namespace ros

#endif // XUNJIAN_NAV_MESSAGE_SENSOR_H