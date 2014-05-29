	/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#include <linux/regulator/consumer.h>

#define SENSOR_NAME "ov7736"

/*=============================================================
  SENSOR REGISTER DEFINES
  ==============================================================*/

DEFINE_MUTEX(ov7736_mut);
static struct msm_sensor_ctrl_t ov7736_s_ctrl;

static struct msm_cam_clk_info cam_mot_8960_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};


static uint16_t sharpening_saved_5300;
static uint16_t sharpening_saved_5301;

/*=============================================================
  SENSOR REGISTER DEFINES
  ==============================================================*/

static struct msm_camera_i2c_reg_conf ov7736_start_settings[] = {
	{0x302e, 0x00},
	{0x3623, 0x03},
	{0x3614, 0x00},
	{0x4805, 0x10},
	{0x300e, 0x04},
	{0x3008, 0x02},
};

static struct msm_camera_i2c_reg_conf ov7736_stop_settings[] = {
	{0x302E, 0x08},
	{0x3623, 0x00},
	{0x3614, 0x20},
	{0x4805, 0xD0},
	{0x300e, 0x14},
	{0x3008, 0x42},
};

#ifdef MOTO_FPSRATE
static struct msm_camera_i2c_reg_conf ov7736_15_15_fps_settings[] = {
	{0x380C, 0x06},
	{0x380D, 0x28},
};

static struct msm_camera_i2c_reg_conf ov7736_30_30_fps_settings[] = {
	{0x380C, 0x03},
	{0x380D, 0x14},
};

#endif

static struct msm_camera_i2c_reg_conf mode_preview_tbl[] = {
};

static struct msm_camera_i2c_reg_conf ov7736_recommend_settings[] = {
	{0x3008, 0x82},
	{0xFFFE, 10},
	{0x3008, 0x42},
	{0xFFFE, 10},
	{0x3104, 0x03},
	{0x3017, 0x7f},
	{0x3018, 0xfc},
	{0x3602, 0x14},
	{0x3611, 0x44},
	{0x3631, 0x22},
	{0x3622, 0x00},
	{0x3633, 0x25},
	{0x370d, 0x04},
	{0x3620, 0x32},
	{0x3714, 0x2c},
	{0x401c, 0x00},
	{0x401e, 0x11},
	{0x4702, 0x01},
	{0x5000, 0x0e},
	{0x5001, 0x01},
	{0x3a00, 0x7a},
	{0x3a18, 0x00},
	{0x3a19, 0x3f},
	{0x300f, 0x88},
	{0x3011, 0x08},
	{0x4303, 0xff},
	{0x4307, 0xff},
	{0x430b, 0xff},
	{0x4305, 0x00},
	{0x4309, 0x00},
	{0x430d, 0x00},
	{0x5000, 0x4f},
	{0x5001, 0x47},
	{0x4300, 0x30},
	{0x4301, 0x80},
	{0x501f, 0x01},
	{0x3017, 0x00},
	{0x3018, 0x00},
	{0x300e, 0x04},
	{0x4801, 0x0f},
	{0x4601, 0x02},
	{0x300f, 0x8a},
	{0x3011, 0x02},
	{0x3010, 0x00},
	{0x4300, 0x3f},
	{0x5180, 0x02},
	{0x5181, 0x02},
	{0x3a0f, 0x35},
	{0x3a10, 0x2c},
	{0x3a1b, 0x36},
	{0x3a1e, 0x2d},
	{0x3a11, 0x90},
	{0x3a1f, 0x10},
	{0x5000, 0xcf},
	{0x5481, 0x08},
	{0x5482, 0x18},
	{0x5483, 0x38},
	{0x5484, 0x60},
	{0x5485, 0x6e},
	{0x5486, 0x78},
	{0x5487, 0x81},
	{0x5488, 0x89},
	{0x5489, 0x90},
	{0x548a, 0x97},
	{0x548b, 0xa5},
	{0x548c, 0xb0},
	{0x548d, 0xc5},
	{0x548e, 0xd8},
	{0x548f, 0xe9},
	{0x5490, 0x1f},
	{0x5380, 0x50},
	{0x5381, 0x43},
	{0x5382, 0x0D},
	{0x5383, 0x10},
	{0x5384, 0x3f},
	{0x5385, 0x50},
	{0x5392, 0x1e},
	{0x5801, 0x00},
	{0x5802, 0x00},
	{0x5803, 0x00},
	{0x5804, 0x34},
	{0x5805, 0x28},
	{0x5806, 0x22},
	{0x5001, 0xc7},
	{0x5580, 0x02},
	{0x5583, 0x40},
	{0x5584, 0x26},
	{0x5589, 0x10},
	{0x558a, 0x00},
	{0x558b, 0x3e},
	{0x5300, 0x0f},
	{0x5301, 0x30},
	{0x5302, 0x0d},
	{0x5303, 0x02},
	{0x5300, 0x04},
	{0x5301, 0x20},
	{0x5302, 0x0a},
	{0x5303, 0x00},
	{0x5304, 0x10},
	{0x5305, 0xCE},
	{0x5306, 0x05},
	{0x5307, 0xd0},
	{0x5680, 0x00},
	{0x5681, 0x50},
	{0x5682, 0x00},
	{0x5683, 0x3c},
	{0x5684, 0x11},
	{0x5685, 0xe0},
	{0x5686, 0x0d},
	{0x5687, 0x68},
	{0x5688, 0x03},
	{0x380C, 0x03},
	{0x380D, 0x14},
	{0x3818, 0xE0},
};

static struct msm_camera_i2c_conf_array ov7736_init_conf[] = {
	{ov7736_recommend_settings,
		ARRAY_SIZE(ov7736_recommend_settings), 0,
		MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_reg_conf const mode_snapshot_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const lensrolloff_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const mode_tp_disable_tbl[] = {
	{0x503D, 0x00}
};

static struct msm_camera_i2c_reg_conf const mode_tp_colorbars_tbl[] = {
	{ 0x3503, 0x03},
	{ 0x350a, 0x00},
	{ 0x350a, 0x10},
	{ 0x3500, 0x00},
	{ 0x3501, 0x04},
	{ 0x3502, 0xf0},
	{ 0x504e, 0x04},
	{ 0x504f, 0x00},
	{ 0x5050, 0x04},
	{ 0x5051, 0x00},
	{ 0x5052, 0x04},
	{ 0x5053, 0x00},
	{ 0x5186, 0x03},
	{ 0x5000, 0x4f},
	{ 0x503D, 0x80}
};

static struct msm_camera_i2c_reg_conf const mode_tp_checkerboard_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const effects_mono_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x22},
	{ 0x5583, 0x80},
	{ 0x5584, 0x80}
};

static struct msm_camera_i2c_reg_conf const effects_negative_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x42},
	{ 0x5583, 0x80},
	{ 0x5584, 0x80}
};

static struct msm_camera_i2c_reg_conf const effects_sepia_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x40},
	{ 0x5584, 0xa0}
};

static struct msm_camera_i2c_reg_conf const effects_redtint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x80},
	{ 0x5584, 0xc0}
};

static struct msm_camera_i2c_reg_conf const effects_bluetint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0xa0},
	{ 0x5584, 0x40}
};

static struct msm_camera_i2c_reg_conf const effects_greentint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x60},
	{ 0x5584, 0x60}
};

static struct msm_camera_i2c_reg_conf const effects_off_tbl[] = {
	{ 0x5001, 0x07},
	{ 0x5580, 0x02},
	{ 0x5583, 0x40},
	{ 0x5584, 0x40}
};

static struct msm_camera_i2c_conf_array ov7736_confs[] = {
	{&mode_preview_tbl[0],
		ARRAY_SIZE(mode_preview_tbl), 50, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct v4l2_subdev_info ov7736_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_sensor_output_info_t ov7736_dimensions[] = {
	{
		.x_output = 640,
		.y_output = 480,
		.line_length_pclk = 640,
		.frame_length_lines = 480,
		.vt_pixel_clk = 200000000,
		.op_pixel_clk = 200000000,
	},
	{
		.x_output = 640,
		.y_output = 480,
		.line_length_pclk = 640,
		.frame_length_lines = 480,
		.vt_pixel_clk = 200000000,
		.op_pixel_clk = 200000000,
	},
};

static struct msm_sensor_output_reg_addr_t ov7736_reg_addr = {
	.x_output = 0xFFFF,
	.y_output = 0xFFFF,
	.line_length_pclk = 0xFFFF,
	.frame_length_lines = 0xFFFF,
};

static struct msm_sensor_id_info_t ov7736_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x7736,
};

static int32_t ov7736_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct device *dev = NULL;
	struct msm_camera_sensor_info *info = s_ctrl->sensordata;

	dev = &s_ctrl->sensor_i2c_client->client->dev;

	pr_info("ov7736_power_up R:%d P:%d A:%d CLK:%d\n",
			info->sensor_reset,
			info->sensor_pwd,
			info->oem_data->sensor_avdd_en,
			info->oem_data->mclk_freq);

	if (!info->oem_data) {
		pr_err("%s: oem data NULL in sensor info, aborting",
				__func__);
		rc = -EINVAL;
		goto power_on_done;
	}

	/* obtain gpios */
	rc = gpio_request(info->sensor_pwd, "ov7736");
	if (rc) {
		pr_err("ov7736: gpio request DIGITAL_EN failed (%d)\n",
				rc);
		goto power_on_done;
	}
	rc = gpio_request(info->oem_data->sensor_avdd_en, "ov7736");
	if (rc) {
		pr_err("ov7736: gpio request ANALOG_EN failed (%d)\n",
				rc);
		goto power_on_done;
	}
	rc = gpio_request(info->sensor_reset, "ov7736");
	if (rc) {
		pr_err("ov7736: gpio request CAM2_RST_N failed (%d)\n", rc);
		goto power_on_done;
	}

	/* turn on anlog supply */
	gpio_direction_output(info->sensor_pwd, 1);

	usleep_range(5000, 5000);

	/* turn on pwdn supply */
	gpio_direction_output(info->oem_data->sensor_avdd_en, 1);
	usleep(20000);

	pr_info("ov7736 enabling MCLK\n");

	cam_mot_8960_clk_info->clk_rate = info->oem_data->mclk_freq;

	rc = msm_cam_clk_enable(dev, cam_mot_8960_clk_info,
			s_ctrl->cam_clk, ARRAY_SIZE(cam_mot_8960_clk_info), 1);
	if (rc < 0) {
		pr_err("OV7736: msm_cam_clk_enable failed (%d)\n", rc);
		goto power_on_done;
	}

	usleep(5000);

	pr_info("ov7736 setting PWRDWN Low\n");
	gpio_direction_output(info->sensor_pwd, 0);

	pr_info("ov7736 setting Reset High\n");
	gpio_direction_output(info->sensor_reset, 1);
	msleep(20);

	pr_info("ov7736 power up sequence complete\n");
power_on_done:
	return rc;
}

static int32_t ov7736_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	struct device *dev = NULL;
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = s_ctrl->sensordata;

	dev = &s_ctrl->sensor_i2c_client->client->dev;
	pr_info("ov7736_sensor_power_off\n");

	/*Disable MCLK*/
	rc = msm_cam_clk_enable(dev, cam_mot_8960_clk_info, s_ctrl->cam_clk,
			ARRAY_SIZE(cam_mot_8960_clk_info), 0);
	if (rc < 0)
		pr_err("ov8820: MCLK disable failed (%d)\n", rc);
	usleep(1000);

	/* Set Reset Low */
	gpio_direction_output(info->sensor_reset, 0);
	usleep(1000);

	/* Disable AVDD */
	gpio_direction_output(info->oem_data->sensor_avdd_en, 0);

	/* Set PWRDWN Low */
	gpio_direction_output(info->sensor_pwd, 0);

	/* Clean up */
	gpio_free(info->sensor_pwd);
	gpio_free(info->sensor_reset);
	gpio_free(info->oem_data->sensor_avdd_en);

	return 0;
}

static int32_t ov7736_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_WORD_DATA);

	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}

	pr_info("ov7736 chipid: %04x\n", chipid);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		pr_err("%s: chip id does not match\n", __func__);
		return -ENODEV;
	}
	pr_info("ov7736: match_id success\n");
	return 0;
}

/* FIX ME: Update stop stream with correct i2c wrie sequence
inorder to move sensor to proper state */
static void ov7736_stop_stream(struct msm_sensor_ctrl_t *s_ctrl) {}

#ifdef MOTO_FPSRATE
static int32_t ov7736_set_frame_rate_range(struct msm_sensor_ctrl_t *s_ctrl,
		struct fps_range_t *fps_range)
{
	int32_t rc = 0;

	if (fps_range->max_fps != fps_range->min_fps) {
		pr_err("%s: FPS is incorrect\n", __func__);
		rc = -EINVAL;
		goto frame_rate_done;
	}

	if (fps_range->max_fps == 30) {
		rc = msm_camera_i2c_write_tbl(
				s_ctrl->sensor_i2c_client,
				ov7736_30_30_fps_settings,
				ARRAY_SIZE(ov7736_30_30_fps_settings),
				MSM_CAMERA_I2C_BYTE_DATA);
	} else if (fps_range->max_fps == 15) {
		rc = msm_camera_i2c_write_tbl(
				s_ctrl->sensor_i2c_client,
				ov7736_15_15_fps_settings,
				ARRAY_SIZE(ov7736_15_15_fps_settings),
				MSM_CAMERA_I2C_BYTE_DATA);
	} else {
		rc = -EINVAL;
		pr_err("%s: failed to set frame rate range (%d)\n", __func__,
				rc);
		goto frame_rate_done;
	}


frame_rate_done:
	return rc;
}
#endif

static int32_t ov7736_set_gamma(struct msm_sensor_ctrl_t *s_ctrl, uint8_t unity)
{
	int32_t rc = 0;
	uint16_t readdata;
	uint8_t data;

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			0x5000, &readdata,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: read gamma register failed\n", __func__);
		return rc;
	}

	data = (uint8_t)readdata;

	if (unity)
		data |= 0x40;
	else
		data &= 0xBF;

	rc = msm_camera_i2c_write(
			s_ctrl->sensor_i2c_client,
			0x5000, data,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: write gamma register failed\n", __func__);
		return rc;
	}

	return rc;
}

static int32_t ov7736_set_sharpening(struct msm_sensor_ctrl_t *s_ctrl,
		uint8_t sharpening)
{
	int32_t rc = 0;
	uint8_t data = 0x00;
	uint16_t readdata;

	if (sharpening == 0) {
		rc = msm_camera_i2c_read(
				s_ctrl->sensor_i2c_client,
				0x5300, &readdata,
				MSM_CAMERA_I2C_BYTE_DATA);

		if (rc < 0) {
			pr_err("%s: read sharpening 1 register failed\n",
					__func__);
			return rc;
		}

		sharpening_saved_5300 = readdata;

		rc = msm_camera_i2c_read(
				s_ctrl->sensor_i2c_client,
				0x5301, &readdata,
				MSM_CAMERA_I2C_BYTE_DATA);

		if (rc < 0) {
			pr_err("%s: read sharpening 2 register failed\n",
					__func__);
			return rc;
		}

		sharpening_saved_5301 = readdata;

		rc = msm_camera_i2c_write(
				s_ctrl->sensor_i2c_client,
				0x5300, data,
				MSM_CAMERA_I2C_BYTE_DATA);

		if (rc < 0) {
			pr_err("%s: write sharpening 1 register failed\n",
					__func__);
			return rc;
		}

		rc = msm_camera_i2c_write(
				s_ctrl->sensor_i2c_client,
				0x5301, data,
				MSM_CAMERA_I2C_BYTE_DATA);

		if (rc < 0) {
			pr_err("%s: write sharpening 2 register failed\n",
					__func__);
			return rc;
		}
	} else {
		if ((sharpening_saved_5300 != 0x0000) &&
				(sharpening_saved_5301 != 0x0000)) {
			data = (uint8_t)sharpening_saved_5300;
			rc = msm_camera_i2c_write(
					s_ctrl->sensor_i2c_client,
					0x5300, data,
					MSM_CAMERA_I2C_BYTE_DATA);

			if (rc < 0) {
				pr_err("%s: write sharpening 1 reg failed\n",
						__func__);
				return rc;
			}

			data = (uint8_t)sharpening_saved_5301;
			rc = msm_camera_i2c_write(
					s_ctrl->sensor_i2c_client,
					0x5301, data,
					MSM_CAMERA_I2C_BYTE_DATA);

			if (rc < 0) {
				pr_err("%s: write sharpening 2 reg failed\n",
						__func__);
				return rc;
			}
		} else {
			pr_err("%s: write sharpening failed\n",
					__func__);
			return -EINVAL;
		}
	}

	return rc;
}

static int32_t ov7736_set_lens_shading(struct msm_sensor_ctrl_t *s_ctrl,
		uint8_t enable)
{
	int32_t rc = 0;
	uint16_t readdata;
	uint8_t data;

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			0x5000, &readdata,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: read lens shading register failed\n", __func__);
		return rc;
	}

	data = (uint8_t)readdata;

	if (enable)
		data |= 0x80;
	else
		data &= 0x7F;

	rc = msm_camera_i2c_write(
			s_ctrl->sensor_i2c_client,
			0x5000, data,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: write gamma register failed\n", __func__);
		return rc;
	}

	return rc;
}

static int32_t ov7736_set_target_exposure(struct msm_sensor_ctrl_t *s_ctrl,
		uint8_t target_exposure)
{
	int32_t rc = 0;
	uint8_t low_limit = 0;
	uint8_t high_limit = 0;

	if (target_exposure <= 4) {
		high_limit = 4;
		low_limit = 0;
	} else {
		high_limit = target_exposure;
		low_limit = high_limit - 4;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3A0F, high_limit,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: Write tgt exp average luma register failed\n",
				__func__);
		return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3A10, low_limit,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: Write tgt exp average luma dark register failed\n",
				__func__);
		return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3A1E, high_limit,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: Write tgt exp average luma dark register failed\n",
				__func__);
		return rc;
	}

	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3A1B, low_limit,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: Write tgt exp average luma dark register failed\n",
				__func__);
		return rc;
	}

	return rc;
}

static int32_t ov7736_get_exposure_time(struct msm_sensor_ctrl_t *s_ctrl,
		uint32_t *ex_time)
{
	uint32_t exposure_time = 0;
	uint16_t  reg16_19 = 0;
	uint16_t  reg8_15 = 0;
	uint16_t  reg0_7 = 0;
	int32_t rc = 0;

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			0x3500, &reg16_19,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			0x3501, &reg8_15,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			0x3502, &reg0_7,
			MSM_CAMERA_I2C_BYTE_DATA);

	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}
	exposure_time = (exposure_time | (uint8_t)(reg16_19 & 0x0F)) << 16;
	exposure_time = exposure_time | (reg8_15 << 8);
	exposure_time = exposure_time | (uint8_t) reg0_7;

	pr_debug("exposure_time %x\n", exposure_time);
	*ex_time = exposure_time;
	return rc;
}

static const struct i2c_device_id ov7736_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov7736_s_ctrl},
	{ }
};

static struct i2c_driver ov7736_i2c_driver = {
	.id_table = ov7736_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov7736_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


static int __init ov7736_init_module(void)
{
	return i2c_add_driver(&ov7736_i2c_driver);
}

static struct v4l2_subdev_core_ops ov7736_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov7736_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov7736_subdev_ops = {
	.core = &ov7736_subdev_core_ops,
	.video  = &ov7736_subdev_video_ops,
};

static struct msm_sensor_fn_t ov7736_func_tbl = {
	.sensor_start_stream    = msm_sensor_start_stream,
	.sensor_stop_stream     = ov7736_stop_stream,
	.sensor_setting         = msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init       = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config          = msm_sensor_config,
	.sensor_power_up        = ov7736_sensor_power_up,
	.sensor_power_down      = ov7736_sensor_power_down,
	.sensor_match_id        = ov7736_sensor_match_id,
	.sensor_csi_setting     = msm_sensor_setting1,
	.sensor_get_csi_params  = msm_sensor_get_csi_params,
	.sensor_set_gamma              = ov7736_set_gamma,
	.sensor_set_sharpening         = ov7736_set_sharpening,
	.sensor_set_lens_shading       = ov7736_set_lens_shading,
	.sensor_set_target_exposure    = ov7736_set_target_exposure,
	.sensor_get_exposure_time      = ov7736_get_exposure_time,
#ifdef MOTO_FPSRATE
	.sensor_set_frame_rate_range   = ov7736_set_frame_rate_range,
#endif
};

static struct msm_sensor_reg_t ov7736_regs = {
	.default_data_type      = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf      = ov7736_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov7736_start_settings),
	.stop_stream_conf       = ov7736_stop_settings,
	.stop_stream_conf_size  = ARRAY_SIZE(ov7736_stop_settings),
	.init_settings          = &ov7736_init_conf[0],
	.init_size              = ARRAY_SIZE(ov7736_init_conf),
	.output_settings        = &ov7736_dimensions[0],
	.mode_settings          = &ov7736_confs[0],
	.num_conf               = ARRAY_SIZE(ov7736_confs),
};

static struct msm_sensor_ctrl_t ov7736_s_ctrl = {
	.msm_sensor_reg               = &ov7736_regs,
	.sensor_i2c_client            = &ov7736_sensor_i2c_client,
	.sensor_i2c_addr              = 0x78,

	.sensor_output_reg_addr       = &ov7736_reg_addr,
	.sensor_id_info               = &ov7736_id_info,

	.cam_mode                     = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex             = &ov7736_mut,
	.sensor_i2c_driver            = &ov7736_i2c_driver,
	.sensor_v4l2_subdev_info      = ov7736_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov7736_subdev_info),
	.sensor_v4l2_subdev_ops       = &ov7736_subdev_ops,
	.func_tbl                     = &ov7736_func_tbl,
};

module_init(ov7736_init_module);
MODULE_DESCRIPTION("OMNIVISION VGA YUV sensor driver");
MODULE_LICENSE("GPL v2");
