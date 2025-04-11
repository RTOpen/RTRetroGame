CSRCS += lv_100ask_dbus_message_dispatch.c

DEPPATH += --dep-path $(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_message_dispatch/
VPATH += :$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_message_dispatch/

CFLAGS += "-I$(LVGL_DIR)/$(MODULES_DIR_NAME)/src/lv_100ask_dbus_message_dispatch/"
