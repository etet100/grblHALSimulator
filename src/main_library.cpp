
#include <QCoreApplication>
#include <QLocalSocket>
#include "WindowsSerial.h"
#include <QAtomicInt>


#ifdef __cplusplus
extern "C"
{
#endif

// #include "grbl/system.h"

void gpilotLockProbeAtCurrentPosition(void) {}
void gpilotResetProbePosition(void) {}
void gpilotSetHome(bool abs, double x, double y, double z) {}

void gpilotSetSingleLimit(int axis, double pos) {

}
void gpilotEstop();

// #define bit(n) (1UL << (n))
// #define EXEC_RESET              bit(5)

// void gpilotEstop()
// {
//     // system_set_exec_state_flag(EXEC_RESET);
//     // // Alarm_EStop = 10,                           //!< 10
//     // system_set_exec_alarm(10);
// }

#include "simulator.h"
#include "eeprom.h"
#include "grbl_interface.h"

#include "grbl/grbllib.h"

arg_vars_t args;
QLocalSocket* mainSocket = nullptr;
QLocalSocket* controlSocket = nullptr;
QAtomicInt* stopFlag_;

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
        mainSocket->write((const char *)buf, len);
        len = 0;
    }
}

//return char if one available.
uint8_t serial_in()
{
    static QString ctrlBuffer = "";
    if (controlSocket->bytesAvailable()) {
        ctrlBuffer += controlSocket->readAll();

        int pos;
        while ((pos = ctrlBuffer.indexOf("\n")) != -1) {
            QString line = ctrlBuffer.left(pos).trimmed();
            ctrlBuffer = ctrlBuffer.mid(pos + 1);

            qDebug() << "[IO][GRBL][DLL] Received:" << line;

            // Process control commands here
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
            if (!doc.isNull() && doc.isObject()) {
                QJsonObject obj = doc.object();
                QString cmd = obj["cmd"].toString();

                if (cmd == "probe_at_current") {
                    gpilotLockProbeAtCurrentPosition();
                } else if (cmd == "reset_probe") {
                    gpilotResetProbePosition();
                } else if (cmd == "set_home") {
                    gpilotSetHome(
                        obj["abs"].toBool(),
                        obj["x"].toDouble(),
                        obj["y"].toDouble(),
                        obj["z"].toDouble()
                    );
                } else if (cmd == "set_single_limit") {
                    //{\"axis\":2,\"cmd\":\"set_single_limit\",\"pos\":25}"
                    gpilotSetSingleLimit(
                        obj["axis"].toInt(),
                        obj["pos"].toDouble()
                    );
                } else if (cmd == "estop") {
                    gpilotEstop();
                }
            }
        }
    }

    if (!mainSocket->bytesAvailable()) {
        return 0;
    }

    char c;
    mainSocket->read(&c, 1);

    return c;
}

void per_tick()
{
    static uint32_t tick = 0;
    if (tick++ > 10000) {
        tick = 0;
        QCoreApplication::processEvents();
        if (*stopFlag_ == 2) { // 2 == stop requested
            sim.exit = sim.exit_OK;
        }

        // process control socket
        if (controlSocket->bytesAvailable()) {
            static QString controlBuffer = "";
            controlBuffer += controlSocket->readAll();

            while (true) {
                if (controlBuffer.isEmpty()) {
                    return;
                }
                int pos = controlBuffer.indexOf("\n");
                if (pos == -1) {
                    return;
                }

                QString line = controlBuffer.left(pos).trimmed();
                controlBuffer.remove(0, pos + 1);

                qDebug() << "[IO][GRBL][DLL] Control command received:" << line;
            }
        }
    }
}

Q_DECL_EXPORT
void GRBL(QString serverName, QAtomicInt* stopFlag)
{
    stopFlag_ = stopFlag;

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

    mainSocket = new QLocalSocket();
    mainSocket->connectToServer(serverName);
    if (!mainSocket->waitForConnected(100)) {
        qDebug() << "[IO][GRBL][DLL] Could not connect to server main channel:" << mainSocket->errorString();
    }

    controlSocket = new QLocalSocket();
    controlSocket->connectToServer(serverName);
    if (!controlSocket->waitForConnected(100)) {
        qDebug() << "[IO][GRBL][DLL] Could not connect to server control channel:" << controlSocket->errorString();
    }

    qDebug() << "[IO][GRBL][DLL] Connected to server, starting simulator.";

    init_simulator();

    plat_thread_t *th = platform_start_thread(grbl_main_thread);
    if (!th){
        qDebug() << "[IO][GRBL][DLL] Fatal: Unable to start hardware thread.";

        printf("Fatal: Unable to start hardware thread.\n");
        exit(-5);
    }

    qDebug() << "[IO][GRBL][DLL] Starting loop.";

    sim_loop(th);

    eeprom_close();
    mainSocket->disconnectFromServer();
    controlSocket->disconnectFromServer();
    platform_kill_thread(th);

    qDebug() << "[IO][GRBL][DLL] Exiting.";
}

#ifdef __cplusplus
}
#endif
