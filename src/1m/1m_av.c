/* ERROR.wtf — BEYOND THE ERROR: ONE MEGABYTE
 * Blockout v0.1 — 120 s AV-Timeline, sample-based master clock.
 * Modes: GLFW window (desktop session) / EGL headless fallback (evidence frames).
 * Development build: NOT a release. Padding to exact 1048576 bytes at build time.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <EGL/egl.h>
#include <alsa/asoundlib.h>

#ifndef ERRORWTF_SECONDS
#define ERRORWTF_SECONDS 120
#endif
#define SR 48000
#define TARGET_W 1280
#define TARGET_H 720

static atomic_long g_samples_played = 0;
static volatile int g_audio_done = 0;

/* Scene boundaries in seconds (Regieskizze §5.2, Blockout v0.1) */
static const double SCENES[] = { 0.0, 12.0, 28.0, 45.0, 64.0, 78.0, 100.0, 112.0, 120.0 };
#define SCENE_COUNT 9

static double scene_time_seconds(void) {
    long s = atomic_load(&g_samples_played);
    return (double)s / SR;
}

static void *audio_thread(void *arg) {
    (void)arg;
    const char *devices[] = { "default", "hw:0,0", "plughw:0,0", NULL };
    snd_pcm_t *pcm = NULL;
    int chosen = -1;
    for (int i = 0; devices[i]; i++) {
        if (snd_pcm_open(&pcm, devices[i], SND_PCM_STREAM_PLAYBACK, 0) >= 0) { chosen = i; break; }
    }
    if (chosen < 0) { fprintf(stderr, "AUDIO_FAIL: no device (visual fallback clock)\n"); g_audio_done = 1; return NULL; }
    if (snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                           2, SR, 1, 120000) < 0) {
        fprintf(stderr, "AUDIO_FAIL: set_params\n"); g_audio_done = 1; return NULL; }
    size_t frames = 2048;
    short *buf = malloc(frames * 2 * sizeof(short));
    size_t total = (size_t)ERRORWTF_SECONDS * SR;
    double scale[4] = { 110.0, 130.81, 164.81, 220.0 };
    size_t written_total = 0;
    while (written_total < total) {
        size_t chunk = frames;
        if (written_total + chunk > total) chunk = total - written_total;
        for (size_t i = 0; i < chunk; i++) {
            double t = (double)(written_total + i) / SR;
            int scene = 0;
            while (scene + 1 < SCENE_COUNT && t >= SCENES[scene + 1]) scene++;
            double beat = fmod(t, 0.5);
            double kick = 0.0, hat = 0.0, bass = 0.0, pad = 0.0, lead = 0.0;
            if (scene >= 1) { /* FIRST ORDER+: percussion enters */
                kick = sin(2.0 * M_PI * (110.0 * exp(-beat * 16.0) + 42.0) * beat) * exp(-beat * 10.0);
                hat = ((int)(t * 8.0) % 2) ? exp(-fmod(t, 0.125) * 60.0) * 0.08 : 0.0;
            }
            if (scene >= 2) { /* SIGNAL ARCHITECTURE+: bassline */
                int bn = ((int)(t * 2.0)) % 4;
                double bf = scale[bn] * 0.5;
                bass = sin(2.0 * M_PI * bf * t) * (0.28 + 0.06 * sin(t * 0.7));
            }
            if (scene >= 3) { /* FALSE PERFECTION+: pad */
                pad = 0.10 * (sin(2.0 * M_PI * 220.0 * t) + sin(2.0 * M_PI * 277.18 * t) * 0.6);
            }
            if (scene == 4) { /* SYSTEM FAULT: break — strip groove, keep texture */
                kick *= 0.15; bass *= 0.35;
            }
            if (scene >= 5) { /* REWRITE REALITY: new pulse + lead motif */
                kick = sin(2.0 * M_PI * (120.0 * exp(-beat * 13.0) + 48.0) * beat) * exp(-beat * 8.0);
                double motif[4] = { 440.0, 523.25, 659.25, 880.0 };
                double mt = fmod(t, 2.0);
                int mi = (int)(mt / 0.5) % 4;
                lead = sin(2.0 * M_PI * motif[mi] * t) * exp(-fmod(t, 0.5) * 3.0) * 0.16;
            }
            if (scene == 6) { /* THE SIGNATURE: harmonic resolution, calm */
                pad = 0.14 * (sin(2.0 * M_PI * 164.81 * t) + sin(2.0 * M_PI * 220.0 * t) * 0.8);
                kick *= 0.5;
            }
            if (scene == 7) { /* AFTERIMAGE: fade */
                double fade = 1.0 - (t - 112.0) / 8.0;
                kick *= fade; bass *= fade; pad *= fade; lead *= fade; hat *= fade;
            }
            double s = kick * 0.5 + hat + bass + pad + lead;
            if (s > 0.92) { s = 0.92; }
            if (s < -0.92) { s = -0.92; }
            buf[2 * i] = (short)(s * 32767.0);
            buf[2 * i + 1] = (short)(s * 0.85 * 32767.0);
        }
        snd_pcm_sframes_t w = snd_pcm_writei(pcm, buf, (snd_pcm_uframes_t)chunk);
        if (w < 0) { if (snd_pcm_recover(pcm, (int)w, 1) < 0) break; w = 0; }
        written_total += (size_t)w;
        atomic_store(&g_samples_played, (long)written_total);
    }
    atomic_store(&g_samples_played, (long)total);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
    g_audio_done = 1;
    printf("EVIDENCE: audio done, device=%s samples=%ld\n", devices[chosen], (long)total);
    return NULL;
}

static void errorfn(int code, const char *desc) {
    fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

static void key_cb(GLFWwindow *w, int key, int sc, int action, int mods) {
    (void)sc; (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(w, 1);
}

static void (*glfw_glClear)(unsigned int mask);
static void (*glfw_glClearColor)(float r, float g, float b, float a);
static const char *(*glfw_glGetString)(unsigned int name);
static int g_have_window = 0;

static void *getfn(const char *name) {
    void *p = (g_have_window ? (void *)glfwGetProcAddress(name) : (void *)eglGetProcAddress(name));
    if (!p) { fprintf(stderr, "EVIDENCE_FAIL: missing GL function %s\n", name); exit(2); }
    return p;
}

int main(void) {
    glfwSetErrorCallback(errorfn);
    int headless = getenv("ERRORWTF_HEADLESS") != NULL;
    GLFWwindow *win = NULL;
    EGLDisplay dpy = EGL_NO_DISPLAY; EGLSurface surf = EGL_NO_SURFACE; EGLContext ctx = EGL_NO_CONTEXT;
    int W = TARGET_W, H = TARGET_H;
    int have_window = 0;
    if (!headless && glfwInit()) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        win = glfwCreateWindow(W, H, "ERROR.wtf // BEYOND THE ERROR // 1MiB", NULL, NULL);
        if (win) { have_window = 1; glfwMakeContextCurrent(win); glfwSetKeyCallback(win, key_cb); glfwSwapInterval(1); }
    }
    if (!have_window) {
        /* headless evidence path */
        setenv("ERRORWTF_HEADLESS", "1", 0);
        eglBindAPI(EGL_OPENGL_API);
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, NULL, NULL)) {
            fprintf(stderr, "EVIDENCE_FAIL: no window and no EGL display\n"); return 2; }
        EGLint ca[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE };
        EGLConfig cfg; EGLint nc = 0;
        if (!eglChooseConfig(dpy, ca, &cfg, 1, &nc) || nc < 1) { fprintf(stderr, "EVIDENCE_FAIL: config\n"); return 2; }
        EGLint pa[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
        surf = eglCreatePbufferSurface(dpy, cfg, pa);
        if (surf == EGL_NO_SURFACE) { fprintf(stderr, "EVIDENCE_FAIL: pbuffer\n"); return 2; }
        eglMakeCurrent(dpy, surf, surf, NULL);
        EGLint cattrs[] = { EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 3, EGL_NONE };
        ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, cattrs);
        if (ctx == EGL_NO_CONTEXT) { fprintf(stderr, "EVIDENCE_FAIL: ctx\n"); return 2; }
        eglMakeCurrent(dpy, surf, surf, ctx);
    }
    g_have_window = have_window;
    glfw_glClear = getfn("glClear");
    glfw_glClearColor = getfn("glClearColor");
    glfw_glGetString = getfn("glGetString");

    const char *renderer = (const char *)glfw_glGetString(GL_RENDERER);
    printf("EVIDENCE: renderer=%s headless=%d duration=%d\n",
           renderer ? renderer : "?", !have_window, ERRORWTF_SECONDS);

    pthread_t at;
    pthread_create(&at, NULL, audio_thread, NULL);

    int last_scene = -1;
    while (!(have_window && glfwWindowShouldClose(win))) {
        double t = scene_time_seconds();
        if (t >= (double)ERRORWTF_SECONDS) break;
        int scene = 0;
        while (scene + 1 < SCENE_COUNT && t >= SCENES[scene + 1]) scene++;
        if (scene != last_scene) {
            printf("EVIDENCE: scene=%d t=%.2f\n", scene, t);
            last_scene = scene;
        }
        /* Blockout v0.1: scene-index visual states (wireframe of the direction bible) */
        float phase = (float)(fmod(t, 8.0) / 8.0);
        float r = 0.02f, g = 0.02f, b = 0.031f;
        switch (scene) {
            case 0: r += 0.30f * phase; break;                          /* COLD BOOT: red rises */
            case 1: g += 0.35f * phase; b += 0.45f * phase; break;      /* FIRST ORDER: cyan enters */
            case 2: b += 0.25f; r += 0.10f * phase; break;              /* SIGNAL ARCHITECTURE */
            case 3: r += 0.05f; g += 0.10f * (1.0f - phase); break;     /* FALSE PERFECTION: calm */
            case 4: r += 0.55f * phase; break;                          /* SYSTEM FAULT: red break */
            case 5: b += 0.30f; g += 0.20f * phase; break;              /* REWRITE REALITY */
            case 6: r += 0.40f; g += 0.40f * (1.0f - phase); b += 0.10f; break; /* THE SIGNATURE */
            case 7: r += 0.10f * (1.0f - phase); break;                 /* AFTERIMAGE: fade to black */
        }
        glfw_glClearColor(r, g, b, 1.0f);
        glfw_glClear(GL_COLOR_BUFFER_BIT);
        if (have_window) { glfwPollEvents(); glfwSwapBuffers(win); }
        else {
            struct timespec ts = { 0, 100 * 1000 * 1000 };
            nanosleep(&ts, NULL);
        }
    }
    while (!g_audio_done) {
        struct timespec ts = { 0, 50 * 1000 * 1000 };
        nanosleep(&ts, NULL);
    }
    pthread_join(at, NULL);
    if (have_window) glfwDestroyWindow(win);
    glfwTerminate();
    printf("EVIDENCE: run complete, duration=%ds exit=clean\n", ERRORWTF_SECONDS);
    return 0;
}
