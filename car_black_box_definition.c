/* Car Black Box Definition*/


#include "main.h"

unsigned char clock_reg[3];
/* To store the time in HH:MM:SS format */
char time[7];  // "HHMMSS"
char log[11];
int log_count;
unsigned char return_time = 5;
unsigned char sec = 0;
unsigned char* menu_array[] = {"View_Log", "Clear_Log", "Download_log", "Set_time", "Change_passwd"};
unsigned int pass_change = 0;
unsigned int long_press = 0;
unsigned int short_press = 0;





void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    

    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void display_time()
{
    get_time();
    
    //HH
    clcd_putch(time[0], LINE2(1)); 
    clcd_putch(time[1], LINE2(2)); 
    clcd_putch(':', LINE2(3)); 
    
    //MM
    clcd_putch(time[2], LINE2(4)); 
    clcd_putch(time[3], LINE2(5)); 
    clcd_putch(':', LINE2(6)); 
    
    //SS
    clcd_putch(time[4], LINE2(7));
    clcd_putch(time[5], LINE2(8));
}



void display_dashboard(unsigned char *event, unsigned char speed)
{
    clcd_print("TIME     E  SP", LINE1(2));
    
    //To display time
    display_time();
    
    //To display event
    clcd_print((const char *)event, LINE2(11));
    
    //To display speed
    clcd_putch((speed) / 10 + '0', LINE2(14));
    clcd_putch((speed) % 10 + '0', LINE2(15));
}

void record_log()
{
    //EEPROM Write
    //Write log string to memory
    unsigned char addr;
    if(log_count == 10)
    {
        log_count = 0;
    }
    addr = (unsigned char) (0x05 + log_count * 10);
    log_count++;
    eeprom_at24c04_str_write(addr, log);
}

void log_car_event(unsigned char *event, unsigned char speed)
{
    get_time();
    strncpy(log, time, 6);
    strncpy(&log[6], (const char *)event, 2);
    log[8] = (speed / 10) + '0';
    log[9] = (speed % 10) + '0';
    log[10] = '\0';
    record_log();
}


void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

unsigned char login(unsigned char key, unsigned char reset_flag)
{
    static char user_pass[5];
    static unsigned char i, attempts_left;
    static unsigned int min = 14;
    
    
    if(reset_flag == RESET_PASSWORD)
    {
        key = ALL_RELEASED;
        attempts_left = 3;
        user_pass[0] = '\0';
        user_pass[1] = '\0';
        user_pass[2] = '\0';
        user_pass[3] = '\0';
        i = 0;
        return_time = 5;
    }
    if(return_time == 0)
    {
        return RETURN_BACK;
        
    }
    if(key == SW4 && i < 4) //1
    {
        user_pass[i] = '1';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    }
    else if(key == SW5 && i < 4)
    {
        user_pass[i] = '0';
        clcd_putch('*', LINE2(4 + i));
        i++;
        return_time = 5;
    }
    if(i == 4)
    {
        char s_password[4];
        for(unsigned char j = 0; j < 4; j++)
        {
            s_password[j] = eeprom_at24c04_random_read(j);
        }
        
        if(strncmp(user_pass, s_password, 4) == 0)
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("LOGIN SUCCESS", LINE1(1));
            __delay_ms(2000);
            return LOGIN_SUCCESS;
            
        }
        else
        {
            attempts_left--;
            if(attempts_left == 0)
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are blocked", LINE1(0));
                clcd_print("Wait for", LINE2(0));
                sec = 60;
                while(min != 0)
                {
                    if(sec == 0)
                    {
                        sec = 60;
                        min--;
                    }
                    clcd_putch((min / 10) + '0', LINE2(10));
                    clcd_putch((min % 10) + '0', LINE2(11));
                    clcd_putch(':', LINE2(12));
                    clcd_putch((sec / 10) + '0', LINE2(13));
                    clcd_putch((sec % 10) + '0', LINE2(14));
                }
                attempts_left = 3;
            }
            else
            {
                clear_screen();
                clcd_print("Wrong Password", LINE1(0));
                clcd_putch(attempts_left + '0', LINE2(1));
                clcd_print("Attempts Left", LINE2(3));
                __delay_ms(3000);
            }
            clear_screen();
            clcd_print("ENTER PASSWORD", LINE1(0));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
            return_time = 5;
        }
        
    }
    
}

unsigned char menu_screen(unsigned char key, unsigned char reset_flag)
{
    static unsigned char menu_pos = 0;
    static int delay = 0;
    static int long_press = 0;
    if(reset_flag == RESET_MENU)
    {
        clear_screen();
        menu_pos = 0;
        return_time = 5;
    }
    if(menu_pos < 4)
    {
        if (short_press) 
        {
            clear_screen();
            menu_pos++;
            return_time = 5;
            short_press = 0;
        }
  
    }
    if(key == SW5 && menu_pos > 0)
    {
        clear_screen();
        menu_pos--;
        return_time = 5;
    }
    
    if(menu_pos == 4)
    {
        clcd_putch('*', LINE2(0));
        clcd_print(menu_array[menu_pos - 1], LINE1(2));
        clcd_print(menu_array[menu_pos], LINE2(2));
    }
    else
    {
        clcd_putch('*', LINE1(0));
        clcd_print(menu_array[menu_pos], LINE1(2));
        clcd_print(menu_array[menu_pos + 1], LINE2(2));
    }
    
    if(return_time == 0)
    {
        return MENU_SCREEN_IDLE;
    }
    
    if(read_digital_keypad(LEVEL) == SW2)
    {
        if(delay == 20)
        {
            delay = 0;
            long_press = 1;
        }
        else
        {
            delay++;
        }
    }
    else
    {
        if(long_press)
        {
            return MENU_SCREEN_IDLE;
        }
        long_press = 0;
    }
    
    return menu_pos;
    
}


unsigned char view_log(unsigned char key)
{
    //EEPROM Write
    //Write log string to memory
    unsigned char log_read[11];
    unsigned char eeprom_read[10];
    unsigned char addr;
    static int delay_1 = 0;
    static int long_press_1 = 0;
    static int delay_2 = 0;
    static int long_press_2 = 0;
    static int log_c = 0;
    if(log_c == 10)
    {
        log_c = 0;
    }
    addr = (unsigned char) (0x05 + log_c * 10);
    if(key == SW4 && log_c < 9)
    {
        clear_screen();
        log_c++;
    }
    else if(key == SW5 && log_c > 0)
    {
        clear_screen();
        log_c--;
    }
    // Read the log data from EEPROM into the eeprom_read array byte by byte
    for (int i = 0; i < 10; i++)
    {
        eeprom_read[i] = eeprom_at24c04_random_read(addr + i);  // Read each byte from EEPROM
    }

    // Copy the EEPROM data to log_read for display
    for (int i = 0; i < 10; i++)
    {
        log_read[i] = eeprom_read[i];
    }
    log_read[10] = '\0';  // Null-terminate the string to ensure correct display
    
    
    clcd_print("LOG", LINE1(0));
    clcd_putch(log_c + '0', LINE1(3));
    clcd_print("TIME  E SP", LINE1(6));
    clcd_putch(log_read[0], LINE2(2));
    clcd_putch(log_read[1], LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(log_read[2], LINE2(5));
    clcd_putch(log_read[3], LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(log_read[4], LINE2(8));
    clcd_putch(log_read[5], LINE2(9));
    clcd_putch(log_read[6], LINE2(11));
    clcd_putch(log_read[7], LINE2(12));
    clcd_putch(log_read[8], LINE2(14));
    clcd_putch(log_read[9], LINE2(15));
    
    
    if(read_digital_keypad(LEVEL) == SW4)
    {
        if(delay_1 == 20)
        {
            delay_1 = 0;
            long_press_1 = 1;
        }
        else
        {
            delay_1++;
        }
    }
    else
    {
        if(long_press_1)
        {
            return LONG_PRESS_UP;
        }
        long_press_1 = 0;
    }
    
    
    if(read_digital_keypad(LEVEL) == SW5)
    {
        if(delay_2 == 20)
        {
            delay_2 = 0;
            long_press_2 = 1;
        }
        else
        {
            delay_2++;
        }
    }
    else
    {
        if(long_press_2)
        {
            return LONG_PRESS_DOWN;
        }
        long_press_2 = 0;
    }
    return NO_ACTION;
}

unsigned char clear_log(void)
{
    static int delay_1 = 0;
    static int long_press_1 = 0;
    static int delay_2 = 0;
    static int long_press_2 = 0;
    static int flag = 0;
    if (!flag) 
    {
        clear_screen();
        clcd_print("CLEARED ALL LOGS", LINE1(0));
        flag = 1;
    }
    
    log_count = 0; 
    if(read_digital_keypad(LEVEL) == SW4)
    {
        if(delay_1 == 20)
        {
            delay_1 = 0;
            long_press_1 = 1;
        }
        else
        {
            delay_1++;
        }
    }
    else
    {
        if(long_press_1)
        {
            return LONG_PRESS_UP_CLEAR;
        }
        long_press_1 = 0;
    }
    
    
    if(read_digital_keypad(LEVEL) == SW5)
    {
        if(delay_2 == 20)
        {
            delay_2 = 0;
            long_press_2 = 1;
        }
        else
        {
            delay_2++;
        }
    }
    else
    {
        if(long_press_2)
        {
            return LONG_PRESS_DOWN_CLEAR;
        }
        long_press_2 = 0;
    }
    return NO_ACTION;
}

unsigned char download_log(unsigned char key) 
{
    static int flag = 0;
    static int delay_1 = 0;
    static int long_press_1 = 0;
    static int delay_2 = 0;
    static int long_press_2 = 0;

    if (!flag) 
    {
        clear_screen();
        clcd_print("Open", LINE1(5));
        clcd_print("Cutecom", LINE2(5));
        flag = 1;
    }



    //EEPROM Write
    //Write log string to memory
    unsigned char log_read[11];
    unsigned char eeprom_read[10];
    unsigned char addr;
    static int log_c = 0;

    if (log_c >= 10) 
    {
        log_c = 0;
    }


    addr = (unsigned char) (0x05 + log_c * 10);


    // Read the log data from EEPROM into the eeprom_read array byte by byte
    for (int i = 0; i < 10; i++) 
    {
        eeprom_read[i] = eeprom_at24c04_random_read(addr + i); // Read each byte from EEPROM
    }

    // Copy the EEPROM data to log_read for display
    for (int i = 0; i < 10; i++) 
    {
        log_read[i] = eeprom_read[i];
    }
    log_read[10] = '\0'; // Null-terminate the string to ensure correct display

    if (log_c == 0) 
    {
        puts("LOGS\n");
        puts("TIME              E                SP\n");
    }




    putchar(log_read[0]);
    putchar(log_read[1]);
    putchar(':');
    putchar(log_read[2]);
    putchar(log_read[3]);
    putchar(':');
    putchar(log_read[4]);
    putchar(log_read[5]);
    puts("        ");
    putchar(log_read[6]);
    putchar(log_read[7]);
    puts("              ");
    putchar(log_read[8]);
    putchar(log_read[9]);
    putchar('\n');

    log_c++;

    if (log_c == 10) 
    {
        puts("\n\n------------------*---------------------\n\n");
    }
    
    if(read_digital_keypad(LEVEL) == SW4)
    {
        if(delay_1 == 20)
        {
            delay_1 = 0;
            long_press_1 = 1;
        }
        else
        {
            delay_1++;
        }
    }
    else
    {
        if(long_press_1)
        {
            return LONG_PRESS_UP_DOWNLOAD;
        }
        long_press_1 = 0;
    }
    
    
    if(read_digital_keypad(LEVEL) == SW5)
    {
        if(delay_2 == 20)
        {
            delay_2 = 0;
            long_press_2 = 1;
        }
        else
        {
            delay_2++;
        }
    }
    else
    {
        if(long_press_2)
        {
            return LONG_PRESS_DOWN_DOWNLOAD;
        }
        long_press_2 = 0;
    }
    return NO_ACTION;

}

unsigned char change_password(unsigned char key) 
{
    static int delay_1 = 0;
    static int long_press_1 = 0;
    static int delay_2 = 0;
    static int long_press_2 = 0;

    char new_pass[4];
    int i = 0;
    char reenter_pass[4];
    int j = 0;
    

    if (!pass_change) 
    {
        clear_screen();
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
        clcd_print("Enter New Passwd", LINE1(0));
        while (i < 4) 
        {
            key = read_digital_keypad(STATE);
            if (key == SW4 && i < 4) //1
            {
                new_pass[i] = '1';
                clcd_putch('*', LINE2(4 + i));
                i++;
            }
            else if (key == SW5 && i < 4) 
            {
                new_pass[i] = '0';
                clcd_putch('*', LINE2(4 + i));
                i++;
            }
        }

        if (i == 4) 
        {
            clear_screen();
            clcd_print("Re-Enter Passwd", LINE1(0));
            clcd_write(LINE2(4), INST_MODE);
        }
        while (j < 4) 
        {
            key = read_digital_keypad(STATE);

            if (key == SW4 && j < 4) //1
            {
                reenter_pass[j] = '1';
                clcd_putch('*', LINE2(4 + j));
                j++;
            }
            else if (key == SW5 && j < 4) 
            {
                reenter_pass[j] = '0';
                clcd_putch('*', LINE2(4 + j));
                j++;
            }

        }
        if (j == 4) 
        {
            if (strncmp(new_pass, reenter_pass, 4) == 0) 
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Password Changed", LINE1(0));
                __delay_ms(3000);
                eeprom_at24c04_str_write(0x00, new_pass);
                return NOTHING;
            }
            else 
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Passwd Mismatch", LINE1(0));
                clcd_print("Retry", LINE2(3));
                __delay_ms(10000);
                __delay_ms(10000);
                __delay_ms(10000);
                clear_screen();
                i = 0;
                j = 0;
                return ENTER_PASS;
            }

        }
    }
    
    if (pass_change) 
    {
        if (read_digital_keypad(LEVEL) == SW4) 
        {
            if (delay_1 == 20) 
            {
                delay_1 = 0;
                long_press_1 = 1;
            }
            else 
            {
                delay_1++;
            }
        }
        else 
        {
            if (long_press_1) 
            {
                return LONG_PRESS_UP_PASS;
            }
            long_press_1 = 0;
        }


        if (read_digital_keypad(LEVEL) == SW5) 
        {
            if (delay_2 == 20) 
            {
                delay_2 = 0;
                long_press_2 = 1;
            }
            else 
            {
                delay_2++;
            }
        } 
        else 
        {
            if (long_press_2) 
            {
                return LONG_PRESS_DOWN_PASS;
            }
            long_press_2 = 0;
        }
    }
    return NO_ACTION;

}


unsigned char set_time(unsigned char key)
{
    static int delay_1 = 0;
    static int long_press_1 = 0;
    static int delay_2 = 0;
    static int long_press_2 = 0;
    
    static int delay = 0;
    static int fields = 0;
    static char blink = 1;
    
    clcd_print("       ", LINE2(9));
    
    
    if (fields == 0) 
    {
        clcd_print("Sec field Edit", LINE1(0));
    }
    else if(fields == 1)
    {
        clcd_print("Min field Edit", LINE1(0));

    }
    else if(fields == 2)
    {
        clcd_print("Hour field Edit", LINE1(0));
    }
    
    if(delay == 10)
    {
        blink = !blink;
        delay = 0; 
    }
    else
    {
        delay++;
    }
    
    // Display time with blinking for the current field
    if (fields == 0) // Seconds
    {
        //HH
        clcd_putch(time[0], LINE2(1));
        clcd_putch(time[1], LINE2(2));
        clcd_putch(':', LINE2(3));

        //MM
        clcd_putch(time[2], LINE2(4));
        clcd_putch(time[3], LINE2(5));
        clcd_putch(':', LINE2(6));
        
        //SS
        clcd_putch(blink ? time[4] : ' ', LINE2(7));
        clcd_putch(blink ? time[5] : ' ', LINE2(8));
        
        
    }
    else if (fields == 1) // Minutes
    {
  
        //HH
        clcd_putch(time[0], LINE2(1));
        clcd_putch(time[1], LINE2(2));
        clcd_putch(':', LINE2(3));
        
        //MM
        clcd_putch(blink ? time[2] : ' ', LINE2(4));
        clcd_putch(blink ? time[3] : ' ', LINE2(5));
        clcd_putch(':', LINE2(6));

        //SS
        clcd_putch(time[4], LINE2(7));
        clcd_putch(time[5], LINE2(8));
        
        
    }
    else if (fields == 2) // Hours
    {
        //HH
        clcd_putch(blink ? time[0] : ' ', LINE2(1));
        clcd_putch(blink ? time[1] : ' ', LINE2(2));
        
        //MM
        clcd_putch(time[2], LINE2(4));
        clcd_putch(time[3], LINE2(5));
        
        clcd_putch(':', LINE2(6));

        //SS
        clcd_putch(time[4], LINE2(7));
        clcd_putch(time[5], LINE2(8));
        

    }
    
    
    if (key == SW4) // Increment the current field
    {
        if (fields == 0) // Seconds
        {
            int sec = (time[4] - '0') * 10 + (time[5] - '0');
            sec = (sec + 1) % 60;
            time[4] = (sec / 10) + '0';
            time[5] = (sec % 10) + '0';
        } 
        else if (fields == 1) // Minutes
        {
            int min = (time[2] - '0') * 10 + (time[3] - '0');
            min = (min + 1) % 60;
            time[2] = (min / 10) + '0';
            time[3] = (min % 10) + '0';
        } 
        else if (fields == 2) // Hours
        {
            int hour = (time[0] - '0') * 10 + (time[1] - '0');
            hour = (hour + 1) % 24;
            time[0] = (hour / 10) + '0';
            time[1] = (hour % 10) + '0';
        }
    } 
    else if (key == SW5) // Move to the next field
    {
        fields = (fields + 1) % 3; // Cycle through fields (0 -> 1 -> 2 -> 0)
    }
    
    // Write the updated time to the RTC after editing
    unsigned char hour_bcd = dec_to_bcd((time[0] - '0') * 10 + (time[1] - '0'));
    unsigned char min_bcd = dec_to_bcd((time[2] - '0') * 10 + (time[3] - '0'));
    unsigned char sec_bcd = dec_to_bcd((time[4] - '0') * 10 + (time[5] - '0'));

    write_ds1307(HOUR_ADDR, hour_bcd);
    write_ds1307(MIN_ADDR, min_bcd);
    write_ds1307(SEC_ADDR, sec_bcd);
    
    if(read_digital_keypad(LEVEL) == SW4)
    {
        if(delay_1 == 20)
        {
            delay_1 = 0;
            long_press_1 = 1;
        }
        else
        {
            delay_1++;
        }
    }
    else
    {
        if(long_press_1)
        {
            return LONG_PRESS_UP_TIME;
        }
        long_press_1 = 0;
    }
    
    
    if(read_digital_keypad(LEVEL) == SW5)
    {
        if(delay_2 == 20)
        {
            delay_2 = 0;
            long_press_2 = 1;
        }
        else
        {
            delay_2++;
        }
    }
    else
    {
        if(long_press_2)
        {
            return LONG_PRESS_DOWN_TIME;
        }
        long_press_2 = 0;
    }
    return NO_ACTION;
    
}

unsigned char dec_to_bcd(unsigned char dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}
    

