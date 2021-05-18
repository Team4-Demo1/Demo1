/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* �ļ���  ��Coordinator
* ����    ��wengzhen
* �汾    ��V0.0.1
* ʱ��    ��2021/5/18
* ����    ��Э������������
********************************************************************
* ����
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* ͷ�ļ� ----------------------------------------------------------------*/
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
    GENERICAPP_CLUSTERID//��Coordinator.h�ļ��ж���ĺ꣬Ϊ�˸�Э��ջ��������ݱ���һ��
};
//����һ��ZigBee�豸�ڵ�
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


endPointDesc_t GenericApp_epDesc;   // �ڵ�������
byte GenericApp_TaskID;             // �������ȼ�
byte GenericApp_TransID;            // ���ݷ������к�
/* �궨�� ----------------------------------------------------------------*/
/* �ṹ���ö�� ----------------------------------------------------------------*/
/* �ڲ��������� ----------------------------------------------------------------*/






/* ���� ----------------------------------------------------------------*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* ������  ��GenericApp
* ����    ��byte  task_id
* ����    ��void
* ����    ��wengzhen
* ʱ��    ��2021/5/18
* ����    �������ʼ������
----------------------------------------------------------------*/
void  GenericApp_Init(byte  task_id)
{
    GenericApp_TaskID = task_id;    // ��ʼ���������ȼ�
    GenericApp_TransID = 0;         //�������������кų�ʼ��Ϊ0
    
    GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;
    GenericApp_epDesc.task_id = &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq = noLatencyReqs;   //�Խڵ����������г�ʼ��
    afRegister( &GenericApp_epDesc);                //ʹ��afRegister�Խڵ�����������ע��
    
    
    
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* ������  ��GenericApp_ProcessEvent
* ����    ��byte task_id, UINT16 events
* ����    �� UINT16
* ����    ��wengzhen
* ʱ��    ��2021/5/18
* ����    ���Խ������ݵĴ�����
----------------------------------------------------------------*/
UINT16  GenericApp_ProcessEvent(byte task_id, UINT16 events)
{
    afIncomingMSGPacket_t *MSGpkt;//����һ��������Ϣ�ṹ���ָ��
    if ( events & SYS_EVENT_MSG )
    {
        //osal_msg_receive��������Ϣ�ж��Ͻ�����Ϣ
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        while ( MSGpkt )
        {
            switch( MSGpkt->hdr.event )
            {
                //�жϽ�����Ϣ�Ƿ�Ϊ��������
            case AF_INCOMING_MSG_CMD:
                GenericApp_MessageMSGCB ( MSGpkt );//�����ݽ��д���
                break;
            default:
                break;
            }
            //�ͷ���Ϣ���ϵĴ���ռ�
            osal_msg_deallocate( (uint8 *)MSGpkt );
            
            //�������һ����Ϣ֮��,���Ŵ�����һ����Ϣ,ֱ�����
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
            
        } 
        //����δ��ɵ���Ϣ
        return (events ^ SYS_EVENT_MSG);
        
    }
    return 0;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* ������  ��GenericApp_MessageMSGCB
* ����    ��afIncomingMSGPacket_t *pkt
* ����    ��void
* ����    ��wengzhen
* ʱ��    ��2021/5/18
* ����    ����Ϣ������
----------------------------------------------------------------*/
void GenericApp_MessageMSGCB ( afIncomingMSGPacket_t *pkt )
{
    unsigned char buffer[4] = "   ";
    switch ( pkt -> clusterId )
    {
    case GENERICAPP_CLUSTERID:
        //���������ݸ��Ƶ�������
        osal_memcpy (buffer,pkt->cmd.Data,3);
        //�жϽ��������Ƿ�Ϊled
        if ( (buffer[0] =='L') || (buffer[1] -='E') || (buffer[2]=='D'))
        {
            HalLedBlink(HAL_LED_2,0,50,500);//���Ϊ�ǵĻ�,led2��˸, HalLedBlink��˸
        }
        else
        {
            HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);//���Ϊ���ǵĻ�,led2����,HalLedBlink��·
        }
        break ;
        
    }
    
}