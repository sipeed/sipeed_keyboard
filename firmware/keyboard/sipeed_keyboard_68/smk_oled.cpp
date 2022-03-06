//
// Created by hailin on 2022/3/6.
//

#include "include/smk_oled.h"
#include "hal_gpio.h"
#include "bflb_platform.h"
#include "smk_oled_font8.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <list>
extern "C"{
#include <smk_hid_protocol.h>
}

static bool g_OLED_Interface_inited= false;
class smk_oled_t{
public:
    smk_oled_t(gpio_pin_type sda=i2c_1_sda_pin,\
                gpio_pin_type scl=i2c_1_scl_pin,\
                gpio_pin_type rst=oled_rst_pin,\
                unsigned char addr=i2c_1_addr1)
                :sda_pin(sda),scl_pin(scl),rst_pin(rst),i2c_addr(addr)
                {}
    void init(){
        I2C_Init();
        OLED_Init();
    }
    void I2C_Init(){
        gpio_set_mode(sda_pin, GPIO_OUTPUT_PP_MODE);
        gpio_set_mode(scl_pin, GPIO_OUTPUT_PP_MODE);
        if(g_OLED_Interface_inited)
            return;
        g_OLED_Interface_inited = true;
        gpio_set_mode(rst_pin, GPIO_OUTPUT_PP_MODE);
        gpio_write(rst_pin, 0);
        vTaskDelay(100);
        gpio_write(rst_pin, 1);
        vTaskDelay(100);
    }
    void I2C_Start(void)
    {
        gpio_write(sda_pin, 1);
        gpio_write(scl_pin, 1);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
        gpio_write(sda_pin, 0);
        gpio_write(scl_pin, 0);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
    }
    void I2C_Stop(void)
    {
        gpio_write(scl_pin, 1);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
        gpio_write(scl_pin, 0);
        gpio_write(sda_pin, 1);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
    }
    void I2C_WaitAck(void) //测数据信号的电平
    {
        gpio_write(scl_pin, 1);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
        gpio_write(scl_pin, 0);
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
    }
    void I2C_Send_Byte(uint8_t dat)
    {
        uint8_t i;
        for (i = 0; i < 8; i++) {
            gpio_write(scl_pin, 0);   //将时钟信号设置为低电平
            if (dat & 0x80){                    //将dat的8位从最高位依次写入
                gpio_write(sda_pin, 1);
            } else {
                gpio_write(sda_pin, 0);
            }
            bflb_platform_delay_us(1000/OLED_I2C_KHz);
            gpio_write(scl_pin, 1); //将时钟信号设置为高电平
            bflb_platform_delay_us(1000/OLED_I2C_KHz);
            dat <<= 1;
        }
        gpio_write(scl_pin, 0); //将时钟信号设置为低电平
        bflb_platform_delay_us(1000/OLED_I2C_KHz);
    }
    void OLED_WR_Byte(uint8_t dat, uint8_t mode)
    {
        I2C_Start();
        I2C_Send_Byte(i2c_addr);
        I2C_WaitAck();
        if (mode) {
            I2C_Send_Byte(0x40);
        } else {
            I2C_Send_Byte(0x00);
        }
        I2C_WaitAck();
        I2C_Send_Byte(dat);
        I2C_WaitAck();
        I2C_Stop();
    }
    //反显函数
    void OLED_ColorTurn(uint8_t i)
    {
        if (i == 0) {
            OLED_WR_Byte(0xA6, OLED_CMD); //正常显示
        }
        if (i == 1) {
            OLED_WR_Byte(0xA7, OLED_CMD); //反色显示
        }
    }

//屏幕旋转180°
    void OLED_DisplayTurn(uint8_t i)
    {
        if (i == 0) {
            OLED_WR_Byte(0xC8, OLED_CMD); //正常显示
            OLED_WR_Byte(0xA1, OLED_CMD);
        }
        if (i == 1) {
            OLED_WR_Byte(0xC0, OLED_CMD); //反转显示
            OLED_WR_Byte(0xA0, OLED_CMD);
        }
    }

//开启OLED显示
    void OLED_DisPlay_On(void)
    {
        OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵使能
        OLED_WR_Byte(0x14, OLED_CMD); //关闭电荷泵
        OLED_WR_Byte(0xAF, OLED_CMD); //点亮屏幕
    }

//关闭OLED显示
    void OLED_DisPlay_Off(void)
    {
        OLED_WR_Byte(0x8D, OLED_CMD); //��ɱ�ʹ��
        OLED_WR_Byte(0x10, OLED_CMD); //�رյ�ɱ�
        OLED_WR_Byte(0xAF, OLED_CMD); //�ر���Ļ
    }

//更新显存到OLED
    void OLED_Refresh(void)
    {
        OLED_SetWindow_full();
        int i=sizeof(OLED_GRAM);
        unsigned char* data_p=OLED_GRAM;
        while(i--)
                OLED_WR_Byte(*(data_p++), OLED_DATA);
    }

//清屏函数
    void OLED_Clear(void)
    {
        memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
        OLED_Refresh();
    }

//画点
//x:0~69
//y:0~39
    void OLED_DrawPoint(uint8_t x, uint8_t y)
    {
        uint8_t i, m, n;
        i = y / 8;
        m = y % 8;
        n = 1 << m;
        OLED_GRAM[i*witdh+x] |= n;
    }

//清除一个点
//x:0~69
//y:0~29
    void OLED_ClearPoint(uint8_t x, uint8_t y)
    {
        uint8_t i, m, n;
        i = y / 8;
        m = y % 8;
        n = 1 << m;
        OLED_GRAM[i*witdh+x] &= ~n;
    }

    void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
    {
        uint8_t i, m, temp;
        uint8_t y0 = y;
        chr -= ' ';
        for (i = 0; i < 12; i++) {
            temp = asc2_1206[chr][i];
            for (m = 0; m < 8; m++) {
                if (temp & 0x80)
                    OLED_DrawPoint(x, y);
                else
                    OLED_ClearPoint(x, y);
                temp <<= 1;
                y++;
                if ((y - y0) == 12) {
                    y = y0;
                    x++;
                    break;
                }
            }
        }
    }
//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
    void OLED_ShowString(uint8_t x, uint8_t y, const char *chr)
    {
        while ((*chr >= ' ') && (*chr <= '~')) {
            OLED_ShowChar(x, y, *chr);
            x += 6 ;
            if (x > witdh) {
                x = 0;
                y += 12;
            }
            chr++;
        }
    }

//OLED的初始化
    void OLED_Init()
    {
        OLED_WR_Byte(0xAE, OLED_CMD); /*display off*/
        OLED_WR_Byte(0xD5, OLED_CMD); /*set osc division*/
        OLED_WR_Byte(0xF0, OLED_CMD);
        OLED_WR_Byte(0xA8, OLED_CMD); /*multiplex ratio*/
        OLED_WR_Byte(0x27, OLED_CMD); /*duty = 1/40*/
        OLED_WR_Byte(0xD3, OLED_CMD); /*set display offset*/
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x40, OLED_CMD); /*Set Display Start Line */
        OLED_WR_Byte(0x8d, OLED_CMD); /*set charge pump enable*/
        OLED_WR_Byte(0x14, OLED_CMD);
        OLED_WR_Byte(0x20, OLED_CMD); /*Set Horizontal addressing mode */
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0xA1, OLED_CMD); /*set segment remap*/
        OLED_WR_Byte(0xC8, OLED_CMD); /*Com scan direction*/
        OLED_WR_Byte(0xDA, OLED_CMD); /*set COM pins*/
        OLED_WR_Byte(0x12, OLED_CMD);
        OLED_WR_Byte(0xAD, OLED_CMD); /*Internal IREF Setting*/
        OLED_WR_Byte(0x30, OLED_CMD);
        OLED_WR_Byte(0x81, OLED_CMD); /*contract control*/
        OLED_WR_Byte(0xfF, OLED_CMD); /*128*/
        OLED_WR_Byte(0xD9, OLED_CMD); /*set pre-charge period*/
        OLED_WR_Byte(0x22, OLED_CMD);
        OLED_WR_Byte(0xdb, OLED_CMD); /*set vcomh*/
        OLED_WR_Byte(0x20, OLED_CMD);
        OLED_WR_Byte(0xA4, OLED_CMD); /*Set Entire Display On/Off*/
        OLED_WR_Byte(0xA6, OLED_CMD); /*normal / reverse*/
//        OLED_WR_Byte(0x0C, OLED_CMD); /*set lower column address*/
//        OLED_WR_Byte(0x11, OLED_CMD); /*set higher column address*/

        OLED_Clear();
        OLED_ColorTurn(0);   //0正常显示，1 反色显示
        OLED_DisplayTurn(1); //0正常显示 1 屏幕翻转显示
    }
    void OLED_SetWindow_full(){
        OLED_WR_Byte(0x21, OLED_CMD);
        OLED_WR_Byte(xoffset, OLED_CMD);
        OLED_WR_Byte(xoffset+witdh-1, OLED_CMD);
        OLED_WR_Byte(0x22, OLED_CMD);
        OLED_WR_Byte(yoffset/8, OLED_CMD);
        OLED_WR_Byte((yoffset+height)/8-1, OLED_CMD);
        OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
    }
    uint8_t * GetBuffer(){return OLED_GRAM;}
private:
    gpio_pin_type sda_pin;
    gpio_pin_type scl_pin;
    gpio_pin_type rst_pin;
    unsigned char i2c_addr;

    int witdh=70;
    int height=40;
    int xoffset=28;
    int yoffset=0;
    uint8_t OLED_GRAM[70*5];
};

smk_oled_t* g_oled[4];
extern "C"{
const char _smk_oled_defstr[4][3][12]={
        {
            "   `  ~    ","","  PrintSc  "
        },
        {
            "  Delete  ","","  Insert  "
        },
        {
            "  PageUp  ","","   Home   "
        },
        {
            " PageDown ","","    End    "
        }
};
__weak_symbol const char (*smk_oled_defstr)[4][3][12]=&_smk_oled_defstr ;
void smk_oled_init(){
    g_oled[0]=new smk_oled_t(i2c_1_sda_pin,i2c_1_scl_pin,oled_rst_pin,i2c_1_addr1);
    g_oled[1]=new smk_oled_t(i2c_1_sda_pin,i2c_1_scl_pin,oled_rst_pin,i2c_1_addr2);
    g_oled[2]=new smk_oled_t(i2c_2_sda_pin,i2c_2_scl_pin,oled_rst_pin,i2c_2_addr1);
    g_oled[3]=new smk_oled_t(i2c_2_sda_pin,i2c_2_scl_pin,oled_rst_pin,i2c_2_addr2);
    for(int i=0;i<4;i++)
    {
        g_oled[i]->init();
        g_oled[i]->OLED_Clear();
        g_oled[i]->OLED_ShowString(0,0,(*smk_oled_defstr)[i][0]);
        g_oled[i]->OLED_ShowString(0,13,(*smk_oled_defstr)[i][1]);
        g_oled[i]->OLED_ShowString(0,26,(*smk_oled_defstr)[i][2]);
        g_oled[i]->OLED_Refresh();
    }
}
QueueHandle_t g_smk_oled_sem;
unsigned int g_smk_oled_which_to_update=0;
int smk_oled_api_write(uint32_t addr,uint8_t* data,uint8_t len){
    g_smk_oled_which_to_update=data[0];
    xSemaphoreGive(g_smk_oled_sem);
    return 1;
}
int smk_oled_api_read(uint32_t addr,uint8_t* data,uint8_t len){
    g_smk_oled_which_to_update=data[0];
    xSemaphoreGive(g_smk_oled_sem);
    return 1;
}
static hid_data_reg_t oled_update_api={
        .base=0xc000,
        .size=1,
        .maptype=map_type_func,
        .datatype=data_type_ext,
        .data=NULL,
        .write=smk_oled_api_write,
        .read=smk_oled_api_read
};
static hid_data_reg_t oled_key1_api={
        .base=0xd000,
        .size=2800/8,
        .maptype=map_type_data,
        .datatype=data_type_fixed,
        .data=NULL,
};
static hid_data_reg_t oled_key2_api={
        .base=0xe000,
        .size=2800/8,
        .maptype=map_type_data,
        .datatype=data_type_fixed,
        .data=NULL,
};
static hid_data_reg_t oled_key3_api={
        .base=0xf000,
        .size=2800/8,
        .maptype=map_type_data,
        .datatype=data_type_fixed,
        .data=NULL,
};
static hid_data_reg_t oled_key4_api={
        .base=0x10000,
        .size=2800/8,
        .maptype=map_type_data,
        .datatype=data_type_fixed,
        .data=NULL,
};
[[noreturn]] void smk_oled_demon_task(void *pvParameters){
    smk_oled_init();
    g_smk_oled_sem=xSemaphoreCreateBinary();
    if(!g_smk_oled_sem)vTaskDelete(NULL);
    hid_data_protocal_reg(&oled_update_api);
    oled_key1_api.data=g_oled[0]->GetBuffer();
    oled_key2_api.data=g_oled[1]->GetBuffer();
    oled_key3_api.data=g_oled[2]->GetBuffer();
    oled_key4_api.data=g_oled[3]->GetBuffer();
    hid_data_protocal_reg(&oled_key1_api);
    hid_data_protocal_reg(&oled_key2_api);
    hid_data_protocal_reg(&oled_key3_api);
    hid_data_protocal_reg(&oled_key4_api);
    while(1){
        xSemaphoreTake(g_smk_oled_sem,0);
        if(g_smk_oled_which_to_update&1){
            g_oled[0]->OLED_Refresh();
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        if(g_smk_oled_which_to_update&2){
            g_oled[1]->OLED_Refresh();
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        if(g_smk_oled_which_to_update&4){
            g_oled[2]->OLED_Refresh();
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        if(g_smk_oled_which_to_update&8){
            g_oled[3]->OLED_Refresh();
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}
}