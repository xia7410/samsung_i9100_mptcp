/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Authors:
 *	Eunchul Kim <chulspro.kim@samsung.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _EXYNOS_DRM_IPP_H_
#define _EXYNOS_DRM_IPP_H_

#define IPP_GET_LCD_WIDTH	_IOR('F', 302, int)
#define IPP_GET_LCD_HEIGHT	_IOR('F', 303, int)
#define IPP_SET_WRITEBACK	_IOW('F', 304, u32)

/*
 * A structure of source,destination operations.
 *
 * @set_fmt: set format of image.
 * @set_transf: set transform(rotations, flip).
 * @set_size: set size of region.
 * @set_addr: set address for dma.
 */
struct exynos_drm_ipp_ops {
	int (*set_fmt)(struct device *dev, u32 fmt);
	int (*set_transf)(struct device *dev,
		enum drm_exynos_degree degree,
		enum drm_exynos_flip flip);
	int (*set_size)(struct device *dev, int swap,
		struct drm_exynos_pos *pos, struct drm_exynos_sz *sz);
	int (*set_addr)(struct device *dev, dma_addr_t *base, u32 id,
		enum drm_exynos_ipp_buf_ctrl buf_ctrl);
};

/*
 * A structure of ipp driver.
 *
 * @list: list head.
 * @dev: platform device.
 * @drm_dev: drm device.
 * @used : used status.
 * @cmd: used command.
 * @ops: source, destination operations.
 * @property: ipp driver property informations.
 * @event: event for user.
 * @event_list: event list.
 * @map_list: list head to map information.
 * @open: this would be called with drm device file open.
 * @close: this would be called with drm device file close.
 * @reset: reset ipp block
 * @check_valid: check valid about format, size, buffer.
 * @start: ipp each device start.
 * @check_prepare: check prepare about format, size, buffer.
 * @stop: ipp each device stop.
 */
struct exynos_drm_ippdrv {
	struct list_head	list;
	struct device	*dev;
	struct drm_device	*drm_dev;
	bool	used;
	enum drm_exynos_ipp_cmd	cmd;
	struct exynos_drm_ipp_ops	*ops[EXYNOS_DRM_OPS_MAX];
	struct drm_exynos_ipp_property	property;
	struct list_head	event_list;
	struct list_head	map_list;

	int (*open)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);
	void (*close)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);
	int (*check_valid)(struct device *dev,
		struct drm_exynos_ipp_property *property);
	int (*reset)(struct device *dev);
	int (*start)(struct device *dev, enum drm_exynos_ipp_cmd cmd);
	int (*check_prepare)(struct device *dev,
		struct exynos_drm_ippdrv *ippdrv);
	void (*stop)(struct device *dev, enum drm_exynos_ipp_cmd cmd);
};

#ifdef CONFIG_DRM_EXYNOS_IPP
extern int exynos_drm_ippdrv_register(struct exynos_drm_ippdrv *ippdrv);
extern int exynos_drm_ippdrv_unregister(struct exynos_drm_ippdrv *ippdrv);
extern int exynos_drm_ipp_property(struct drm_device *drm_dev, void *data,
					 struct drm_file *file);
extern int exynos_drm_ipp_buf(struct drm_device *drm_dev, void *data,
					 struct drm_file *file);
extern int exynos_drm_ipp_ctrl(struct drm_device *drm_dev, void *data,
					 struct drm_file *file);
extern int exynos_drm_ippnb_register(struct notifier_block *nb);
extern int exynos_drm_ippnb_unregister(struct notifier_block *nb);
extern int exynos_drm_ippnb_send_event(unsigned long val, void *v);
extern int exynos_drm_ipp_init(struct drm_device *dev);
extern void exynos_drm_ipp_fini(struct drm_device *dev);
#else
static inline int exynos_drm_ippdrv_register(struct exynos_drm_ippdrv *ippdrv)
{
	return -ENODEV;
}

static inline int exynos_drm_ippdrv_unregister(struct exynos_drm_ippdrv *ippdrv)
{
	return -ENODEV;
}

static inline int exynos_drm_ipp_property(struct drm_device *drm_dev,
						void *data,
						struct drm_file *file_priv)
{
	return -ENOTTY;
}
static inline int exynos_drm_ipp_buf(struct drm_device *drm_dev,
						void *data,
						struct drm_file *file)
{
	return -ENOTTY;
}
static inline int exynos_drm_ipp_ctrl(struct drm_device *drm_dev,
						void *data,
						struct drm_file *file)
{
	return -ENOTTY;
}

static inline int exynos_drm_ippnb_register(struct notifier_block *nb)
{
	return -ENODEV;
}

static inline int exynos_drm_ippnb_unregister(struct notifier_block *nb)
{
	return -ENODEV;
}

static inline int exynos_drm_ippnb_send_event(unsigned long val, void *v)
{
	return -ENOTTY;
}

static inline int exynos_drm_ipp_init(struct drm_device *dev)
{
	return -ENODEV;
}

static inline void exynos_drm_ipp_fini(struct drm_device *dev)
{
	return;
}
#endif

void ipp_send_event_handler(struct exynos_drm_ippdrv *ippdrv,
	int buf_idx);

#endif /* _EXYNOS_DRM_IPP_H_ */

