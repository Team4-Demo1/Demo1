/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 文件名  ：Coordinator
* 作者    ：wengzhen
* 版本    ：V0.0.1
* 时间    ：2021/5/18
* 描述    ：协调器的主函数
********************************************************************
* 副本
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* 头文件 ----------------------------------------------------------------*/
#include "oSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"
#if !defined ( WIN32 )
#include "OnBoard.h"
#endif
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
    GENERICAPP_CLUSTERID//在Coordinator.h文件中定义的宏，为了跟协议栈里面的数据保持一致
};
//描述一个ZigBee设备节点
const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
    GENERICAPP_ENDPOINT,              //  int Endpoint;
    GENERICAPP_PROFID,                //  uint16 AppProfId[2];
    GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
    GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
    GENERICAPP_FLAGS,                 //  int   AppFlags:4;
    GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
    (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
    0,
    (cId_t *)NULL                     //  byte *pAppInClusterList;
};


endPointDesc_t GenericApp_epDesc;   // 节点描述符
byte GenericApp_TaskID;             // 任务优先级
byte GenericApp_TransID;            // 数据发送序列号
/* 宏定义 ----------------------------------------------------------------*/
/* 结构体或枚举 ----------------------------------------------------------------*/
/* 内部函数声明 ----------------------------------------------------------------*/






/* 函数 ----------------------------------------------------------------*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 函数名  ：GenericApp
* 参数    ：byte  task_id
* 返回    ：void
* 作者    ：wengzhen
* 时间    ：2021/5/18
* 描述    ：任务初始化函数
----------------------------------------------------------------*/
void  GenericApp_Init(byte  task_id)
{
    GenericApp_TaskID = task_id;    // 初始化任务优先级
    GenericApp_TransID = 0;         //将发送数据序列号初始化为0
    
    GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
    GenericApp_epDesc.task_id = &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq = noLatencyReqs;   //对节点描述符进行初始化
    afRegister( &GenericApp_epDesc);                //使用afRegister对节点描述符进行注册
    
    
    
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 函数名  ：GenericApp_ProcessEvent
* 参数    ：byte task_id, UINT16 events
* 返回    ： UINT16
* 作者    ：wengzhen
* 时间    ：2021/5/18
* 描述    ：对接收数据的处理函数
----------------------------------------------------------------*/
UINT16  GenericApp_ProcessEvent(byte task_id, UINT16 events)
{
    afIncomingMSGPacket_t *MSGpkt;//定义一个接收信息结构体的指针
    if ( events & SYS_EVENT_MSG )
    {
        //osal_msg_receive函数从消息列队上接受信息
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        while ( MSGpkt )
        {
            switch( MSGpkt->hdr.event )
            {
                //判断接收信息是否为无线数据
            case AF_INCOMING_MSG_CMD:
                GenericApp_MessageMSGCB ( MSGpkt );//对数据进行处理
                break;
            default:
                break;
            }
            //释放消息向上的储存空间
            osal_msg_deallocate( (uint8 *)MSGpkt );
            
            //处理完第一个消息之后,接着处理下一个消息,直到完成
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
            
        } 
        //返回未完成的信息
        return (events ^ SYS_EVENT_MSG);
        
    }
    return 0;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 函数名  ：GenericApp_MessageMSGCB
* 参数    ：afIncomingMSGPacket_t *pkt
* 返回    ：void
* 作者    ：wengzhen
* 时间    ：2021/5/18
* 描述    ：消息处理函数
----------------------------------------------------------------*/
void GenericApp_MessageMSGCB ( afIncomingMSGPacket_t *pkt )
{
    unsigned char buffer[4] = "   ";
    switch ( pkt -> clusterId )
    {
    case GENERICAPP_CLUSTERID:
        //将接收数据复制到缓冲区
        osal_memcpy (buffer,pkt->cmd.Data,3);
        //判断接收数据是否为led
        if ( (buffer[0] =='L') || (buffer[1] -='E') || (buffer[2]=='D'))
        {
            HalLedBlink(HAL_LED_2,0,50,500);//结果为是的话,led2闪烁, HalLedBlink闪烁
        }
        else
        {
            HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);//结果为不是的话,led2点亮,HalLedBlink电路
        }
        break ;
        
    }
    
}