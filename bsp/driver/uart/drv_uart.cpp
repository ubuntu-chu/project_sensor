/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/  

#include    "drv_uart.h"

#if		0

/******************************************************************************
 *                            ���ļ��ڲ��궨��
******************************************************************************/

//�豸������
#define                        _UART_BUFFER_SIZE                (100)
//�豸״̬
#define                        _UART_STATUS_BUSY                (0)
#define                        _UART_STATUS_IDLE                (1)

//�Ƿ�֧��RS485
#define                        _RS485_DEVICE_SUPPORT            (1)
#if (_RS485_DEVICE_SUPPORT > 0)

//RS485�շ�����
#define                        _RS485_RECV_SEND_PORT            (PORTC)
#define                        _RS485_RECV_SEND_PIN             (6)
#define                        _RS485_SEND_STATUS_SWITCH()      C_SBI(_RS485_RECV_SEND_PORT, _RS485_RECV_SEND_PIN)  
#define                        _RS485_RECV_STATUS_SWITCH()      C_CBI(_RS485_RECV_SEND_PORT, _RS485_RECV_SEND_PIN)     

#endif

/******************************************************************************
 *                           ���ļ���̬��������
******************************************************************************/

static DeviceStatus_TYPE _HAL_UARTOpen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _HAL_UARTRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _HAL_UARTWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _HAL_UARTControl(pDeviceAbstract pdev, uint8 cmd, void *args);
static DeviceStatus_TYPE _HAL_UARTRxIndicate(pDeviceAbstract pdev, portSIZE_TYPE size);
static void _HAL_UARTWriteChar(uint8 c);

/******************************************************************************
 *                       ���ļ�������ľ�̬���ݽṹ
******************************************************************************/

static DeviceAbstractInfo st_DeviceAbstractInfo_UART CONST_PRIVILEGED = {
    
        //�豸����
        DEVICE_NAME_UART,
        //�豸����->�ַ��豸
        DEVICE_CLASS_CHAR,
        //�豸��ʶ->��д�豸
        DEVICE_FLAG_RDWR,
        //init
        NULL,
        //open
        _HAL_UARTOpen,
        //close
        NULL,
        //read
        _HAL_UARTRead, 
        //write
        _HAL_UARTWrite,
        //control
        _HAL_UARTControl,
        //rx_indicate
        _HAL_UARTRxIndicate,
        //tx_complete
        NULL,
        
    };

static DeviceAbstract  st_Device_UART = {
    
        //�豸��Ϣ
        &st_DeviceAbstractInfo_UART, 
    };

typedef struct
{
    uint32                                                      m_hw_base;
	uint32                                                      m_baudrate;

	//���ա���ȡ��
    uint16                                                      m_saveIndex, m_readIndex;
    uint8                                                       m_irqno;
    //�豸״̬
    uint8                                                       m_status;
    //�豸������
    uint8                                                       m_buffer[_UART_BUFFER_SIZE];
    //֡������ɣ�
    uint8                                                       m_frameDone;
    uint16                                                      m_frameLen;
    
}DeviceInfo_UART;

static DeviceInfo_UART   st_DeviceInfo_UART;

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸ע��                                                             
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                               
 *                                                                            
 *                                                                            
******************************************************************************/ 

portuBASE_TYPE __HAL_UARTRegister(void)
{
    return API_DeviceRegister(&st_Device_UART);
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸��                                                             
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/ 

static DeviceStatus_TYPE _HAL_UARTOpen(pDeviceAbstract pdev, uint16 oflag)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = &st_DeviceInfo_UART;
    
    //�ҽ��豸��Ϣ�ṹ��
    pdev->m_private                                             = (void *)pdeviceInfo_UART;
    //��ʼ���豸��Ϣ�ṹ���Ա
    pdeviceInfo_UART->m_saveIndex                               = 0;
    pdeviceInfo_UART->m_readIndex                               = 0;
    pdeviceInfo_UART->m_status                                  = _UART_STATUS_IDLE;
    //pdeviceInfo_UART->m_irqno                                   = ;
    //��ʼ���豸������
    memset((void *)(pdeviceInfo_UART->m_buffer), 0, _UART_BUFFER_SIZE);
    
    //�豸�Ĵ�����ʼ�� �˴�Ӧ��д�ɶ�̬��ʽ
    //RXD����Ϊ���� PD0   9600������
	DDRD 				                                       &= ~(1 << 0);
	UCSRB 					                                    = 0x00;                 //disable while setting baud rate
	UCSRA					                                    = 0x00;
	UCSRC 					                                    = (1 << URSEL) | 0x06;
    
	UBRRL                                                       = 0x20;                 //set baud rate lo
	UBRRH                                                       = 0x00;                 //set baud rate hi
	UCSRB 					                                    = 0x98;
    
    #if (_RS485_DEVICE_SUPPORT > 0)
    {
        //���豸ʱ ʹRS485���ڽ��շ�ʽ
        _RS485_RECV_STATUS_SWITCH();
    }
    #endif
    
    return DEVICE_OK;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸��ȡ                                                          
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static portSIZE_TYPE _HAL_UARTRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
    uint8               *pbuffer                                = (uint8 *)buffer;
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    portCPSR_TYPE        level;
    
    //�жϽ��� DMA���շ�ʽ��֧��
    if (pdev->m_openFlag & DEVICE_OFLAG_DMA_RX)
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //������ �����д�����
        return 0;
    }
    
    //�жϷ�ʽ����
    if (pdev->m_openFlag & DEVICE_OFLAG_INT_RX)
    {
        level                                                   = portDISABLE_INTERRUPTS();
        memcpy((void *)buffer, pdeviceInfo_UART->m_buffer, size);
        pdeviceInfo_UART->m_frameDone                           = FALSE;
        memset(pdeviceInfo_UART->m_buffer, 0, pdeviceInfo_UART->m_frameLen);
        pdeviceInfo_UART->m_frameLen                            = 0;
        pdeviceInfo_UART->m_readIndex                           = 0;
        pdeviceInfo_UART->m_saveIndex                           = 0;
        portENABLE_INTERRUPTS(level);
        /*
        while (size)
		{
            if (pdeviceInfo_UART->m_readIndex != pdeviceInfo_UART->m_saveIndex)
			{
				*pbuffer                                        = pdeviceInfo_UART->m_buffer[pdeviceInfo_UART->m_readIndex];
				pdeviceInfo_UART->m_readIndex++;
				if (pdeviceInfo_UART->m_readIndex >= _UART_BUFFER_SIZE)
                {
					pdeviceInfo_UART->m_readIndex = 0;
                }
			}
			else
			{
				portENABLE_INTERRUPTS(level);
				break;
			}
            
			portENABLE_INTERRUPTS(level);
			pbuffer++; size--;
            
		}
        */
    }
    //��ѯ��ʽ����
    else
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //������ �����д�����
        return 0;
    }
    
    return (uint16)pbuffer - (uint16)buffer;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸д��                                                          
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static portSIZE_TYPE   _HAL_UARTWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size)
{
    uint8   *pbuffer;
    
    //�жϷ��� DMA���ͷ�ʽ��֧��
    if (pdev->m_openFlag & DEVICE_OFLAG_INT_TX)
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //������ �����д�����
        return 0;
    }
    
    pbuffer                                                     = (uint8 *)buffer;
    #if (_RS485_DEVICE_SUPPORT > 0)
    {
        //ʹRS485���ڷ��ͷ�ʽ
        _RS485_SEND_STATUS_SWITCH();
    }
    #endif
    
    //��ѯ����
    //��ģʽ��
    if (pdev->m_openFlag & DEVICE_OFLAG_STREAM)
    {
        while (size)
		{
			if (*pbuffer == '\n')
			{
				//����س��ַ�
                _HAL_UARTWriteChar('\r');
			}
            _HAL_UARTWriteChar(*pbuffer);
			pbuffer++;
			size--;
		}
    }
    //��ͨ��ʽ��
    else
    {
        while (size)
		{
            _HAL_UARTWriteChar(*pbuffer);
			pbuffer++;
			size--;
		}
    }
    
    #if (_RS485_DEVICE_SUPPORT > 0)
    {
        //ʹRS485���ڽ��շ�ʽ
        _RS485_RECV_STATUS_SWITCH();
    }
    #endif
    
    return (uint16)pbuffer - (uint16)buffer;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸����ָʾ����                                                          
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :  �ڴ˲���ʵ��Э�鲿��                                                            
 *                                                                            
 *                                                                            
******************************************************************************/

static DeviceStatus_TYPE _HAL_UARTRxIndicate(pDeviceAbstract pdev, portSIZE_TYPE size)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    
    //�ж��Ƿ���յ�һ֡����
    if (TRUE == pdeviceInfo_UART->m_frameDone)
    {
        //����巢�͵�������Ϣ
        API_RSDriveMsgAdd(INSTALL_SCHED_MSG, MSG_SCHED, SCHED_RECV_FRAME, 0);
    }
    
    
    return DEVICE_OK;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� : �豸����                                                          
 *                                                                           
 *  ��ڲ��� :                                                            
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static DeviceStatus_TYPE _HAL_UARTControl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    portCPSR_TYPE        level;
    
    switch (cmd)
    {
        case UART_CTRL_FRAME_INFO:
            
            level                                               = portDISABLE_INTERRUPTS(); 
            ((FrameInfo *)args)->m_done                         = pdeviceInfo_UART->m_frameDone;
            ((FrameInfo *)args)->m_len                          = pdeviceInfo_UART->m_frameLen;
            portENABLE_INTERRUPTS(level);
            break;
        
        case UART_CTRL_BUFFER_FLUSH:
            
            level                                               = portDISABLE_INTERRUPTS(); 
            memset((void *)(pdeviceInfo_UART->m_buffer), 0, _UART_BUFFER_SIZE);
            portENABLE_INTERRUPTS(level);
            break;
        
        case UART_CTRL_STATUS:
            
            pdeviceInfo_UART->m_status                          = _UART_STATUS_IDLE;
            break;
        
        case UART_CTRL_RECV_INT_START:
            
            UCSRB 					                            = 0x98;
            break;
        
        case UART_CTRL_RECV_INT_STOP:
            
            UCSRB 					                            = 0x00;
            break;
        
        case UART_CTRL_BAUD_SET:
            
            break;
        
        default:
            return DEVICE_ECMD_INVALID;
    }
    
    return DEVICE_OK;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/ 

static void _HAL_UARTWriteChar(uint8 c)
{
	UDR    							                            = c;
	
	do{
	
	    ;
	}while (C_BIT_IS_CLEAR(UCSRA, UDRE));
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :  ���ڽ����ж�                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :  Ŀǰ����;�����������ָ��Ĺ���                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static void _HAL_UARTRecv(pDeviceAbstract pdev, uint8 recvData)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    FP_pfrx_indicate    *rx_indicate;
    
    //�豸���ڿ���״̬ʱ ��������
    if (_UART_STATUS_IDLE != pdeviceInfo_UART->m_status)
    {
        return;
    }
    
    pdeviceInfo_UART->m_buffer[pdeviceInfo_UART->m_saveIndex]  = recvData;
    pdeviceInfo_UART->m_saveIndex++;
    if (pdeviceInfo_UART->m_saveIndex > _UART_BUFFER_SIZE)
    {
        pdeviceInfo_UART->m_saveIndex                           = 0;
    }
    //�жϻ������Ƿ�����
    if (pdeviceInfo_UART->m_saveIndex == pdeviceInfo_UART->m_readIndex)
    {
        pdeviceInfo_UART->m_readIndex++;
        if (pdeviceInfo_UART->m_readIndex > _UART_BUFFER_SIZE)
        {
            pdeviceInfo_UART->m_readIndex                       = 0;
        }
    }
    
    //�ж��Ƿ���յ������� LF ������һ֡�Ѿ����ճɹ�
    if ('\n' == recvData)
    {
        pdeviceInfo_UART->m_status                              = _UART_STATUS_BUSY;
        pdeviceInfo_UART->m_frameDone                           = TRUE;
        pdeviceInfo_UART->m_frameLen                            = pdeviceInfo_UART->m_saveIndex - pdeviceInfo_UART->m_readIndex;
    }
    //�ж���Ҫ���ý���ָʾ����
    if (DEVICE_OK == API_DeviceRxIndicatePtrGet(pdev, &rx_indicate))
    {
        //���յ�һ���ַ�
        rx_indicate(pdev, 1);
    }
}

SIGNAL(SIG_UART_RECV)
{
	//���չ������ж��ǹرյ�
    //���ý��պ���
    _HAL_UARTRecv(&st_Device_UART, UDR);
}

void UartSend(void)
{
    static uint8 seq = 0;
    
    uint8   aa[] = {0, 0xa1, 0x91, 0xa1, 9, '\n'};
    portuBASE_TYPE  i           = 0;
    
    if (1 == seq)
    {
        //aa[1]    = 0xa1;
        //aa[2]    = 0x91;
        seq      = 0;
    }
    else
    {
        seq      = 1;
    }
    aa[0]        = t_AppRunInfo.m_selfAddr;
    aa[4]        = API_UtilGetChkSum(aa, 4);
    
    for (;i < sizeof(aa); i++)
    {
        _HAL_UARTRecv(&st_Device_UART, aa[i]);
    }
}
#endif

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
