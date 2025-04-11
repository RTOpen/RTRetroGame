CSRCS += lv_100ask_dbus_handler.c

DEPPATH += --dep-path $(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_handler/
VPATH += :$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_handler/

CFLAGS += "-I$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_handler/"
