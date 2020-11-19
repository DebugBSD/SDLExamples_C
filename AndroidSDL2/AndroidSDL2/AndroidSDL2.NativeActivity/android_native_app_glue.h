/*
 * Copyright (C) 2010 El proyecto de código abierto de Android
 *
 * Concedido bajo la licencia de Apache, versión 2.0 (la "Licencia");
 * solo podrá usar este archivo en cumplimiento con la Licencia.
 * Puede obtener una copia de la Licencia en
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * A menos que lo exija la legislación vigente o que se convenga por escrito, el software
 * distribuido bajo la Licencia se distribuye "TAL CUAL",
 * SIN GARANTÍAS NI CONDICIONES DE NINGÚN TIPO, ni expresas ni implícitas.
 * Vea la Licencia para consultar el lenguaje específico que rige los permisos y
 * las limitaciones en virtud de la Licencia.
 *
 */

#ifndef _ANDROID_NATIVE_APP_GLUE_H
#define _ANDROID_NATIVE_APP_GLUE_H

#include <poll.h>
#include <pthread.h>
#include <sched.h>

#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * La interfaz de actividad nativa proporcionada por <android/native_activity.h>
 * se basa en un conjunto de devoluciones de llamada suministradas por la aplicación a las que llamará
 * el subproceso principal de la actividad cuando ocurran ciertos eventos.
 *
 * Esto significa que estas devoluciones de llamada _no_ _deben_ bloquear; si no, se corre
 * el riesgo de que el sistema fuerce el cierre de la aplicación. Este modelo
 * de programación es directo y ligero, pero restrictivo.
 *
 * La biblioteca estática 'threaded_native_app' se usa para proporcionar un modelo
 * de ejecución distinto en el que la aplicación puede implementar su propio bucle de eventos
 * principales en otro subproceso. Funciona del modo siguiente:
 *
 * 1/ La aplicación debe proporcionar una función denominada "android_main()" a la que
 *    se llamará cuando se cree la actividad en un subproceso nuevo que es
 *    diferente del subproceso principal de la actividad.
 *
 * 2/ android_main() recibe un puntero a una estructura "android_app" válida
 *    que contiene referencias a otros objetos importantes, como la
 *    instancia del objeto ANativeActivity en la que la aplicación se ejecuta.
 *
 * 3/ El objeto "android_app" retiene una instancia de ALooper que ya
 *    escucha dos cosas importantes:
 *
 *      - los eventos del ciclo de vida de la actividad (p. ej., "pausar", "reanudar"). Vea las declaraciones de APP_CMD_XXX
 *        abajo.
 *
 *      - los eventos de entrada procedentes de AInputQueue asociada a la actividad.
 *
 *    Cada uno de estos se corresponde con un identificador ALooper devuelto por
 *    ALooper_pollOnce con valores de LOOPER_ID_MAIN y LOOPER_ID_INPUT,
 *    respectivamente.
 *
 *    Su aplicación puede usar el mismo ALooper para escuchar descriptores de archivo
 *    adicionales. Pueden estar basados en devoluciones de llamada o pueden tener
 *    identificadores de devolución que empiezan por LOOPER_ID_USER.
 *
 * 4/ Cuando reciba un evento LOOPER_ID_MAIN o LOOPER_ID_INPUT,
 *    los datos devueltos apuntarán a una estructura android_poll_source.  En ella, puede
 *    llamar a la función process() y rellenar android_app->onAppCmd
 *    y android_app->onInputEvent para llamarlos y procesar usted mismo
 *    el evento.
 *
 *    Otra opción es llamar a las funciones de bajo nivel para leer y procesar
 *    los datos directamente y...  fijarse en las implementaciones process_cmd() y process_input()
 *    de la adherencia para consultar cómo hacerlo.
 *
 * Vea la muestra titulada "native-activity" que acompaña al NDK con un
 * ejemplo de uso completo. También puede consultar el JavaDoc de NativeActivity.
 */

struct android_app;

/**
 * Datos asociados con un ALooper fd que se devolverá como "outData"
 * cuando ese origen tenga listos los datos.
 */
struct android_poll_source {
    // El identificador de este origen. Puede ser LOOPER_ID_MAIN o
    // LOOPER_ID_INPUT.
    int32_t id;

    // La android_app a la que está asociada esta sangría.
    struct android_app* app;

    // Función a la que llamar para realizar el procesamiento estándar de datos desde
    // este origen.
    void (*process)(struct android_app* app, struct android_poll_source* source);
};

/**
 * Esta es la interfaz para el código estándar de adherencias de una aplicación
 * de subprocesos. En este modelo, el código de la aplicación se ejecuta
 * en su propio subproceso, que es independiente del subproceso principal del proceso.
 * No es necesario que este subproceso esté asociado con la máquina virtual
 * de Java, pero deberá estarlo para poder hacer llamadas JNI a
 * objetos Java.
 */
struct android_app {
    // La aplicación puede colocar un puntero en su propio objeto de estado
    // aquí si quiere.
    void* userData;

    // Rellene esto con la función para procesar comandos de aplicación principales (APP_CMD_*)
    void (*onAppCmd)(struct android_app* app, int32_t cmd);

    // Rellene esto con la función para procesar eventos de entrada. En este momento
    // el evento ya se ha distribuido previamente y se terminará al
    // devolverse. Devuelva 1 si ha administrado el evento y 0 para las distribuciones
    // predeterminadas.
    int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);

    // La instancia del objeto ANativeActivity en la que se ejecuta esta aplicación.
    ANativeActivity* activity;

    // La configuración actual en la que se ejecuta la aplicación.
    AConfiguration* config;

    // Este es el último estado guardado de la instancia que aparece en la hora de creación.
    // Es NULL si no había ningún estado. Puede usar esto cuando lo necesite; la
    // memoria permanecerá hasta que llame a android_app_exec_cmd() para
    // APP_CMD_RESUME; en este momento se liberará y savedState se establecerá en NULL.
    // Estas variables solo deben cambiarse al procesar un APP_CMD_SAVE_STATE,
    // momento en el cual se inicializarán en NULL y podrá asignar la memoria de su
    // estado y colocar la información aquí. En ese caso, la memoria
    // se liberará más tarde.
    void* savedState;
    size_t savedStateSize;

    // El ALooper asociado con el subproceso de la aplicación.
    ALooper* looper;

    // Cuando no es NULL, esta es la cola de entrada desde la que la aplicación
    // recibirá los eventos de entrada del usuario.
    AInputQueue* inputQueue;

    // Cuando no es NULL, esta es la superficie de ventana en la que puede dibujar la aplicación.
    ANativeWindow* window;

    // Rectángulo del contenido actual de la ventana; esta es el área donde el
    // contenido de la ventana debe colocarse para que el usuario lo vea.
    ARect contentRect;

    // Estado actual de la actividad de la aplicación. Puede ser APP_CMD_START,
    // APP_CMD_RESUME, APP_CMD_PAUSE o APP_CMD_STOP; véase abajo.
    int activityState;

    // Esto no es cero cuando la NativeActivity de la aplicación se va a
    // destruir y se espera a que el subproceso de la aplicación termine.
    int destroyRequested;

    // -------------------------------------------------
    // Abajo están las implementaciones "privadas" del código de adherencia.

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int msgread;
    int msgwrite;

    pthread_t thread;

    struct android_poll_source cmdPollSource;
    struct android_poll_source inputPollSource;

    int running;
    int stateSaved;
    int destroyed;
    int redrawNeeded;
    AInputQueue* pendingInputQueue;
    ANativeWindow* pendingWindow;
    ARect pendingContentRect;
};

enum {
    /**
     * Identificador de los datos del generador de bucles de los comandos procedentes del subproceso principal de la aplicación, que
     * se devuelve como identificador desde ALooper_pollOnce().  Los datos de este
     * identificador son un puntero  a una estructura android_poll_source.
     * Estos se pueden recuperar y procesar con android_app_read_cmd()
     * y android_app_exec_cmd().
     */
    LOOPER_ID_MAIN = 1,

    /**
     * Identificador de los datos del generador de bucles de los eventos procedentes de la AInputQueue de la
     * ventana de la aplicación, que se devuelve como identificador desde
     * ALooper_pollOnce(). Los datos de este identificador son un puntero a una estructura
     * android_poll_source. Estos se pueden leer a través del objeto inputQueue
     * de android_app.
   */
    LOOPER_ID_INPUT = 2,

    /**
     * Inicio de identificadores ALooper definidos por el usuario.
     */
    LOOPER_ID_USER = 3,
};

enum {
    /**
     * Comando del subproceso principal: la AInputQueue ha cambiado.  Al procesar
     * este comando, android_app->inputQueue se actualizará a la nueva cola
     * (o a NULL).
     */
    APP_CMD_INPUT_CHANGED,

    /**
     * Comando del subproceso principal: una nueva ANativeWindow está lista para su uso. Al recibir
     * este comando, android_app->window contendrá la nueva superficie
     * de ventana.
     */
    APP_CMD_INIT_WINDOW,

    /**
     * Comando del subproceso principal: es necesario cerrar
     * la ANativeWindow existente. Al recibir este comando, android_app->window sigue
     * conteniendo la ventana existente; después de llamar a android_app_exec_cmd
     * se establecerá en NULL.
     */
    APP_CMD_TERM_WINDOW,

    /**
     * Comando del subproceso principal: se ha cambiado el tamaño de la ANativeWindow actual.
     * Vuelva a dibujarla con el nuevo tamaño.
     */
    APP_CMD_WINDOW_RESIZED,

    /**
     * Comando del subproceso principal: el sistema exige que la ANativeWindow actual
     * vuelva a dibujarse. Tiene que volver a dibujar la ventana antes de entregar esto a
     * android_app_exec_cmd() para evitar problemas de dibujo transitorios.
     */
    APP_CMD_WINDOW_REDRAW_NEEDED,

    /**
     * Comando del subproceso principal: el área de contenido de la ventana ha cambiado,
     * por ejemplo, de mostrar u ocultar la ventana de entrada por software.  Puede
     * buscar el rectángulo del contenido nuevo en android_app::contentRect.
     */
    APP_CMD_CONTENT_RECT_CHANGED,

    /**
     * Comando del subproceso principal: la ventana de actividad de la aplicación ha ganado
     * el foco de entrada.
     */
    APP_CMD_GAINED_FOCUS,

    /**
     * Comando del subproceso principal: la ventana de actividad de la aplicación ha perdido
     * el foco de entrada.
     */
    APP_CMD_LOST_FOCUS,

    /**
     * Comando del subproceso principal: la configuración actual del dispositivo ha cambiado.
     */
    APP_CMD_CONFIG_CHANGED,

    /**
     * Comando del subproceso principal: el sistema se está quedando sin memoria.
     * Intente reducir el uso de memoria.
     */
    APP_CMD_LOW_MEMORY,

    /**
     * Comando del subproceso principal: la actividad de la aplicación se ha iniciado.
     */
    APP_CMD_START,

    /**
     * Comando del subproceso principal: la actividad de la aplicación se ha reanudado.
     */
    APP_CMD_RESUME,

    /**
     * Comando del subproceso principal: la aplicación debe generar un nuevo estado guardado
     * para sí misma desde el que restaurar más adelante si es necesario. Si ha guardado un estado,
     * asígnelo con la asignación de memoria y colóquelo en android_app.savedState con
     * el tamaño de android_app.savedStateSize.  Más adelante
     * se liberarán.
     */
    APP_CMD_SAVE_STATE,

    /**
     * Comando del subproceso principal: la actividad de la aplicación se ha pausado.
     */
    APP_CMD_PAUSE,

    /**
     * Comando del subproceso principal: la actividad de la aplicación se ha detenido.
     */
    APP_CMD_STOP,

    /**
     * Comando del subproceso principal: la actividad de la aplicación se está destruyendo
     * y se está esperando a que el subproceso de la aplicación se limpie y se cierre antes de continuar.
     */
    APP_CMD_DESTROY,
};

/**
 * Llame cuando ALooper_pollAll() devuelva LOOPER_ID_MAIN leyendo el siguiente
 * mensaje de comando de la aplicación.
 */
int8_t android_app_read_cmd(struct android_app* android_app);

/**
 * Llame con el comando devuelto por android_app_read_cmd() para hacer el
 * procesamiento previo inicial del comando dado. Puede realizar sus propias
 * acciones para el comando después de llamar a esta función.
 */
void android_app_pre_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * Llame con el comando devuelto por android_app_read_cmd() para hacer el
 * procesamiento previo final del comando dado. Debe haber realizado sus propias
 * acciones para el comando antes de llamar a esta función.
 */
void android_app_post_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * Esta es la función que el código de la aplicación debe implementar; representa
 * la entrada principal a la aplicación.
 */
extern void android_main(struct android_app* app);

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID_NATIVE_APP_GLUE_H */
