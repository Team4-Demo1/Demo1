/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* �ļ���  �� Enddevice
* ����    �� pangpeiyang
* �汾    �� V1.0.0
* ʱ��    �� 2021/5/18
* ��Ҫ    �� �ն˽ڵ�  
********************************************************************
* ����
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/* ͷ�ļ� ----------------------------------------------------------------*/

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
endPointDesc_t GenericApp_epDesc;//�ڵ�������
byte GenericApp_TaskID;// �������ȼ�
byte GenericApp_TransID; // ���ݷ�������
devStates_t GenericApp_NwkState;//����ڵ�״̬
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );//��Ϣ������
void GenericApp_SendTheMessage( void );//���ݷ��ͺ���
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* �������� GenericApp_Init
* ���ߣ�pangpeiyang
* ������byte task_id
* ���أ�void
* �����������ʼ��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID             = task_id;//��ʼ���������ȼ�
  GenericApp_NwkState           =DEV_INIT;//��䣬��ʾδ���������������Լ�ȥ��������
  GenericApp_TransID            = 0;//���������ݰ������кų�ʼΪ0��ÿ����һ�����ݰ�������Զ���1
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;//�ڵ���������ʼ��
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = 
    (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;//�ڵ���������ʼ��
  afRegister( &GenericApp_epDesc );//�ڵ�������ע�ᣬע������ʹ��OSAL�ṩ��ϵͳ����
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_ProcessEvent
* ���ߣ�pangpeiyang
* ������byte task_id,UNIT16 event
* ���أ�UINT16
* ��������Ϣ����
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;//���������Ϣ�ṹ���ָ��
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );//����Ϣ���н�����Ϣ
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case ZDO_STATE_CHANGE://���յ�����Ϣ�����ж�
        GenericApp_NwkState=(devStates_t)(MSGpkt->hdr.status);//��ȡ�ڵ��豸����
        if(GenericApp_NwkState==DEV_END_DEVICE)//�Խڵ��豸���ͽ����жϣ�������ܶ˽ڵ�DEV_END_DEVICE��ʵ���������ݷ���
        {
          GenericApp_SendTheMessage();
        }
        break;
      default:
        break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );//�ͷŴ洢�ռ�
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );//������һ��Ϣ�Ĵ���
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_SendTheMessage
* ���ߣ�pangpeiyang
* ������void
* ���أ�void
* ���������ݷ���
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_SendTheMessage(void)
{
  unsigned char theMessageData[4]= "LED";//����һ�����飬��ŷ��͵�����
  afAddrType_t my_DstAddr;//���庯�����ڽ�������
  my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//���͵�ַ����Ϊ����
  my_DstAddr.endPoint = GENERICAPP_ENDPOINT;//��ʼ���˿�
  my_DstAddr.addr.shortAddr = 0x0000;//ָ��Э�����������ַ
  //�������ݷ��ͺ��������������ݵķ���
  AF_DataRequest(&my_DstAddr,&GenericApp_epDesc,GENERICAPP_CLUSTERID,3,theMessageData,&GenericApp_TransID,AF_DISCV_ROUTE,AF_DEFAULT_RADIUS);
  HalLedBlink(HAL_LED_2,0,50,500);//���÷�����0����˸����,����0��ʾ����һֱ��,���ǲ���,50�ǰٷ�֮��ʮ,���������һ��.500������,����0.5s.
}
