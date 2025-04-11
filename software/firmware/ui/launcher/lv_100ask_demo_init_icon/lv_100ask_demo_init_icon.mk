CSRCS += lv_100ask_demo_init_icon.c

DEPPATH += --dep-path $(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_init_icon/
VPATH += :$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_init_icon/

CFLAGS += "-I$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_init_icon/"
