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

#include <malloc.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

/**
* Nuestros datos guardados.
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* El estado compartido de nuestra aplicación.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};

/**
* Inicialice un contexto de EGL para la visualización actual.
*/
static int engine_init_display(struct engine* engine) {
	// inicialice OpenGL ES y EGL

	/*
	* Indique aquí los atributos de la configuración deseada.
	* Abajo seleccione una EGLConfig con al menos 8 bits por componente
	* de color compatible con una ventana en pantalla
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Aquí, la aplicación elige la configuración que quiere. En este
	* ejemplo, tenemos un proceso de selección muy simplificado en el que elegimos
	* la primera EGLConfig que coincide con nuestros criterios */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID es un atributo de la EGLConfig que tenemos
	* garantizado que ANativeWindow_setBuffersGeometry() aceptará.
	* Cuando elijamos EGLConfig, podremos volver a configurar los
	* búferes ANativeWindow que buscamos usando EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	// Inicialice el estado de GL.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* Solo el fotograma actual de la visualización.
*/
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// No hay visualización.
		return;
	}

	// Rellene la pantalla con un color.
	glClearColor(((float)engine->state.x) / engine->width, engine->state.angle,
		((float)engine->state.y) / engine->height, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(engine->display, engine->surface);
}

/**
* Anule el contexto de EGL asociado actualmente con la visualización.
*/
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* Procese el evento de entrada siguiente.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
* Procese el comando principal siguiente.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// El sistema nos ha pedido que guardemos nuestro estado actual. Hágalo.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// La ventana se va a mostrar, prepárela.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// La ventana se va a ocultar o cerrar, límpiela.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// Cuando nuestra aplicación gana foco, empezamos a supervisar el acelerómetro.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			// Nos gustaría obtener 60 eventos por segundo (en microsegundos).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// Cuando nuestra aplicación pierde foco, dejamos de supervisar el acelerómetro.
		// Esto evita consumir batería mientras no se está usando.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// Detenga también la animación.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	}
}

/**
* Este es el punto de entrada principal de una aplicación nativa que usa
* android_native_app_glue.  Se ejecuta en su propio subproceso con su propio
* bucle de eventos para recibir eventos de entrada y hacer otras cosas.
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	// Prepararse para supervisar el acelerómetro
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// Estamos empezando con un estado guardado anterior; restaure a partir de él.
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;

	// bucle en espera de hacer cosas.

	while (1) {
		// Lea todos los eventos pendientes.
		int ident;
		int events;
		struct android_poll_source* source;

		// Si no hay animación, bloquearemos para siempre la espera de eventos.
		// Si hay animación, repetiremos el bucle hasta que se lean todos los eventos y luego
		// dibujaremos el siguiente fotograma de animación.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// Procese este evento.
			if (source != NULL) {
				source->process(state, source);
			}

			// Si un sensor tiene datos, procéselo ahora.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						LOGI("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			// Compruebe si vamos a cerrar.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			// Operaciones de eventos acabadas; dibuje el siguiente fotograma de animación.
			engine.state.angle += .01f;
			if (engine.state.angle > 1) {
				engine.state.angle = 0;
			}

			// La operación de dibujo está limitada según la tasa de actualización de la pantalla, por lo que
			// no es necesario hacer intervalos aquí.
			engine_draw_frame(&engine);
		}
	}
}
