lessThan(QT_MAJOR_VERSION, 6) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 6.8 or newer")
}
equals(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 8) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 6.8 or newer")
}

TEMPLATE = lib
QT += network
TARGET = grblHal
DEFINES += F_CPU=16000000
DEFINES += INJECT_BLOCK_BUFFER_SIZE=256

CONFIG -= debug_and_release
LIBS += -pthread -lws2_32

win32 {
    DEFINES += WINDOWS=1
    DEFINES += WIN32
}

unix {
    DEFINES += LINUX=1
}

DISTFILES += \
    src/grbl/.gitattributes \
    src/grbl/.gitignore \
    src/grbl/CMakeLists.txt \
    src/grbl/COPYING \
    src/grbl/README.md \
    src/grbl/changelog.md

HEADERS += \
    src/WindowsSerial.h \
    src/driver.h \
    src/eeprom.h \
    src/grbl/alarms.h \
    src/grbl/canbus.h \
    src/grbl/config.h \
    src/grbl/coolant_control.h \
    src/grbl/core_handlers.h \
    src/grbl/crc.h \
    src/grbl/crossbar.h \
    src/grbl/driver_opts.h \
    src/grbl/driver_opts2.h \
    src/grbl/errors.h \
    src/grbl/gcode.h \
    src/grbl/grbl.h \
    src/grbl/grbllib.h \
    src/grbl/hal.h \
    src/grbl/ioports.h \
    src/grbl/kinematics.h \
    src/grbl/kinematics/corexy.h \
    src/grbl/kinematics/delta.h \
    src/grbl/kinematics/maslow.h \
    src/grbl/kinematics/polar.h \
    src/grbl/kinematics/wall_plotter.h \
    src/grbl/machine_limits.h \
    src/grbl/messages.h \
    src/grbl/modbus.h \
    src/grbl/motion_control.h \
    src/grbl/motor_pins.h \
    src/grbl/ngc_expr.h \
    src/grbl/ngc_flowctrl.h \
    src/grbl/ngc_params.h \
    src/grbl/nuts_bolts.h \
    src/grbl/nvs.h \
    src/grbl/nvs_buffer.h \
    src/grbl/override.h \
    src/grbl/pid.h \
    src/grbl/pin_bits_masks.h \
    src/grbl/planner.h \
    src/grbl/platform.h \
    src/grbl/plugins.h \
    src/grbl/plugins_init.h \
    src/grbl/probe.h \
    src/grbl/protocol.h \
    src/grbl/regex.h \
    src/grbl/report.h \
    src/grbl/rgb.h \
    src/grbl/settings.h \
    src/grbl/sleep.h \
    src/grbl/spindle_control.h \
    src/grbl/spindle_sync.h \
    src/grbl/state_machine.h \
    src/grbl/stepdir_map.h \
    src/grbl/stepper.h \
    src/grbl/stepper2.h \
    src/grbl/stream.h \
    src/grbl/stream_file.h \
    src/grbl/stream_passthru.h \
    src/grbl/strutils.h \
    src/grbl/system.h \
    src/grbl/task.h \
    src/grbl/tool_change.h \
    src/grbl/vfs.h \
    src/grbl_eeprom_extensions.h \
    src/grbl_interface.h \
    src/mcu.h \
    src/platform.h \
    src/platform_windows.h \
    src/serial.h \
    src/simulator.h

SOURCES += \
    src/WindowsSerial.cpp \
    src/driver.c \
    src/eeprom.c \
    src/grbl/alarms.c \
    src/grbl/canbus.c \
    src/grbl/coolant_control.c \
    src/grbl/crc.c \
    src/grbl/crossbar.c \
    src/grbl/errors.c \
    src/grbl/gcode.c \
    src/grbl/grbllib.c \
    src/grbl/ioports.c \
    src/grbl/kinematics/corexy.c \
    src/grbl/kinematics/delta.c \
    src/grbl/kinematics/maslow.c \
    src/grbl/kinematics/polar.c \
    src/grbl/kinematics/wall_plotter.c \
    src/grbl/machine_limits.c \
    src/grbl/messages.c \
    src/grbl/modbus.c \
    src/grbl/motion_control.c \
    src/grbl/my_plugin.c \
    src/grbl/ngc_expr.c \
    src/grbl/ngc_flowctrl.c \
    src/grbl/ngc_params.c \
    src/grbl/nuts_bolts.c \
    src/grbl/nvs_buffer.c \
    src/grbl/override.c \
    src/grbl/pid.c \
    src/grbl/planner.c \
    src/grbl/protocol.c \
    src/grbl/regex.c \
    src/grbl/report.c \
    src/grbl/settings.c \
    src/grbl/sleep.c \
    src/grbl/spindle_control.c \
    src/grbl/state_machine.c \
    src/grbl/stepper.c \
    src/grbl/stepper2.c \
    src/grbl/stream.c \
    src/grbl/stream_file.c \
    src/grbl/stream_passthru.c \
    src/grbl/strutils.c \
    src/grbl/system.c \
    src/grbl/tool_change.c \
    src/grbl/vfs.c \
    src/grbl_eeprom_extensions.c \
    src/grbl_interface.c \
    # src/main.c \
    src/main_library.cpp \
    src/mcu.c \
    src/planner_inject_accessors.c \
    src/platform_windows.c \
    src/serial.c \
    src/simulator.c \
    # src/validator.c \
    # src/validator_driver.c
