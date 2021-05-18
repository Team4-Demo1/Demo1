/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* �ļ���  �� Coordinator
* ����    �� pangpeiyang
* �汾    �� V1.0.0
* ʱ��    �� 2021/5/18
* ��Ҫ    �� Э���� 
********************************************************************
* ����
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
//�˴�Ϊ���ɵ�ͷ�ļ�����
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
//ZigBee�豸�ڵ㣬�豸������
endPointDesc_t GenericApp_epDesc;//�ڵ�������
byte GenericApp_TaskID;// �������ȼ�
byte GenericApp_TransID;// ���ݷ�������
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* �������� GenericApp_Init
* ���ߣ�pangpeiyang
* ������byte task_id
* ���أ�void
* �����������ʼ��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pack );//��Ϣ������GenericApp_MessageMSGCB
void GenericApp_SendTheMessage( void );//���ݷ��ͺ���GenericApp_SendTheMessage
void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID            = task_id;//��ʼ���������ȼ�
  GenericApp_TransID           = 0;//���������ݰ������кų�ʼΪ0��ÿ����һ�����ݰ�������Զ���1
  GenericApp_epDesc.endPoint   = GENERICAPP_ENDPOINT;//�ڵ���������ʼ��
  GenericApp_epDesc.task_id    = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc = 
    (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;//�ڵ���������ʼ��
  afRegister( &GenericApp_epDesc );//ע��ڵ���������ע������ʹ��OSAL�ṩ��ϵͳ����
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
  afIncomingMSGPacket_t *MSGpkt;//����һ��ָ�������Ϣ�ṹ��ָ��MSGpkt
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID );//����Ϣ���н�����Ϣ�������������ݰ�
    while ( MSGpkt )
    {
      switch( MSGpkt->hdr.event )
      {
      case AF_INCOMING_MSG_CMD://���յ�����Ϣ�����ж�
        GenericApp_MessageMSGCB( MSGpkt ); //���޸ĸú���ʵ����ʽ
        break;
      default:
        break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt );//�ͷŴ洢�ռ�
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
        ( GenericApp_TaskID );//������һ��Ϣ�Ĵ���
    }
    return (events ^ SYS_EVENT_MSG);
  }
  return 0;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_MessageMSGCB
* ���ߣ�pangpeiyang
* ������afIncomingMSGPacket_t *pkt
* ���أ�void
* ���������ݴ���
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  unsigned char buffer[4] = " ";//����һ������
  switch ( pkt->clusterId )
  {
  case GENERICAPP_CLUSTERID:
    osal_memcpy(buffer,pkt->cmd.Data,3);//���յ������ݿ�����������buffer��
    if((buffer[0] == 'L') || (buffer[1] == 'E') || (buffer[2] == 'D'))//�жϽ��ܵ����ַ�
    {
      HalLedBlink(HAL_LED_2,0,50,500);//�����LEDִ�д˾䣬ʹLED2��˸
    }
    else
    {
      HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);//����LED�����LED2
    }
    break;
  }
}