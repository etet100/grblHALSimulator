
#include <QCoreApplication>
#include <QLocalSocket>
#include "WindowsSerial.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "simulator.h"
#include "eeprom.h"
#include "grbl_interface.h"

#include "grbl/grbllib.h"

arg_vars_t args;
QLocalSocket* socket_ = nullptr;

PLAT_THREAD_FUNC(grbl_main_thread, exit)
{
    grbl_enter();

    return 0; //NULL;
}

// Print serial output to args.serial_out_file
void serial_out(uint8_t data)
{
    static uint8_t buf[256] = {0};
    static uint8_t len = 0;

    buf[len++] = data;
    // print when we get to newline or run out of buffer
    if (data == '\n' || data == '\r' || len >= 127) {
        socket_->write((const char *)buf, len);
        len = 0;
    }
}

//return char if one available.
uint8_t serial_in()
{
    if (!socket_->bytesAvailable()) {
        return 0;
    }

    char c;
    socket_->read(&c, 1);

    return c;
}

void per_tick()
{
    static uint32_t tick = 0;
    if (tick++ > 10000) {
        tick = 0;
        QCoreApplication::processEvents();
    }
}

Q_DECL_EXPORT
void GRBL(QString serverName)
{
    //defaults
    args.step_out_file = stderr;
    args.block_out_file = stdout;
    args.serial_out_file = stdout;
    args.comment_char = '#';
    args.speedup = 1.0f;
    args.step_time = 0.0f;
    // Get the minimum time step for printing stepper values.
    // If not given or the command line cannot be parsed to a float than
    // step_time= 0.0; This means to not print stepper values at all

    set_eeprom_name("grbl_eeprom.dat");

    platform_init();

    sim.on_init = grbl_app_init;
    sim.on_shutdown = grbl_app_exit;
    sim.on_tick = per_tick;
    sim.on_byte = grbl_per_byte;
    sim.getchar = serial_in;
    sim.putchar = serial_out;

    socket_ = new QLocalSocket();
    socket_->connectToServer(serverName);
    if (!socket_->waitForConnected(100)) {
        qDebug() << "Could not connect to server:" << socket_->errorString();
    }

    init_simulator();

    plat_thread_t *th = platform_start_thread(grbl_main_thread);
    if (!th){
        printf("Fatal: Unable to start hardware thread.\n");
        exit(-5);
    }

    sim_loop(th);

    eeprom_close();
}

#ifdef __cplusplus
}
#endif
