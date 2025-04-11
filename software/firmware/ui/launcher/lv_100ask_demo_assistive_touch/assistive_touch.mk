CSRCS += assistive_touch.c

DEPPATH += --dep-path $(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_assistive_touch/
VPATH += :$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_assistive_touch/

CFLAGS += "-I$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_demo_assistive_touch/"
