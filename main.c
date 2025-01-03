/*
 * File:   main.c
 * Author: VRUSHABH MAGDUM
 *
 * Created on 14 November, 2024, 10:16 AM
 */



#include "main.h"


#pragma config WDTE = OFF

extern unsigned char return_time;
extern unsigned int pass_change;
extern unsigned int long_press;
extern unsigned int short_press;


static void init_config(void) 
{

    //Initialize I2C
    init_i2c(100000);
    
    //Initialize RTC
    init_ds1307();
    
    //Initialize CLCD
    init_clcd();
    
    //Initialize Digital_Keypad
    init_digital_keypad();
    
    //Initialize ADC
    init_adc();
    
    //Initialize Timer 2
    init_timer2();
    
    //Initialize UART
    init_uart(9600);
    
    puts("UART TEST CODE\n\r");
    
    
    GIE = 1;
    
    PEIE = 1;
}

void main(void) 
{
    //variable declaration
    unsigned char control_flag = DASH_BOARD, reset_flag;
    unsigned char key;
    unsigned char speed = 0;
    char *gear[] = {"GR", "GN", "G1", "G2", "G3", "G4"};
    unsigned char event[3] = "ON";
    unsigned char gr = 0;
    unsigned int delay = 0;
    static unsigned char menu_pos = 0;
    static unsigned char view_idle = 0;
    static unsigned char time_idle = 0;
    static unsigned char clear_idle = 0;
    static unsigned char download_idle = 0;
    static unsigned char change_pass_idle = 0;
    char ch;
    
    init_config();
    log_car_event(event, speed);
    eeprom_at24c04_str_write(0x00, "1010");
    while (1) 
    {
        key = read_digital_keypad(STATE);
        //For debouncing
        for(unsigned char j = 200; j--;);
        if(key == SW1)
        {
            strcpy(event, "CO");
            log_car_event(event, speed);
        }
        else if(key == SW2 && gr < 6)
        {
            strcpy(event, gear[gr]);
            gr++;
            log_car_event(event, speed);
        }
        else if(key == SW3 && gr > 0)
        {
            gr--;
            strcpy(event, gear[gr]);
            log_car_event(event, speed);
        }
        else if((key == SW4 || key == SW5) && control_flag == DASH_BOARD)
        {
            control_flag = LOGIN;
            clear_screen();
            clcd_print("ENTER PASSWORD", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            reset_flag = RESET_PASSWORD;
            TMR2ON = 1;/* Switching on the Timer2 */
            
        }
        
        if (read_digital_keypad(LEVEL) == SW4 && control_flag == MAIN_MENU)
        {
            delay++;
            if(delay == 20)
            {
                long_press = 1;
                delay = 30;
            }
        } 
        else 
        {
            if(delay > 0 && delay < 20)
            {
                short_press = 1;
                delay = 0;
            }
            if (long_press) 
            {
                if (menu_pos == 0) 
                {
                    clear_screen();
                    control_flag = VIEW_LOG;

                }

                if (menu_pos == 1) 
                {
                    control_flag = CLEAR_LOG;

                }

                if (menu_pos == 2) 
                {
                    control_flag = DOWNLOAD_LOG;

                }

                if (menu_pos == 3) 
                {
                    control_flag = SET_TIME;

                }

                if (menu_pos == 4) 
                {
                    control_flag = CHANGE_PASS;
                }
                long_press = 0;

            }
            if(delay < 200)
            {
               delay = 0; 
            }
            
        }
        
        
        
        speed = read_adc() / 10.25; // to be fit in range of 0 - 99
        
        
        switch(control_flag)
        {
            case DASH_BOARD:
                pass_change = 0; // Password can be rechanged
                display_dashboard(event, speed);
                break;
                
            case LOGIN:
                switch(login(key, reset_flag))
                {
                    case RETURN_BACK:
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;/* Switching OFF the Timer2 */
                        
                        break;
                        
                    case LOGIN_SUCCESS:
                        control_flag = MAIN_MENU;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;/* Switching OFF the Timer2 */
                        reset_flag = RESET_MENU;
                        continue;
                        
                }
                break;
                
            case MAIN_MENU:
                pass_change = 0; // Password can be rechanged
                TMR2ON = 1;
                menu_pos = menu_screen(key, reset_flag);
                if (menu_pos == MENU_SCREEN_IDLE) 
                {
                    control_flag = DASH_BOARD;
                    clear_screen();
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    TMR2ON = 0; /* Switching OFF the Timer2 */
                }
                break;
                
            case VIEW_LOG:
                view_idle = view_log(key);
                switch(view_idle)
                {
                    case LONG_PRESS_UP:
                    {
                        control_flag = MAIN_MENU;
                        return_time = 5;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case LONG_PRESS_DOWN:
                    {
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                        
                }
                break;
                
                
            case CLEAR_LOG:
                clear_idle = clear_log();
                switch(clear_idle)
                {
                    case LONG_PRESS_UP_CLEAR:
                    {
                        control_flag = MAIN_MENU;
                        return_time = 5;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case LONG_PRESS_DOWN_CLEAR:
                    {
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                }
                break;
                
            case DOWNLOAD_LOG:
                download_idle = download_log(key);
                switch(download_idle)
                {
                    case LONG_PRESS_UP_DOWNLOAD:
                    {
                        control_flag = MAIN_MENU;
                        return_time = 5;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case LONG_PRESS_DOWN_DOWNLOAD:
                    {
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                }
                break;
                
            case SET_TIME:
                time_idle = set_time(key);
                switch(time_idle)
                {
                    case LONG_PRESS_UP_TIME:
                    {
                        control_flag = MAIN_MENU;
                        return_time = 5;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case LONG_PRESS_DOWN_TIME:
                    {
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                }
                break;
                
            case CHANGE_PASS:
                change_pass_idle = change_password(key);
                switch(change_pass_idle)
                {
                    case LONG_PRESS_UP_PASS:
                    {
                        control_flag = MAIN_MENU;
                        return_time = 5;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case LONG_PRESS_DOWN_PASS:
                    {
                        control_flag = DASH_BOARD;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                    }
                    break;
                    
                    case ENTER_PASS:
                    {
                        control_flag = CHANGE_PASS;
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        pass_change = 0;
                    }
                    break;
                    
                    case NOTHING:
                    {
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        pass_change = 1;
                        
                       
                        
                        
                        

                    }
                    break;
                }
                break;
        }
        reset_flag = RESET_NOTHING;
        
    }
    return;
}
