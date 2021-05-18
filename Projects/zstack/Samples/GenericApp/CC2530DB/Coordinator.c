/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* 文件名  ： Coordinator
* 作者    ： pangpeiyang
* 版本    ： V1.0.0
* 时间    ： 2021/5/18
* 简要    ： 协调器 
********************************************************************
* 副本
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
#include "OnBoard.h"
#endif

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
//此处为若干的头文件定义
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] = 
{
  GENERICAPP_CLUSTERID
};
const SimpleDescriptionFormat_t GenericApp_SimpleDesc = 
{
  GENERICAPP_ENDPOINT,
  GENERICAPP_PROFID,
  GENERICAPP_DEVICEID,
  GENERICAPP_DEVICE_VERSION,
  GENERICAPP_FLAGS,
  GENERICAPP_MAX_CLUSTERS,
  (cId_t *)GenericApp_ClusterList,
  0,
  (cId_t *)NULL
};
//ZigBee设备节点，设备描述。
endPointDesc_t GenericApp_epDesc;//节点描述符
byte GenericApp_TaskID;// 任务优先级
byte GenericApp_TransID;// 数据发送序列
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名： GenericApp_Init
* 作者：pangpeiyang
* 参数：byte task_id
* 返回：void
* 描述：任务初始化
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pack );//消息处理函数GenericApp_MessageMSGCB
void GenericApp_SendTheMessage( void );//数据发送函数GenericApp_SendTheMessage
void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID            = task_id;//初始化任务优先级
  GenericApp_TransID           = 0;//将发送数据包的序列号初始为0，每发送一个数据包，序号自动加1
  GenericApp_epDesc.endPoint   = GENERICAPP_ENDPOINT;//节点描述符初始化
  GenericApp_epDesc.task_id    = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc = 
    (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;//节点描述符初始化
  afRegister( &GenericApp_epDesc );//注册节点描述符，注册后才能使用OSAL提供的系统服务
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名：GenericApp_ProcessEvent
* 作者：pangpeiyang
* 参数：byte task_id,UNIT16 event
* 返回：UINT16
* 描述：消息处理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;//定义一个指向接受消息结构体指针MSGpkt
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID );//从消息队列接受消息，包含无线数据包
    while ( MSGpkt )
    {
      switch( MSGpkt->hdr.event )
      {
      case AF_INCOMING_MSG_CMD://对收到的消息进行判断
        GenericApp_MessageMSGCB( MSGpkt ); //可修改该函数实现形式
        break;
      default:
        break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );//释放存储空间
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
        ( GenericApp_TaskID );//进行下一消息的处理
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名：GenericApp_MessageMSGCB
* 作者：pangpeiyang
* 参数：afIncomingMSGPacket_t *pkt
* 返回：void
* 描述：数据处理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  unsigned char buffer[4] = " ";//定义一个数组
  switch ( pkt->clusterId )
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(buffer,pkt->cmd.Data,3);//将收到的数据拷贝到缓冲区buffer中
    if((buffer[0] == 'L') || (buffer[1] == 'E') || (buffer[2] == 'D'))//判断接受到的字符
    {
      HalLedBlink(HAL_LED_2,0,50,500);//如果是LED执行此句，使LED2闪烁
    }
    else
    {
      HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);//不是LED则点亮LED2
    }
    break;
  }
}