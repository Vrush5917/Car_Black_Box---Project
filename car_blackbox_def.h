/* 
 * File:   car_blackbox_def.h
 * Author: magdu
 *
 * Created on 14 November, 2024, 12:14 PM
 */

#ifndef CAR_BLACKBOX_DEF_H
#define	CAR_BLACKBOX_DEF_H

void display_dashboard(unsigned char *event, unsigned char speed);

void log_car_event(unsigned char *event, unsigned char speed);

void clear_screen(void);

unsigned char login(unsigned char key, unsigned char reset_flag);

void clcd_write(unsigned char byte, unsigned char mode);

unsigned char menu_screen(unsigned char key, unsigned char reset_flag);

unsigned char view_log(unsigned char key);

unsigned char clear_log(void);

unsigned char download_log(unsigned char key);

unsigned char change_password(unsigned char key);

unsigned char set_time(unsigned char key);

unsigned char dec_to_bcd(unsigned char dec);





#endif	/* CAR_BLACKBOX_DEF_H */

