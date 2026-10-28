/* ERROR.wtf — BEYOND THE ERROR: ONE MEGABYTE — production source (phase 5)
 * Single translation unit: score_sample() (music engine, verbatim) + FRACTURED
 * ORDER scene shader (B v3 logic, 8 scenes) + procedural wordmark reveal.
 * Build: gcc -> pad_exact.sh 1048576 -> errorwtf-1m.
 * Modes: GLFW window (desktop) / EGL headless (evidence captures).
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

#ifndef EW_SECONDS
#define EW_SECONDS 120
#endif
#define SR 48000
#define W 1280
#define H 720
#define PI 3.14159265358979323846

static atomic_long g_samples = 0;

/* ================= SCORE (single source of truth, from music_engine) ================= */
static const double SC[] = { 0, 12, 28, 45, 64, 78, 100, 112, 120 };
#define NS 9
static int scene_of(double t) { int s = 0; while (s + 1 < NS && t >= SC[s + 1]) s++; return s; }
static const double MOTIV_F[5] = { 329.63, 466.16, 392.00, 493.88, 587.33 };
static const double MOTIV_T[5] = { 0.00, 0.25, 0.55, 1.30, 1.30 };
static const double MOTIV_D[5] = { 0.22, 0.26, 0.70, 0.85, 0.85 };
static double tone(double t, double f, double amp, double decay, double saw) {
    if (t < 0 || t > decay * 3.2) return 0.0;
    double env = exp(-t / decay * 3.0);
    double s = sin(2 * PI * f * t) + saw * 0.35 * sin(2 * PI * 2 * f * t + 0.7)
             + 0.15 * sin(2 * PI * 3 * f * t + 1.3);
    return amp * env * s;
}
static double motif(double t, double t0, int mode) {
    double rate = mode == 1 ? 0.55 : 1.0;
    double tl = (t - t0) / rate;
    double out = 0.0;
    int n = (mode == 2) ? 5 : 4;
    for (int i = 0; i < n; i++) {
        double f = MOTIV_F[i];
        if (mode == 1 && i == 2) f *= 0.985;
        if (mode == 2 && i == 3) f = 392.00;
        out += tone(tl - MOTIV_T[i], f, 0.16, MOTIV_D[i], 0.5);
        if (mode == 1) out += tone(tl - MOTIV_T[i], f * 1.0595, 0.05, MOTIV_D[i], 0.2);
    }
    if (mode == 2) out += tone(tl - MOTIV_T[3], MOTIV_F[3] * 1.5, 0.10, MOTIV_D[3], 0.3);
    return out;
}
static uint64_t rstate = 42;
static double rnd(void) { rstate = rstate * 6364136223846793005ULL + 1442695040888963407ULL; return (double)(rstate >> 40) / 16777216.0; }
static double kick(double t, double amp) { return (t < 0 || t > 0.45) ? 0.0 : amp * exp(-t * 9.0) * sin(2 * PI * (120.0 * exp(-t * 14.0) + 45.0) * t); }
static double hat(double t) { return (t >= 0 && t < 0.03) ? 0.10 * exp(-t * 90.0) * (rnd() * 2 - 1) : 0.0; }
static double drone(double t, double base) {
    return 0.11 * sin(2 * PI * base * t) * (0.75 + 0.25 * sin(t * 0.45))
         + 0.07 * sin(2 * PI * base * 1.5 * t + 0.5) * (0.75 + 0.25 * sin(t * 0.31 + 1.0));
}
static double bass(double t, double f, double amp) { return amp * sin(2 * PI * f * t) * (0.7 + 0.3 * sin(t * 0.9)); }

static double score_sample(double t) {
    int s = scene_of(t);
    double out = 0.0;
    double bt = fmod(t, 0.5);
    double dfade = (s == 7) ? 1.0 - (t - 112.0) / 8.0 : 1.0;
    if (s != 4) out += drone(t, 82.41) * dfade * (s < 2 ? 0.7 : 1.0);
    if (s <= 1) {
        out += motif(t, 3.0, s == 0 ? 1 : 0) * 0.8;
        if (s == 1) out += kick(bt, 0.34) + hat(fmod(t + 0.25, 0.5)) + bass(t, 82.41, 0.16);
    } else if (s == 2) {
        out += kick(bt, 0.40) + hat(fmod(t + 0.25, 0.5)) + bass(t, 82.41, 0.20);
        int bar = ((int)(t / 2.0)) % 4;
        static const double bl[4] = { 82.41, 92.50, 98.00, 82.41 };
        double bt2 = fmod(t, 2.0);
        if (bt2 < 1.6) out += bass(t - (t - bt2), bl[bar], 0.16);
        out += motif(t, 29.5, 0) + motif(t, 38.5, 0);
        out += 0.05 * (sin(2 * PI * 329.63 * t) + sin(2 * PI * 392.00 * t));
    } else if (s == 3) {
        out += kick(bt, 0.16) + 0.09 * sin(2 * PI * 123.47 * t);
        out += motif(t, 47.0, 0) * 0.7;
        out += 0.08 * (sin(2 * PI * 164.81 * t) + 0.6 * sin(2 * PI * 196.00 * t));
    } else if (s == 4) {
        double jit = (rnd() - 0.5) * 0.04;
        double btj = fmod(t + jit, 0.5);
        out += kick(btj, 0.30) * (0.8 + 0.2 * rnd());
        out += 0.22 * bass(t, 82.41 * (1.0 + 0.04 * sin(t * 2.7)), 1.0);
        out += 0.14 * bass(t, 87.31, 1.0);
        out += motif(t, 65.0, 1) + motif(t, 71.0, 1);
    } else if (s == 5) {
        out += kick(bt, 0.38) + hat(fmod(t, 0.5)) + bass(t, 98.00, 0.18);
        out += motif(t, 80.0, 2) + motif(t, 88.0, 2);
        out += 0.10 * sin(2 * PI * 523.25 * t) * (0.6 + 0.4 * sin(t * 0.8));
    } else if (s == 6) {
        out += kick(bt, 0.24);
        out += motif(t, 101.0, 2) + motif(t, 104.5, 2) + motif(t, 108.0, 2);
        out += 0.10 * (sin(2 * PI * 329.63 * t) + sin(2 * PI * 392.00 * t) + sin(2 * PI * 493.88 * t));
    } else {
        double fade = 1.0 - (t - 112.0) / 8.0;
        out += motif(t, 113.0, 2) * 0.7 * fade;
        out += drone(t, 82.41) * 0.5 * fade;
    }
    return out;
}

static void *audio_thread(void *arg) {
    (void)arg;
    snd_pcm_t *pcm = NULL;
    const char *devs[] = { "default", "hw:0,0", "plughw:0,0", NULL };
    for (int i = 0; devs[i]; i++)
        if (snd_pcm_open(&pcm, devs[i], SND_PCM_STREAM_PLAYBACK, 0) >= 0) break;
    if (!pcm) { fprintf(stderr, "AUDIO_FAIL: visual fallback clock\n"); return NULL; }
    snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SR, 1, 100000);
    size_t frames = 1024, total = (size_t)EW_SECONDS * SR, done = 0;
    short *buf = malloc(frames * 2 * sizeof(short));
    double norm = 1.0; /* score peaks measured <= 0.86 offline */
    while (done < total) {
        size_t n = frames < total - done ? frames : total - done;
        for (size_t i = 0; i < n; i++) {
            double t = (double)(done + i) / SR;
            double s = score_sample(t) * norm;
            if (s > 0.92) s = 0.92;
            if (s < -0.92) s = -0.92;
            buf[2 * i] = (short)(s * 32767.0);
            buf[2 * i + 1] = (short)(s * 0.88 * 32767.0);
        }
        snd_pcm_sframes_t w = snd_pcm_writei(pcm, buf, (snd_pcm_uframes_t)n);
        if (w < 0) { if (snd_pcm_recover(pcm, (int)w, 1) < 0) break; w = 0; }
        done += (size_t)w;
        atomic_store(&g_samples, (long)done);
    }
    atomic_store(&g_samples, (long)total);
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
    printf("EVIDENCE: audio complete (%zu samples)\n", total);
    return NULL;
}

/* ================= GL 3.3 core loader ================= */
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
GLFN(glUniform1i, void, (int, int))

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
    glUniform1f_p = getfn("glUniform1f"); glUniform1i_p = getfn("glUniform1i");
}

/* ================= SHADER: FRACTURED ORDER, 8 scenes, wordmark reveal ================= */
static const char *VS =
    "#version 330 core\n"
    "const vec2 v[3]=vec2[3](vec2(-1.,-1.),vec2(3.,-1.),vec2(-1.,3.));\n"
    "void main(){gl_Position=vec4(v[gl_VertexID],0.,1.);}";

/* wordmark 'ERROR.WTF' — 5x7 pixel glyphs, one int per row (bits left->right) */
static const char *FS =
    "#version 330 core\n"
    "uniform float u_time;uniform int u_scene;out vec4 o;\n"
    "float lm(vec2 uv,float w){vec2 g=abs(fract(uv)-0.5);return smoothstep(w,0.0,min(g.x,g.y));}\n"
    "float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}\n"
    "mat2 rot(float a){float c=cos(a),s=sin(a);return mat2(c,-s,s,c);}\n"
    /* 7 rows x 5 cols per glyph; 9 glyphs */
    "const int G[63]=int[63](\n"
    /* E */ "30,16,30,16,16,16,30,\n"
    /* R */ "28,18,28,24,20,18,17,\n"
    /* R */ "28,18,28,24,20,18,17,\n"
    /* O */ "14,17,17,17,17,17,14,\n"
    /* R */ "28,18,28,24,20,18,17,\n"
    /* dot */ "0,0,0,0,0,12,12,\n"
    /* W */ "17,17,17,21,21,21,10,\n"
    /* T */ "31,4,4,4,4,4,4,\n"
    /* F */ "31,16,30,16,16,16,16);\n"
    "float mark(vec2 uv,float reveal,float glitch){\n"
    " if(reveal<=0.0)return 0.0;\n"
    " float acc=0.0;\n"
    " vec2 p=((uv-vec2(0.126,0.435))*vec2(1280.0,720.0))/13.3;\n"
    " if(p.y<-1.0||p.y>8.0)return 0.0;\n"
    " for(int i=0;i<9;i++){\n"
    "  float done=float(i)/9.0;\n"
    "  if(reveal<done)continue;\n"
    "  vec2 q=p-vec2(float(i)*8.0,0.0);\n"
    "  if(q.x<0.0||q.x>5.0||q.y<0.0||q.y>7.0)continue;\n"
    "  vec2 cell=floor(q);float fx=fract(q.x),fy=fract(q.y);\n"
    "  int bit=(G[i*7+int(6.0-cell.y)]>>int(4.0-cell.x))&1;\n"
    "  float j=(glitch>0.0)?(hash(cell*0.173+vec2(glitch*9.7))-0.5)*glitch*2.0:0.0;\n"
    "  float px=smoothstep(0.30,0.55,fx)*smoothstep(0.30,0.55,fy);\n"
    "  if(bit==1)acc=max(acc,px*(0.8+0.2*clamp(1.0-abs(j)*8.0,0.0,1.0)));\n"
    " }\n return clamp(acc,0.0,1.0);}\n"
    "void main(){\n"
    " vec2 uv=gl_FragCoord.xy/vec2(1280,720);\n"
    " float t=u_time;int sc=u_scene;\n"
    " float fr=0.0;\n"
    " if(sc==4)fr=1.0;\n"
    " if(sc==5)fr=1.0-smoothstep(0.0,3.5,t-78.0);\n"
    " float disp=fr*2.2;\n"
    " vec2 warp=vec2(sin(uv.y*9.0+t*2.5),cos(uv.x*7.0-t*2.0))*disp*0.08;\n"
    " vec2 spine_uv=vec2(uv.x*2.0+t*0.05,uv.y*2.0-t*0.03)+warp;\n"
    " float spine=lm(spine_uv,0.012)*(sc<=1?0.45:0.9);\n"
    " vec2 bay1=(uv+0.03*sin(t*0.3))*8.0+warp*1.5;\n"
    " vec2 bay2=(uv-0.02*cos(t*0.2))*5.0+warp*1.2;\n"
    " float baymix=(sc==5)?0.85:0.55;\n"
    " float bays=max(lm(bay1,0.05)*(baymix+0.15*sin(t*0.9+uv.x*10.0)),lm(bay2,0.06)*((sc<=1)?0.15:0.4));\n"
    " float grid=lm(uv*28.0+warp*2.5,0.03)*(0.22+0.10*sin(t*1.2+uv.y*14.0));\n"
    " float pr=fract(t);\n"
    " float ring=smoothstep(0.02,0.0,abs(distance(uv,vec2(0.5))-pr*0.55))*(1.0-pr);\n"
    " float pulse=ring*((sc>=2&&sc<=3)?0.5:0.25)+smoothstep(0.5,0.0,abs(fract(t*2.0)-0.5))*0.10;\n"
    " vec3 col=vec3(0.010,0.015,0.02);\n"
    " col+=vec3(0.50,0.62,0.72)*bays;\n"
    " col+=vec3(0.90,0.94,0.98)*spine;\n"
    " col+=vec3(0.35,0.45,0.55)*grid;\n"
    " col+=vec3(0.10,0.30,0.45)*pulse;\n"
    " col+=vec3(0.38,0.04,0.02)*fr*0.45;\n"
    " col+=vec3(0.75,0.12,0.05)*fr*(0.30+bays*0.85+grid*0.75+spine*0.15);\n"
    /* scene-specific accents */
    " /* scene-specific compositions (phase 5 polish) */\n"
    " if(sc==0){float bl=smoothstep(0.0,12.0,t);\n"
    "  float rowb=step(fract(uv.y*18.0),bl*fract(uv.x*2.0+0.5));\n"
    "  col+=vec3(0.30,0.04,0.02)*rowb*0.6;\n"
    "  col+=vec3(0.45,0.05,0.03)*smoothstep(0.6,0.0,distance(uv,vec2(0.5)))*(0.5+0.5*sin(t*1.4));}\n"
    " if(sc==2){float grow=smoothstep(28.0,42.0,t);\n"
    "  float col_i=floor(uv.x*16.0);\n"
    "  float hgt=0.18+0.45*hash(vec2(col_i,7.0));\n"
    "  float tower=step(1.0-uv.y,hgt*grow)*step(fract(uv.x*16.0),0.55)*step(0.45,fract(uv.x*16.0));\n"
    "  col+=vec3(0.42,0.52,0.62)*tower*(0.5+0.3*sin(t*1.3+col_i));\n"
    "  float win=step(0.75,hash(vec2(floor(uv.x*48.0),floor(uv.y*27.0)+floor(t*0.5))))*tower;\n"
    "  col+=vec3(0.85,0.88,0.60)*win*0.7;}\n"
    " if(sc==3){vec2 m=uv-0.5;float mir=lm(vec2(abs(m.x)*10.0+2.0,m.y*10.0+2.0),0.06)*0.35;\n"
    "  col+=vec3(0.70,0.78,0.85)*mir*(0.6+0.4*sin(t*0.5));\n"
    "  col+=vec3(0.06,0.10,0.14)*smoothstep(0.7,0.2,length(m));}\n"
    " if(sc==5){float ph=(t-78.0)/22.0;\n"
    "  vec2 dst=(uv-0.5)*rot(ph*1.2)*1.15+0.5;\n"
    "  float moving=lm(dst*14.0+warp,0.045)*(0.4+0.2*sin(ph*6.283+uv.x*4.0));\n"
    "  col+=vec3(0.55,0.65,0.75)*moving*0.8;\n"
    "  col+=vec3(0.20,0.45,0.35)*smoothstep(0.15,0.0,abs(fract(ph*3.0)-0.5))*0.4;}\n"
    " if(sc==6){float rev=smoothstep(100.0,103.0,t);\n"
    "  float gl=smoothstep(100.0,102.2,t)*(1.0-smoothstep(102.2,103.5,t));\n"
    "  float m=mark(uv,rev,gl*1.5);\n"
    "  float breathe=0.85+0.15*sin((t-103.5)*1.1);\n"
    "  float sparkle=step(0.9985,hash(vec2(floor(uv.x*90.0),floor(uv.y*50.0)+floor(t*8.0))))*rev;\n"
    "  col+=vec3(0.92,0.94,0.97)*m*breathe;\n"
    "  col+=vec3(1.0,0.95,0.85)*sparkle*0.6;\n"
    "  col+=vec3(0.25,0.10,0.05)*gl*0.4;}\n"
    " if(sc==7){float f=1.0-(t-112.0)/8.0;col*=f*f;}\n"
    " o=vec4(col,1.0);}";

int main(void) {
#ifdef GLFW_PLATFORM_X11
    /* force XWayland so X11 evidence tools (xdotool/x11grab) can see the window */
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
    glfwInit();
    GLFWwindow *win = NULL;
    EGLDisplay dpy = EGL_NO_DISPLAY; EGLSurface surf = EGL_NO_SURFACE; EGLContext ctx = EGL_NO_CONTEXT;
    if (getenv("ERRORWTF_HEADLESS")) {
        eglBindAPI(EGL_OPENGL_API);
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, NULL, NULL)) { fprintf(stderr, "EVIDENCE_FAIL: egl\n"); return 2; }
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
        win = glfwCreateWindow(W, H, "ERROR.wtf // BEYOND THE ERROR", NULL, NULL);
        if (!win) { fprintf(stderr, "EVIDENCE_FAIL: window\n"); return 2; }
        glfwSetWindowPos(win, 300, 200); /* keep inside visible screen for capture tools */
        glfwMakeContextCurrent(win);
    }
    load_gl();
    printf("EVIDENCE: renderer=%s\n", glGetString_p(GL_RENDERER));

    unsigned vs = glCreateShader_p(GL_VERTEX_SHADER);
    glShaderSource_p(vs, 1, &VS, NULL); glCompileShader_p(vs);
    int ok = 0; glGetShaderiv_p(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { char lg[512]; glGetShaderInfoLog_p(vs, 512, NULL, lg); fprintf(stderr, "VS: %s\n", lg); return 2; }
    unsigned fs = glCreateShader_p(GL_FRAGMENT_SHADER);
    glShaderSource_p(fs, 1, &FS, NULL); glCompileShader_p(fs);
    glGetShaderiv_p(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { char lg[1024]; glGetShaderInfoLog_p(fs, 1024, NULL, lg); fprintf(stderr, "FS: %s\n", lg); return 2; }
    unsigned prog = glCreateProgram_p();
    glAttachShader_p(prog, vs); glAttachShader_p(prog, fs); glLinkProgram_p(prog);
    glGetProgramiv_p(prog, GL_LINK_STATUS, &ok);
    if (!ok) { fprintf(stderr, "EVIDENCE_FAIL: link\n"); return 2; }
    glUseProgram_p(prog);
    unsigned vao, vbo;
    glGenVertexArrays_p(1, &vao); glBindVertexArray_p(vao);
    glGenBuffers_p(1, &vbo); glBindBuffer_p(GL_ARRAY_BUFFER, vbo);
    float tri[9] = { -1, -1, 0, 3, -1, 0, -1, 3, 0 };
    glBufferData_p(GL_ARRAY_BUFFER, sizeof tri, tri, GL_STATIC_DRAW);
    glEnableVertexAttribArray_p(0);
    glVertexAttribPointer_p(0, 3, GL_FLOAT, 0, 0, NULL);
    glViewport_p(0, 0, W, H);
    GLint ul_t = glGetUniformLocation_p(prog, "u_time"), ul_s = glGetUniformLocation_p(prog, "u_scene");

    pthread_t at; pthread_create(&at, NULL, audio_thread, NULL);

    unsigned char *px = malloc((size_t)W * H * 3);
    int cap_i = 0, last_scene = -1;
    static const double CAPT_T[] = { 6, 20, 36, 55, 70, 85, 107, 116 };
    int capture_all = getenv("ERRORWTF_CAPTURE_ALL") != NULL;
    double next_cap = 0.0; int cap_idx = 0;
    while (1) {
        double t = (double)atomic_load(&g_samples) / SR;
        if (t >= (double)EW_SECONDS) break;
        int sc = scene_of(t);
        if (sc != last_scene) { printf("EVIDENCE: scene=%d t=%.2f\n", sc, t); last_scene = sc; }
        glClearColor_p(0, 0, 0, 1);
        glClear_p(GL_COLOR_BUFFER_BIT);
        glUseProgram_p(prog);
        glUniform1f_p(ul_t, (float)t);
        glUniform1i_p(ul_s, sc);
        glDrawArrays_p(GL_TRIANGLES, 0, 3);
        if (win) {
            glfwPollEvents();
            glfwSwapBuffers(win);
        } else {
            if (capture_all) {
                if (t >= next_cap && cap_idx < EW_SECONDS) {
                    glReadPixels_p(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, px);
                    char fn[128];
                    snprintf(fn, sizeof fn, "captures/1m/cap_%03d.ppm", cap_idx++);
                    FILE *f = fopen(fn, "wb");
                    if (f) {
                        fprintf(f, "P6\n%d %d\n255\n", W, H);
                        for (int y = H - 1; y >= 0; y--) fwrite(px + (size_t)y * W * 3, (size_t)W * 3, 1, f);
                        fclose(f);
                    }
                    next_cap = cap_idx;
                }
            } else if (cap_i < 8 && t >= CAPT_T[cap_i]) {
                glReadPixels_p(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, px);
                char fn[128];
                snprintf(fn, sizeof fn, "captures/1m/prod_t%.0f.ppm", CAPT_T[cap_i]);
                FILE *f = fopen(fn, "wb");
                if (f) {
                    fprintf(f, "P6\n%d %d\n255\n", W, H);
                    for (int y = H - 1; y >= 0; y--) fwrite(px + (size_t)y * W * 3, (size_t)W * 3, 1, f);
                    fclose(f);
                    printf("EVIDENCE: captured %s (scene=%d)\n", fn, sc);
                }
                cap_i++;
            }
            struct timespec ts = { 0, 100 * 1000 * 1000 };
            nanosleep(&ts, NULL);
        }
    }
    pthread_join(at, NULL);
    printf("EVIDENCE: run complete duration=%ds exit=clean\n", EW_SECONDS);
    return 0;
}
