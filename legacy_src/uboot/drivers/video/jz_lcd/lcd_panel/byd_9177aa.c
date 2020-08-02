/* linux/drivers/video/exynos/byd_9177aa.c
 *
 * MIPI-DSI based byd_9177aa AMOLED lcd 4.65 inch panel driver.
 *
 * Inki Dae, <inki.dae@samsung.com>
 * Donghwa Lee, <dh09.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <config.h>
#include <serial.h>
#include <common.h>
#include <lcd.h>
#include <linux/list.h>
#include <linux/fb.h>
#include <asm/types.h>
#include <asm/arch/tcu.h>
#include <asm/arch/lcdc.h>
#include <asm/arch/gpio.h>
#include <regulator.h>

#include <jz_lcd/jz_dsim.h>
#include "../jz_mipi_dsi/jz_mipi_dsih_hal.h"

#define MIPI_RST_N GPIO_PC(3)
#define MIPI_PWR GPIO_PC(2)

vidinfo_t panel_info = {540, 960, LCD_BPP, };

static struct dsi_cmd_packet byd_9177aa_cmd_list[] = {
	{0x39, 0x04, 0x00,  {0xb9, 0xff, 0x83, 0x89}}, //set extc
	{0x39, 0x14, 0x00,  {0xb1, 0x00, 0x00, 0x06, 0xe8, 0x59, 0x10, 0x11, 0xd1, 0xf1, 0x3d, 0x45, 0x2e, 0x2e, 0x43, 0x01, 0x5a, 0xf0, 0x00, 0xe6}}, //set power
	{0x39, 0x08, 0x00,  {0xb2, 0x00, 0x00, 0x78, 0x0c, 0x07, 0x3f, 0x30}}, //set  display
	{0x39, 0x18, 0x00,  {0xb4, 0x80, 0x08, 0x00, 0x32, 0x10, 0x04, 0x32, 0x10, 0x00, 0x32, 0x10, 0x00, 0x37, 0x0a, 0x40, 0x08, 0x37, 0x00, 0x46, 0x06, 0x58, 0x58, 0x06}}, //column inversion
	{0x39, 0x39, 0x00,  {0xd5, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x60, 0x00, 0x99, 0x88, 0xaa, 0xbb, 0x88, 0x23, 0x88, 0x01, 0x88, 0x67, 0x88, 0x45, 0x01, 0x23, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x99, 0xbb, 0xaa, 0x88, 0x54, 0x88, 0x76, 0x88, 0x10, 0x88, 0x32, 0x32, 0x10, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},//set gip
	{0x39, 0x04, 0x00,  {0xb7, 0x00, 0x00, 0x50}}, //set power option
	{0x39, 0x05, 0x00,  {0xb6, 0x00, 0x92, 0x00, 0x92}}, //set vcom
	{0x39, 0x23, 0x00,  {0xe0, 0x05, 0x11, 0x14, 0x37, 0x3f, 0x3f, 0x20, 0x4f, 0x05, 0x0e, 0x0d, 0x12, 0x14, 0x12, 0x14, 0x1d, 0x1c, 0x05, 0x11, 0x14, 0x37, 0x3f, 0x3f, 0x20, 0x4f, 0x05, 0x0e, 0x0d, 0x12, 0x14, 0x12, 0x14, 0x1d, 0x1c}}, //set gamma
	{0x39, 0x80, 0x00,  {0xc1, 0x01, 0x00, 0x1b, 0x20, 0x28, 0x2e, 0x34, 0x3b, 0x43, 0x4b, 0x52, 0x5a, 0x62, 0x6b, 0x72, 0x78, 0x7D, 0x82, 0x87, 0x8D, 0x93, 0x9A, 0x9E, 0xA3, 0xA8, 0xAF, 0xB5, 0xBE, 0xC6, 0xCD, 0xD8, 0xE1, 0xEB, 0xF9, 0x10, 0x12, 0x02, 0x25, 0x65, 0x55, 0x65, 0x65, 0x40, 0x00, 0x1B, 0x20, 0x28, 0x2E, 0x34, 0x3B, 0x43, 0x4B, 0x52, 0x5A, 0x62, 0x6B, 0x72, 0x78, 0x7D, 0x82, 0x87, 0x8D, 0x93, 0x9A, 0x9E, 0xA3, 0xA8, 0xAF, 0xB5, 0xBE, 0xC6, 0xCD, 0xD8, 0xE1, 0xEB, 0xF9, 0x10, 0x12, 0x02, 0x25, 0x65, 0x55, 0x65, 0x65, 0x40, 0x00, 0x1B, 0x20, 0x28, 0x2E, 0x34, 0x3B, 0x43, 0x4B, 0x52, 0x5A, 0x62, 0x6B, 0x72, 0x78, 0x7D, 0x82, 0x87, 0x8D, 0x93, 0x9A, 0x9E, 0xA3, 0xA8, 0xAF, 0xB5, 0xBE, 0xC6, 0xCD, 0xD8, 0xE1, 0xEB, 0xF9, 0x10, 0x12, 0x02, 0x25, 0x65, 0x55, 0x65, 0x65, 0x40}}, //set dgc lut
	{0x39, 0x02, 0x00,  {0xCC, 0x02}}, //set rev panel
};

#if 0
static void byd_9177aa_regulator_enable(struct byd_9177aa *lcd)
{
}
static void byd_9177aa_regulator_disable(struct byd_9177aa *lcd)
{
}
#endif
static void byd_9177aa_sleep_in(struct dsi_device *dsi)
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x10, 0x00};

	write_command(dsi, data_to_send);
}

static void byd_9177aa_sleep_out(struct dsi_device *dsi)
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x11, 0x00};

	write_command(dsi, data_to_send);
}

static void byd_9177aa_display_on(struct dsi_device *dsi)
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x29, 0x00};
	write_command(dsi, data_to_send);
}

static void byd_9177aa_display_off(struct dsi_device *dsi)
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x28, 0x00};
	write_command(dsi, data_to_send);
}

static void byd_9177aa_panel_init(struct dsi_device *dsi)
{
	int  i;
	for(i = 0; i < ARRAY_SIZE(byd_9177aa_cmd_list); i++) {
		write_command(dsi,  byd_9177aa_cmd_list[i]);
	}
}

static int byd_9177aa_set_power(struct lcd_device *ld, int power)
{
	int ret = 0;
	return ret;
}

void panel_pin_init(void)
{
	debug("--------------------%s\n", __func__);
	int ret= 0;
	ret = gpio_request(MIPI_RST_N, "lcd mipi panel rst");

	ret = gpio_request(MIPI_PWR, "lcd mipi panel avcc");
	serial_puts("byd_9177aa panel display pin init\n");
}

void panel_init_set_sequence(struct dsi_device *dsi)
{
	debug("--------------------%s\n", __func__);
	byd_9177aa_panel_init(dsi);
	byd_9177aa_sleep_out(dsi);
	udelay(120*1000);
	byd_9177aa_display_on(dsi);
	udelay(10*1000);
}
void panel_display_on(struct dsi_device *dsi)
{
}


void panel_power_on(void)
{
	debug("--------------------%s\n", __func__);
	gpio_direction_output(MIPI_PWR, 1); /* 2.8v en*/
	udelay(3*1000);
	gpio_direction_output(MIPI_RST_N, 0);
	udelay(3*1000);
	gpio_direction_output(MIPI_RST_N, 1);
	udelay(8*1000);

	serial_puts("byd_9177aa panel display on\n");
}

void panel_power_off(void)
{
	gpio_direction_output(MIPI_PWR, 0); /* 2.8v en*/
	serial_puts("byd_9177aa panel display off\n");
}

struct fb_videomode jzfb1_videomode = {
	.name = "byd_9177aa-lcd",
	.refresh = 60,
	.xres = 540,
	.yres = 960,
	.pixclock = KHZ2PICOS(26000),
	.left_margin = 48, //48,
	.right_margin = 400, //48,
	.upper_margin = 13,
	.lower_margin = 9,
	.hsync_len = 48,
	.vsync_len = 2,
	.sync = FB_SYNC_HOR_HIGH_ACT & FB_SYNC_VERT_HIGH_ACT,
	.vmode = FB_VMODE_NONINTERLACED,
	.flag = 0,
};

