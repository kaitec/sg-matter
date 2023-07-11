#ifndef MOTOR_H_
#define MOTOR_H_
 
#define PSEVDO_HALL     132
#define DIV_ANGLE       15
#define MAX_DEF_T_STEPS 12

#define ON_LEVEL 1
#define DEAD_TIME 600
#define STEP_TIME_OUT (800+DEAD_TIME)
#define MIN_WORK_TIMEP 40000
#define MAX_WORK_TIMEP 240000
#define MAX_REST_TIMEP 3600000
#define Q_REST_TIMEP 150000

#define S_IO_CONTROL   1
#define JSON_EMPTY_CMD 0
#define FB_IN_MOTION   0 // Motor feedback GPIO level motor in motion

#define LIFT 1
#define ROLL 1
#define TILT 2

typedef enum {
	M_SUCCESS = 0x00,
	M_STOPED,
	M_DIR_UP,
	M_DIR_DOWN,
	M_DIR_UP_ERR,
	M_DIR_DOWN_ERR,
	M_DIR_GET
} motor_movement_t;

typedef struct {
	uint8_t angle_t;
	uint8_t perc_roll;
	uint32_t set_step;
	uint32_t set_t_step;
	uint32_t set_r_step;
	uint32_t current_step;
	uint32_t max_r_step;
	uint32_t max_t_step;
	uint8_t user_state;
	bool condition;
} motor_t;

typedef struct{
	uint16_t prot;
	uint16_t move;
	uint32_t rest;
	uint32_t work;
	uint32_t tilt;
}user_motor_time_t;

typedef struct{
	int max_eff;
	int obtain_ping;
	uint32_t search_inetrval;
}user_state_time_t;

typedef struct{
	user_motor_time_t b_h;
	user_state_time_t b_p;
}user_time_t;

//user_time_t blind_time;

typedef struct {
  uint8_t cmd_val;
  uint8_t cmd_len;
  uint8_t cmd;
} recivcmd_t; 

typedef enum {
	c_s_success = 0,
	c_s_halt,
	c_s_timeout,
	c_s_checking,

	c_s_unknow_status
} C_STATUS_CODE_t;

typedef struct {
	uint64_t stop;
	uint32_t res;
} AVG_MEM_t;

void motor_handler(void);
void motor_timer_function(void);
void motor_HallFb_function(void);
void motor_reset_variables(void);
void motor_load_position(void);
void motor_init(void);
void motor_reset(void);

void motor_set_blind(uint8_t len, uint8_t val);
uint8_t motor_get_lift(void);
uint8_t motor_get_tilt(void);

void enocean_roll_set(uint8_t cmd);
void enocean_tilt_set(uint8_t cmd);

extern bool motor_start;
extern bool motor_feedback;
extern uint32_t hall_ticks;
extern uint32_t esp_logi_ticks;
extern motor_t user_motor_var;
extern C_STATUS_CODE_t CS_RESP;
extern motor_movement_t motor_driver_state(motor_movement_t state);

#endif /* MOTOR_H_ */