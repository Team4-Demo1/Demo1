/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* 文件名  ： Enddevice
* 作者    ： pangpeiyang
* 版本    ： V1.0.0
* 时间    ： 2021/5/18
* 简要    ： 终端节点  
********************************************************************
* 副本
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/* 头文件 ----------------------------------------------------------------*/

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
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};
const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;
  0,
  (cId_t *)NULL,
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
};
endPointDesc_t GenericApp_epDesc;//节点描述符
byte GenericApp_TaskID;// 任务优先级
byte GenericApp_TransID; // 数据发送序列
devStates_t GenericApp_NwkState;//保存节点状态
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );//消息处理函数
void GenericApp_SendTheMessage( void );//数据发送函数
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名： GenericApp_Init
* 作者：pangpeiyang
* 参数：byte task_id
* 返回：void
* 描述：任务初始化
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID             = task_id;//初始化任务优先级
  GenericApp_NwkState           =DEV_INIT;//这句，表示未连上网，得由它自己去发现网络
  GenericApp_TransID            = 0;//将发送数据包的序列号初始为0，每发送一个数据包，序号自动加1
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;//节点描述符初始化
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = 
    (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;//节点描述符初始化
  afRegister( &GenericApp_epDesc );//节点描述符注册，注册后才能使用OSAL提供的系统服务
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
  afIncomingMSGPacket_t *MSGpkt;//定义接受消息结构体的指针
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );//从消息队列接受消息
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case ZDO_STATE_CHANGE://对收到的消息进行判断
        GenericApp_NwkState=(devStates_t)(MSGpkt->hdr.status);//读取节点设备类型
        if(GenericApp_NwkState==DEV_END_DEVICE)//对节点设备类型进行判断，如果是总端节点DEV_END_DEVICE，实现无线数据发送
        {
          GenericApp_SendTheMessage();
        }
        break;
      default:
        break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );//释放存储空间
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );//进行下一消息的处理
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名：GenericApp_SendTheMessage
* 作者：pangpeiyang
* 参数：void
* 返回：void
* 描述：数据发送
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_SendTheMessage(void)
{
  unsigned char theMessageData[4]= "LED";//定义一个数组，存放发送的数据
  afAddrType_t my_DstAddr;//定义函数用于接收数据
  my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//发送地址设置为单播
  my_DstAddr.endPoint = GENERICAPP_ENDPOINT;//初始化端口
  my_DstAddr.addr.shortAddr = 0x0000;//指定协调器的网络地址
  //调用数据发送函数进行无线数据的发送
  AF_DataRequest(&my_DstAddr,&GenericApp_epDesc,GENERICAPP_CLUSTERID,3,theMessageData,&GenericApp_TransID,AF_DISCV_ROUTE,AF_DEFAULT_RADIUS);
  HalLedBlink(HAL_LED_2,0,50,500);//调用方法，0是闪烁次数,但是0表示的是一直闪,不是不闪,50是百分之五十,就是亮灭各一半.500是周期,就是0.5s.
}
