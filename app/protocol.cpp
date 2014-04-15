#include "protocol.h"

#ifdef RAW_MAC

uint16_t    seq_id = 0;

uint16_t seq_id_get(void)
{
	return seq_id++;
}

protocol_mac::protocol_mac()
{

}

protocol_mac::~protocol_mac()
{

}

void protocol_mac::init(void)
{
	seq_id							= 0;
}

uint16 protocol_mac::pack(mac_frame_t* mac_frm_ptr, app_frame_t* app_frm_ptr, uint8_t* ptr, uint8_t* pData)
{
	uint16_t val;
	uint16_t val_len = 0x00;
	uint8_t *tmp_ptr = ptr;

	if(app_frm_ptr->len > 0x00){

		mac_frm_ptr->len = def_FRAME_MAC_LEN + def_FRAME_APP_HEAD_LEN + def_FRAME_PACK_IDEX_LEN 		// len assign
			+ def_FRAME_DATA_LEN + app_frm_ptr->len + def_FRAME_CK_LEN + def_FRAME_DELIMITER_LEN;

		val_len = def_FRAME_DELIMITER_LEN + def_FRAME_LEN_LEN + def_FRAME_MAC_LEN;
		memcpy(tmp_ptr, mac_frm_ptr, val_len);
		tmp_ptr = tmp_ptr + val_len;


		val_len = def_FRAME_APP_HEAD_LEN + def_FRAME_PACK_IDEX_LEN;
		memcpy(tmp_ptr, app_frm_ptr, val_len);
		tmp_ptr = tmp_ptr + val_len;

		val_len = def_FRAME_DATA_LEN;
		memcpy(tmp_ptr, (void *)&app_frm_ptr->len, val_len);
		tmp_ptr = tmp_ptr + val_len;

		val_len = app_frm_ptr->len;
		memcpy(tmp_ptr, pData, val_len);
		tmp_ptr = tmp_ptr + val_len;

		val_len =def_FRAME_DELIMITER_LEN + def_FRAME_LEN_LEN + def_FRAME_MAC_LEN  \
			+ def_FRAME_APP_HEAD_LEN + def_FRAME_PACK_IDEX_LEN \
			+ def_FRAME_DATA_LEN + app_frm_ptr->len;

	}else{
		mac_frm_ptr->len = def_FRAME_MAC_LEN + def_FRAME_APP_HEAD_LEN + def_FRAME_CK_LEN + def_FRAME_DELIMITER_LEN;

		val_len = def_FRAME_DELIMITER_LEN + def_FRAME_LEN_LEN + def_FRAME_MAC_LEN;
		memcpy(tmp_ptr, mac_frm_ptr, val_len);
		tmp_ptr = tmp_ptr + val_len;

		val_len = def_FRAME_FUN_LEN + def_FRAME_SUM_PACK_LEN;
		memcpy(tmp_ptr, app_frm_ptr, val_len);
		tmp_ptr = tmp_ptr + val_len;

		val_len = def_FRAME_DELIMITER_LEN + def_FRAME_LEN_LEN + def_FRAME_MAC_LEN  \
			+ def_FRAME_APP_HEAD_LEN;
	}

	val = sum16(ptr, val_len);																		// cal CK

	ST_WORD(tmp_ptr, val);
	tmp_ptr = tmp_ptr + def_FRAME_CK_LEN;

	ST_WORD(tmp_ptr, def_FRAME_END_delimiter);

	val_len = val_len + def_FRAME_CK_LEN + def_FRAME_DELIMITER_LEN;										// whole frame len

	return val_len;																			// whole frame len
}

int8	protocol_mac::unpack(uint8_t* pbuf, uint16 len, frame_ctl_t *pfrm_ctl)
{
	uint16		chk_sum;
    uint16		frame_len;

	//check head
	if ((pbuf[0] != (def_FRAME_delimiter & 0x00ff))
		&& (pbuf[1] != (def_FRAME_delimiter >> 8))){
		return -1;
	}
	//check tail
	if ((pbuf[len-2] != (def_FRAME_END_delimiter	 & 0x00ff))
		&& (pbuf[len-1] != (def_FRAME_END_delimiter >> 8))){
		return -2;
	}
    frame_len		    = LD_DWORD(&pbuf[2]);
    //check len
    if (frame_len != len-4){
        return -4;
    }
	//check sum
	chk_sum 	= LD_WORD(&pbuf[len-4]);
	if (chk_sum != sum16(pbuf, len - 4)){
		return -3;
	}
	//valid frame   for safe: byte by byte
	pfrm_ctl->mac_frm_ptr.delimiter_start		= LD_DWORD(&pbuf[0]);
    pfrm_ctl->mac_frm_ptr.len           = frame_len;
	pfrm_ctl->mac_frm_ptr.seq_id		= LD_DWORD(&pbuf[4]);
	pfrm_ctl->mac_frm_ptr.dev_adr		= LD_DWORD(&pbuf[12]);
	pfrm_ctl->mac_frm_ptr.sen_adr		= LD_DWORD(&pbuf[16]);
	pfrm_ctl->mac_frm_ptr.time 		= LD_DWORD(&pbuf[8]);
	//set sys time
	cpu_sys_time_set(pfrm_ctl->mac_frm_ptr.time);
    memcpy(&pfrm_ctl->mac_frm_ptr.ctl, &pbuf[6], 2);
	//pfrm_ctl->mac_frm_ptr.ctl		= (mac_frm_ctrl_t)LD_WORD(&pbuf[6]);
	pfrm_ctl->mac_frm_ptr.type		= pbuf[20];
	pfrm_ctl->app_frm_ptr.fun		= pbuf[21];
	pfrm_ctl->app_frm_ptr.sum		= pbuf[22];
	pfrm_ctl->app_frm_ptr.idex		= pbuf[23];
	pfrm_ctl->app_frm_ptr.len 		= LD_WORD(&pbuf[24]);
	pfrm_ctl->data_ptr 				= &pbuf[26];

	return 0;
}

void protocol_mac::frm_ctl_init(frame_ctl_t *pfrm_ctl, mac_frm_ctrl_t frm_ctl, uint8 total, uint8 index, uint8 func_code, uint8 *pbuf, uint16 len)
{
	memset(pfrm_ctl, 0, sizeof(frame_ctl_t));

	pfrm_ctl->mac_frm_ptr.delimiter_start = def_FRAME_delimiter;
	pfrm_ctl->mac_frm_ptr.seq_id = seq_id_get();

	pfrm_ctl->mac_frm_ptr.ctl		= frm_ctl;

	pfrm_ctl->mac_frm_ptr.time = cpu_sys_time_get();

	pfrm_ctl->mac_frm_ptr.dev_adr = c_modeinfo.dev_addr_get();
	pfrm_ctl->mac_frm_ptr.sen_adr = c_modeinfo.sensor_addr_get();

	pfrm_ctl->mac_frm_ptr.type 	= c_modeinfo.dev_type_get();
	pfrm_ctl->app_frm_ptr.fun 	= func_code;

	pfrm_ctl->app_frm_ptr.sum 	= total;
	pfrm_ctl->app_frm_ptr.idex 	= index;

	pfrm_ctl->app_frm_ptr.len 	= len;
	pfrm_ctl->data_ptr 			= (uint8_t *)pbuf;
}

void protocol_mac::mac_frm_ctrl_init(mac_frm_ctrl_t *pfrm_ctrl, uint8 ack, uint8 dir, uint8 ack_req, uint8 frm_type)
{
	memset(pfrm_ctrl, 0, sizeof(mac_frm_ctrl_t));
    pfrm_ctrl->ack_mask 		= ack;
    pfrm_ctrl->direction 		= dir;
	pfrm_ctrl->ack_req 			= ack_req;
	pfrm_ctrl->frm_type 		= frm_type;
}

class protocol_mac 	t_protocol_mac;

#endif

