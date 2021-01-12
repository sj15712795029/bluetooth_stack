typedef struct
{
	uint16_t handle;
	uint8_t uuid_type;
	uint16_t uuid16;
	uint8_t uuid128[16];
	uint8_t *value;
	uint8_t read_enable;
	uint8_t write_enable;
}gatt_server_service_t;

err_t gatt_server_add_service(att_server_service_t *service,uint16_t service_num);

