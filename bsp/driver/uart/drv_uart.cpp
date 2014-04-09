/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/  

#include    "drv_uart.h"

#if		0

/******************************************************************************
 *                            本文件内部宏定义
******************************************************************************/

//设备缓冲区
#define                        _UART_BUFFER_SIZE                (100)
//设备状态
#define                        _UART_STATUS_BUSY                (0)
#define                        _UART_STATUS_IDLE                (1)

//是否支持RS485
#define                        _RS485_DEVICE_SUPPORT            (1)
#if (_RS485_DEVICE_SUPPORT > 0)

//RS485收发操作
#define                        _RS485_RECV_SEND_PORT            (PORTC)
#define                        _RS485_RECV_SEND_PIN             (6)
#define                        _RS485_SEND_STATUS_SWITCH()      C_SBI(_RS485_RECV_SEND_PORT, _RS485_RECV_SEND_PIN)  
#define                        _RS485_RECV_STATUS_SWITCH()      C_CBI(_RS485_RECV_SEND_PORT, _RS485_RECV_SEND_PIN)     

#endif

/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/

static DeviceStatus_TYPE _HAL_UARTOpen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _HAL_UARTRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _HAL_UARTWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _HAL_UARTControl(pDeviceAbstract pdev, uint8 cmd, void *args);
static DeviceStatus_TYPE _HAL_UARTRxIndicate(pDeviceAbstract pdev, portSIZE_TYPE size);
static void _HAL_UARTWriteChar(uint8 c);

/******************************************************************************
 *                       本文件所定义的静态数据结构
******************************************************************************/

static DeviceAbstractInfo st_DeviceAbstractInfo_UART CONST_PRIVILEGED = {
    
        //设备名字
        DEVICE_NAME_UART,
        //设备类型->字符设备
        DEVICE_CLASS_CHAR,
        //设备标识->读写设备
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
    
        //设备信息
        &st_DeviceAbstractInfo_UART, 
    };

typedef struct
{
    uint32                                                      m_hw_base;
	uint32                                                      m_baudrate;

	//接收、读取域
    uint16                                                      m_saveIndex, m_readIndex;
    uint8                                                       m_irqno;
    //设备状态
    uint8                                                       m_status;
    //设备缓冲区
    uint8                                                       m_buffer[_UART_BUFFER_SIZE];
    //帧接收完成？
    uint8                                                       m_frameDone;
    uint16                                                      m_frameLen;
    
}DeviceInfo_UART;

static DeviceInfo_UART   st_DeviceInfo_UART;

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备注册                                                             
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                               
 *                                                                            
 *                                                                            
******************************************************************************/ 

portuBASE_TYPE __HAL_UARTRegister(void)
{
    return API_DeviceRegister(&st_Device_UART);
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备打开                                                             
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/ 

static DeviceStatus_TYPE _HAL_UARTOpen(pDeviceAbstract pdev, uint16 oflag)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = &st_DeviceInfo_UART;
    
    //挂接设备信息结构体
    pdev->m_private                                             = (void *)pdeviceInfo_UART;
    //初始化设备信息结构体成员
    pdeviceInfo_UART->m_saveIndex                               = 0;
    pdeviceInfo_UART->m_readIndex                               = 0;
    pdeviceInfo_UART->m_status                                  = _UART_STATUS_IDLE;
    //pdeviceInfo_UART->m_irqno                                   = ;
    //初始化设备缓冲区
    memset((void *)(pdeviceInfo_UART->m_buffer), 0, _UART_BUFFER_SIZE);
    
    //设备寄存器初始化 此处应该写成多态形式
    //RXD配置为输入 PD0   9600波特率
	DDRD 				                                       &= ~(1 << 0);
	UCSRB 					                                    = 0x00;                 //disable while setting baud rate
	UCSRA					                                    = 0x00;
	UCSRC 					                                    = (1 << URSEL) | 0x06;
    
	UBRRL                                                       = 0x20;                 //set baud rate lo
	UBRRH                                                       = 0x00;                 //set baud rate hi
	UCSRB 					                                    = 0x98;
    
    #if (_RS485_DEVICE_SUPPORT > 0)
    {
        //打开设备时 使RS485处于接收方式
        _RS485_RECV_STATUS_SWITCH();
    }
    #endif
    
    return DEVICE_OK;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备读取                                                          
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static portSIZE_TYPE _HAL_UARTRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
    uint8               *pbuffer                                = (uint8 *)buffer;
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    portCPSR_TYPE        level;
    
    //中断接收 DMA接收方式不支持
    if (pdev->m_openFlag & DEVICE_OFLAG_DMA_RX)
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //返回零 代表有错误发生
        return 0;
    }
    
    //中断方式接收
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
    //查询方式接收
    else
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //返回零 代表有错误发生
        return 0;
    }
    
    return (uint16)pbuffer - (uint16)buffer;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备写入                                                          
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static portSIZE_TYPE   _HAL_UARTWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size)
{
    uint8   *pbuffer;
    
    //中断发送 DMA发送方式不支持
    if (pdev->m_openFlag & DEVICE_OFLAG_INT_TX)
    {
        API_DeviceErrorInfoSet(DEVICE_EOFLG_INVALID);
        //返回零 代表有错误发生
        return 0;
    }
    
    pbuffer                                                     = (uint8 *)buffer;
    #if (_RS485_DEVICE_SUPPORT > 0)
    {
        //使RS485处于发送方式
        _RS485_SEND_STATUS_SWITCH();
    }
    #endif
    
    //轮询发送
    //流模式打开
    if (pdev->m_openFlag & DEVICE_OFLAG_STREAM)
    {
        while (size)
		{
			if (*pbuffer == '\n')
			{
				//输出回车字符
                _HAL_UARTWriteChar('\r');
			}
            _HAL_UARTWriteChar(*pbuffer);
			pbuffer++;
			size--;
		}
    }
    //普通方式打开
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
        //使RS485处于接收方式
        _RS485_RECV_STATUS_SWITCH();
    }
    #endif
    
    return (uint16)pbuffer - (uint16)buffer;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备接收指示函数                                                          
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :  在此部分实现协议部分                                                            
 *                                                                            
 *                                                                            
******************************************************************************/

static DeviceStatus_TYPE _HAL_UARTRxIndicate(pDeviceAbstract pdev, portSIZE_TYPE size)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    
    //判断是否接收到一帧数据
    if (TRUE == pdeviceInfo_UART->m_frameDone)
    {
        //向面板发送调度器信息
        API_RSDriveMsgAdd(INSTALL_SCHED_MSG, MSG_SCHED, SCHED_RECV_FRAME, 0);
    }
    
    
    return DEVICE_OK;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 : 设备控制                                                          
 *                                                                           
 *  入口参数 :                                                            
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                              
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
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :                                                                
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
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :  串口接收中断                                                             
 *                                                                           
 *  入口参数 :                                                                
 *                                                                             
 *  出口参数 :                                                                
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 :  目前无用;连接任意多条指令的功能                                                              
 *                                                                            
 *                                                                            
******************************************************************************/

static void _HAL_UARTRecv(pDeviceAbstract pdev, uint8 recvData)
{
    DeviceInfo_UART     *pdeviceInfo_UART                       = (DeviceInfo_UART *)(pdev->m_private);
    FP_pfrx_indicate    *rx_indicate;
    
    //设备处于空闲状态时 接收数据
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
    //判断缓冲区是否已满
    if (pdeviceInfo_UART->m_saveIndex == pdeviceInfo_UART->m_readIndex)
    {
        pdeviceInfo_UART->m_readIndex++;
        if (pdeviceInfo_UART->m_readIndex > _UART_BUFFER_SIZE)
        {
            pdeviceInfo_UART->m_readIndex                       = 0;
        }
    }
    
    //判断是否接收到结束符 LF 完整的一帧已经接收成功
    if ('\n' == recvData)
    {
        pdeviceInfo_UART->m_status                              = _UART_STATUS_BUSY;
        pdeviceInfo_UART->m_frameDone                           = TRUE;
        pdeviceInfo_UART->m_frameLen                            = pdeviceInfo_UART->m_saveIndex - pdeviceInfo_UART->m_readIndex;
    }
    //判断需要调用接收指示函数
    if (DEVICE_OK == API_DeviceRxIndicatePtrGet(pdev, &rx_indicate))
    {
        //接收到一个字符
        rx_indicate(pdev, 1);
    }
}

SIGNAL(SIG_UART_RECV)
{
	//接收过程中中断是关闭的
    //调用接收函数
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
