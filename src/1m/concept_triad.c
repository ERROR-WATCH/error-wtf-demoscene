/* ERROR.wtf // 1MiB — Concept Triad prototypes (Phase 3)
 * Directions: A=SIGNAL GHOST  B=FRACTURED ORDER  C=RED RECURSION
 * Usage: triad <A|B|C>  — runs 15s AV in EGL headless mode, captures 6 PPM frames.
 * Real GL 3.3 core shaders + per-direction procedural score (ALSA hw output).
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

#define SR 48000
#define DUR 15
#define W 1280
#define H 720
#define CAPT_TIMES { 0.5f, 3.0f, 6.0f, 9.0f, 12.0f, 14.5f }
#define CAPT_COUNT 6
static const float CAPT_T[CAPT_COUNT] = CAPT_TIMES;

static atomic_long g_samples = 0;
static char DIRC;

static void *audio_thread(void *arg) {
    (void)arg;
    snd_pcm_t *pcm = NULL;
    const char *devs[] = { "default", "hw:0,0", "plughw:0,0", NULL };
    for (int i = 0; devs[i]; i++)
        if (snd_pcm_open(&pcm, devs[i], SND_PCM_STREAM_PLAYBACK, 0) >= 0) break;
    if (!pcm) { fprintf(stderr, "AUDIO_FAIL\n"); return NULL; }
    snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SR, 1, 100000);
    size_t frames = 1024, total = (size_t)DUR * SR, done = 0;
    short *buf = malloc(frames * 2 * sizeof(short));
    unsigned seed = (unsigned)DIRC * 7919;
    while (done < total) {
        size_t n = frames < total - done ? frames : total - done;
        for (size_t i = 0; i < n; i++) {
            double t = (double)(done + i) / SR;
            double s = 0.0;
            double beat_t = fmod(t, 0.5);
            if (DIRC == 'A') {          /* SIGNAL GHOST: sparse clicks + sub pulse */
                double ct = fmod(t, 0.75);
                if (ct < 0.02) s += 0.32 * sin(2.0 * M_PI * 2200.0 * ct) * (1.0 - ct / 0.02);
                double st = fmod(t, 2.0);
                if (st < 0.5) s += 0.30 * sin(2.0 * M_PI * 55.0 * st) * exp(-st * 4.0);
            } else if (DIRC == 'B') {   /* FRACTURED ORDER: driving groove */
                s += 0.42 * sin(2.0 * M_PI * (130.0 * exp(-beat_t * 15.0) + 46.0) * beat_t) * exp(-beat_t * 9.0);
                if (fmod(t + 0.25, 0.5) < 0.03) s += 0.06;
                static const double bs[4] = { 110.0, 98.0, 87.31, 110.0 };
                int bar = ((int)(t / 2.0)) % 4;
                double bt = fmod(t, 2.0);
                if (bt < 1.6) s += 0.20 * sin(2.0 * M_PI * bs[bar] * 0.5 * t) * exp(-bt * 0.4);
                s += 0.07 * (sin(2.0 * M_PI * 220.0 * t) + 0.6 * sin(2.0 * M_PI * 261.63 * t));
            } else {                    /* RED RECURSION: drone + blips */
                s += 0.20 * sin(2.0 * M_PI * 55.0 * t) * (0.8 + 0.2 * sin(t * 0.5));
                s += 0.12 * sin(2.0 * M_PI * 82.41 * t) * (0.8 + 0.2 * sin(t * 0.35 + 1.0));
                double bt = fmod(t, 0.25);
                if (bt < 0.015) {
                    seed = seed * 1103515245u + 12345u;
                    double f = 800.0 + (seed >> 16) % 900;
                    s += 0.14 * sin(2.0 * M_PI * f * bt) * (1.0 - bt / 0.015);
                }
            }
            if (s > 0.92) s = 0.92;
            if (s < -0.92) s = -0.92;
            buf[2 * i] = (short)(s * 32767.0);
            buf[2 * i + 1] = (short)(s * 0.85 * 32767.0);
        }
        snd_pcm_sframes_t w = snd_pcm_writei(pcm, buf, (snd_pcm_uframes_t)n);
        if (w < 0) { if (snd_pcm_recover(pcm, (int)w, 1) < 0) break; w = 0; }
        done += (size_t)w;
        atomic_store(&g_samples, (long)done);
    }
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
    return NULL;
}

/* --- GL 3.3 core loader (same proven pattern as smoke_av / 1m_av) --- */
#define GLFN(name, ret, args) static ret (*name##_p) args;
GLFN(glClear, void, (unsigned))
GLFN(glClearColor, void, (float, float, float, float))
GLFN(glGetString, const char *, (unsigned))
GLFN(glViewport, void, (int, int, int, int))
GLFN(glReadPixels, void, (int, int, int, int, unsigned, unsigned, void *))
GLFN(glCreateShader, unsigned, (unsigned))
GLFN(glShaderSource, void, (unsigned, int, const char **, const int *))
GLFN(glCompileShader, void, (unsigned))
GLFN(glGetShaderiv, void, (unsigned, unsigned, int *))
GLFN(glGetShaderInfoLog, void, (unsigned, int, int *, char *))
GLFN(glCreateProgram, unsigned, (void))
GLFN(glAttachShader, void, (unsigned, unsigned))
GLFN(glLinkProgram, void, (unsigned))
GLFN(glGetProgramiv, void, (unsigned, unsigned, int *))
GLFN(glUseProgram, void, (unsigned))
GLFN(glGenVertexArrays, void, (int, unsigned *))
GLFN(glBindVertexArray, void, (unsigned))
GLFN(glGenBuffers, void, (int, unsigned *))
GLFN(glBindBuffer, void, (unsigned, unsigned))
GLFN(glBufferData, void, (unsigned, long, const void *, unsigned))
GLFN(glEnableVertexAttribArray, void, (unsigned))
GLFN(glVertexAttribPointer, void, (unsigned, int, unsigned, char, int, const void *))
GLFN(glDrawArrays, void, (unsigned, int, int))
GLFN(glGetUniformLocation, int, (unsigned, const char *))
GLFN(glUniform1f, void, (int, float))

static void *getfn(const char *n) {
    void *p = (void *)eglGetProcAddress(n);
    if (!p) { fprintf(stderr, "EVIDENCE_FAIL: missing %s\n", n); exit(2); }
    return p;
}
static void load_gl(void) {
    glClear_p = getfn("glClear"); glClearColor_p = getfn("glClearColor");
    glGetString_p = getfn("glGetString"); glViewport_p = getfn("glViewport");
    glReadPixels_p = getfn("glReadPixels"); glCreateShader_p = getfn("glCreateShader");
    glShaderSource_p = getfn("glShaderSource"); glCompileShader_p = getfn("glCompileShader");
    glGetShaderiv_p = getfn("glGetShaderiv"); glGetShaderInfoLog_p = getfn("glGetShaderInfoLog");
    glCreateProgram_p = getfn("glCreateProgram"); glAttachShader_p = getfn("glAttachShader");
    glLinkProgram_p = getfn("glLinkProgram"); glGetProgramiv_p = getfn("glGetProgramiv");
    glUseProgram_p = getfn("glUseProgram"); glGenVertexArrays_p = getfn("glGenVertexArrays");
    glBindVertexArray_p = getfn("glBindVertexArray"); glGenBuffers_p = getfn("glGenBuffers");
    glBindBuffer_p = getfn("glBindBuffer"); glBufferData_p = getfn("glBufferData");
    glEnableVertexAttribArray_p = getfn("glEnableVertexAttribArray");
    glVertexAttribPointer_p = getfn("glVertexAttribPointer");
    glDrawArrays_p = getfn("glDrawArrays");
    glGetUniformLocation_p = getfn("glGetUniformLocation");
    glUniform1f_p = getfn("glUniform1f");
}

static const char *VS =
    "#version 330 core\n"
    "const vec2 v[3]=vec2[3](vec2(-1.,-1.),vec2(3.,-1.),vec2(-1.,3.));\n"
    "void main(){gl_Position=vec4(v[gl_VertexID],0.,1.);}";

static const char *FS_A =
    "#version 330 core\nuniform float u_time;out vec4 o;\n"
    "float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}\n"
    "void main(){\n"
    " vec2 uv=gl_FragCoord.xy/vec2(1280,720);\n"
    " vec3 col=vec3(0.012);\n"
    " float row=floor(gl_FragCoord.y/3.0);\n"
    " float sweep=fract(u_time*0.22);\n"
    " float line=step(0.9975,fract(uv.y*0.5+0.5-sweep))- (1.0-step(0.999,fract(uv.y*0.5+0.5-sweep)));\n"
    " float seg=step(0.975,hash(vec2(row,floor(u_time*2.0))));\n"
    " float core=smoothstep(0.14,0.0,distance(uv,vec2(0.5+0.09*sin(u_time*0.7),0.5)));\n"
    " float ghost=smoothstep(0.5,0.0,distance(uv,vec2(0.5,0.5)))*0.05*(0.5+0.5*sin(u_time*2.0));\n"
    " col+=vec3(0.85,0.05,0.04)*(line*seg*0.9+core*0.95);\n"
    " col+=vec3(0.45,0.02,0.02)*ghost;\n"
    " o=vec4(col,1.0);}";

static const char *FS_B =
    "#version 330 core\nuniform float u_time;out vec4 o;\n"
    "float lm(vec2 uv,float w){vec2 g=abs(fract(uv)-0.5);return smoothstep(w,0.0,min(g.x,g.y));}\n"
    "void main(){\n"
    " vec2 uv=gl_FragCoord.xy/vec2(1280,720);\n"
    " float t=u_time;\n"
    /* (4) longer fracture window, slow decay: still clearly fractured at t=12 */
    " float fr=smoothstep(7.5,8.5,t)*(1.0-smoothstep(12.0,14.0,t));\n"
    /* (1) ALL structure layers fracture: per-layer displacement strength */
    " float disp=fr*2.2;\n"
    " vec2 warp=vec2(sin(uv.y*9.0+t*2.5),cos(uv.x*7.0-t*2.0))*disp*0.08;\n"
    /* (2) hierarchy: spines (heavy, slow drift) over varied bays over minor grid */
    " vec2 spine_uv=vec2(uv.x*2.0+t*0.05,uv.y*2.0-t*0.03)+warp;\n"
    " float spine=lm(spine_uv,0.012)*0.9;\n"
    " vec2 bay1=(uv+0.03*sin(t*0.3))*8.0+warp*1.5;\n"
    " vec2 bay2=(uv-0.02*cos(t*0.2))*5.0+warp*1.2;\n"
    " float bays=max(lm(bay1,0.05)*(0.55+0.15*sin(t*0.9+uv.x*10.0)),lm(bay2,0.06)*0.4);\n"
    " float grid=lm(uv*28.0+warp*2.5,0.03)*(0.22+0.10*sin(t*1.2+uv.y*14.0));\n"
    /* (3) strong ordered-state pulse: expanding ring + beat flash */
    " float pr=fract(t*1.0);\n"
    " float ring=smoothstep(0.02,0.0,abs(distance(uv,vec2(0.5))-pr*0.55))*(1.0-pr);\n"
    " float pulse=ring*0.5+smoothstep(0.5,0.0,abs(fract(t*2.0)-0.5))*0.10;\n"
    /* fracture tint: strong energy, but structure-carried (not a flat panel) */
    " vec3 col=vec3(0.010,0.015,0.02);\n"
    " col+=vec3(0.50,0.62,0.72)*bays;\n"
    " col+=vec3(0.90,0.94,0.98)*spine;\n"
    " col+=vec3(0.35,0.45,0.55)*grid;\n"
    " col+=vec3(0.10,0.30,0.45)*pulse;\n"
    " col+=vec3(0.38,0.04,0.02)*fr*0.45;\n"
    " col+=vec3(0.75,0.12,0.05)*fr*(0.30+bays*0.85+grid*0.75+spine*0.15);\n"
    " o=vec4(col,1.0);}";

static const char *FS_C =
    "#version 330 core\nuniform float u_time;out vec4 o;\n"
    "float rrec(vec2 uv,float t){\n"
    " float acc=0.0;\n"
    " for(int i=0;i<5;i++){\n"
    "  float fi=float(i);\n"
    "  vec2 p=(uv-0.5)*(2.0+fi*0.9)*(1.0+0.05*sin(t*0.6+fi));\n"
    "  float a=t*0.15+fi*0.4;\n"
    "  p=mat2(cos(a),-sin(a),sin(a),cos(a))*p;\n"
    "  vec2 q=abs(fract(p)-0.5);\n"
    "  acc+=smoothstep(0.46,0.42,max(q.x,q.y))*smoothstep(0.30,0.34,max(q.x,q.y))*(0.85-fi*0.14);\n"
    " }\n return acc;}\n"
    "void main(){\n"
    " vec2 uv=gl_FragCoord.xy/vec2(1280,720);\n"
    " float t=u_time;\n"
    " float v=rrec(uv,t);\n"
    " float breathe=0.75+0.25*sin(t*0.9);\n"
    " vec3 col=vec3(0.012,0.005,0.005);\n"
    " col+=vec3(0.82,0.05,0.03)*v*breathe;\n"
    " col+=vec3(0.35,0.02,0.02)*smoothstep(0.3,1.0,length(uv-0.5))*0.4;\n"
    " o=vec4(col,1.0);}";

int main(int argc, char **argv) {
    if (argc < 2 || !strchr("ABC", (DIRC = argv[1][0])) || !argv[1][0]) {
        fprintf(stderr, "usage: triad <A|B|C>\n"); return 1;
    }
    const char *fs = DIRC == 'A' ? FS_A : DIRC == 'B' ? FS_B : FS_C;

    glfwInit();
    GLFWwindow *win = NULL;
    EGLDisplay dpy = EGL_NO_DISPLAY; EGLSurface surf = EGL_NO_SURFACE; EGLContext ctx = EGL_NO_CONTEXT;
    if (getenv("ERRORWTF_HEADLESS")) {
        eglBindAPI(EGL_OPENGL_API);
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, NULL, NULL)) { fprintf(stderr, "EVIDENCE_FAIL: egl init\n"); return 2; }
        EGLint ca[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
                        EGL_BLUE_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE };
        EGLConfig cfg; EGLint nc = 0;
        if (!eglChooseConfig(dpy, ca, &cfg, 1, &nc) || nc < 1) { fprintf(stderr, "EVIDENCE_FAIL: cfg\n"); return 2; }
        EGLint pa[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
        surf = eglCreatePbufferSurface(dpy, cfg, pa);
        if (surf == EGL_NO_SURFACE) { fprintf(stderr, "EVIDENCE_FAIL: pbuffer\n"); return 2; }
        EGLint cattrs[] = { EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 3, EGL_NONE };
        ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, cattrs);
        if (ctx == EGL_NO_CONTEXT) { fprintf(stderr, "EVIDENCE_FAIL: ctx\n"); return 2; }
        eglMakeCurrent(dpy, surf, surf, ctx);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        win = glfwCreateWindow(W, H, "triad", NULL, NULL);
        if (!win) { fprintf(stderr, "EVIDENCE_FAIL: window\n"); return 2; }
        glfwMakeContextCurrent(win);
    }
    load_gl();
    printf("EVIDENCE: renderer=%s dir=%c\n", glGetString_p(GL_RENDERER), DIRC);

    unsigned vs = glCreateShader_p(GL_VERTEX_SHADER);
    glShaderSource_p(vs, 1, &VS, NULL);
    glCompileShader_p(vs);
    int ok = 0;
    glGetShaderiv_p(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { char log[512]; glGetShaderInfoLog_p(vs, 512, NULL, log); fprintf(stderr, "VS: %s\n", log); return 2; }
    unsigned fsid = glCreateShader_p(GL_FRAGMENT_SHADER);
    glShaderSource_p(fsid, 1, &fs, NULL);
    glCompileShader_p(fsid);
    glGetShaderiv_p(fsid, GL_COMPILE_STATUS, &ok);
    if (!ok) { char log[512]; glGetShaderInfoLog_p(fsid, 512, NULL, log); fprintf(stderr, "FS%c: %s\n", DIRC, log); return 2; }
    unsigned prog = glCreateProgram_p();
    glAttachShader_p(prog, vs);
    glAttachShader_p(prog, fsid);
    glLinkProgram_p(prog);
    glGetProgramiv_p(prog, GL_LINK_STATUS, &ok);
    if (!ok) { fprintf(stderr, "EVIDENCE_FAIL: link\n"); return 2; }
    glUseProgram_p(prog);

    unsigned vao, vbo;
    glGenVertexArrays_p(1, &vao);
    glBindVertexArray_p(vao);
    glGenBuffers_p(1, &vbo);
    glBindBuffer_p(GL_ARRAY_BUFFER, vbo);
    float tri[9] = { -1, -1, 0, 3, -1, 0, -1, 3, 0 };
    glBufferData_p(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    glEnableVertexAttribArray_p(0);
    glVertexAttribPointer_p(0, 3, GL_FLOAT, 0, 0, NULL);
    glViewport_p(0, 0, W, H);

    pthread_t at;
    pthread_create(&at, NULL, audio_thread, NULL);

    unsigned char *px = malloc((size_t)W * H * 3);
    int captured = 0, cap_i = 0;
    double last = -1;
    while (1) {
        double t = (double)atomic_load(&g_samples) / SR;
        if (t >= (double)DUR) break;
        if ((int)t != last) { last = (int)t; }
        glClearColor_p(0, 0, 0, 1);
        glClear_p(GL_COLOR_BUFFER_BIT);
        glUseProgram_p(prog);
        glUniform1f_p(glGetUniformLocation_p(prog, "u_time"), (float)t);
        glDrawArrays_p(GL_TRIANGLES, 0, 3);
        if (win) { glfwPollEvents(); glfwSwapBuffers(win); }
        else {
            if (cap_i < CAPT_COUNT && t >= CAPT_T[cap_i] && captured <= cap_i) {
                glReadPixels_p(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, px);
                char fn[128];
                snprintf(fn, sizeof fn, "captures/1m/triad_%c_t%.1f.ppm", DIRC, CAPT_T[cap_i]);
                FILE *f = fopen(fn, "wb");
                if (f) {
                    fprintf(f, "P6\n%d %d\n255\n", W, H);
                    for (int y = H - 1; y >= 0; y--) fwrite(px + (size_t)y * W * 3, (size_t)W * 3, 1, f);
                    fclose(f);
                    printf("EVIDENCE: captured %s\n", fn);
                }
                cap_i++;
            }
            struct timespec ts = { 0, 120 * 1000 * 1000 };
            nanosleep(&ts, NULL);
        }
        if (cap_i >= CAPT_COUNT && t > CAPT_T[CAPT_COUNT - 1] + 0.5) break;
    }
    pthread_join(at, NULL);
    printf("EVIDENCE: triad %c complete\n", DIRC);
    return 0;
}
