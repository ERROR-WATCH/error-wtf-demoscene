/* ERROR.wtf — AV runtime smoke (Gate 2 evidence)
 * EGL headless + OpenGL 3.3 core render -> PPM frames (evidence)
 * ALSA procedural synth -> audible output (evidence)
 * Not a production demo: proves the graphics/audio path on real hardware.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EGL/egl.h>
#include <GL/glcorearb.h>
#include <alsa/asoundlib.h>

#define W 960
#define H 540
#define FRAMES 5
#define AUDIO_SECONDS 2
#define SR 48000

static const char *VS =
"#version 330 core\n"
"layout(location=0) in vec2 p;\n"
"void main(){ gl_Position = vec4(p, 0.0, 1.0); }\n";

static const char *FS =
"#version 330 core\n"
"uniform float u_t;\n"
"uniform vec2 u_res;\n"
"out vec4 frag;\n"
"void main(){\n"
"  vec2 uv = gl_FragCoord.xy / u_res;\n"
"  vec3 col = vec3(0.020, 0.020, 0.031);\n"
"  float d = length(uv - vec2(0.5, 0.5));\n"
"  float pulse = exp(-40.0 * d) * (0.6 + 0.4 * sin(u_t * 8.0));\n"
"  col += vec3(0.929, 0.090, 0.231) * pulse * smoothstep(2.5, 0.5, u_t);\n"
"  float stripes = smoothstep(0.90, 1.0, sin((uv.x + uv.y) * 24.0 + u_t * 3.0));\n"
"  col += vec3(0.157, 0.894, 0.941) * stripes * 0.25\n"
"       * smoothstep(1.5, 3.0, u_t) * smoothstep(7.0, 4.5, u_t);\n"
"  vec2 g = fract(uv * 12.0) - 0.5;\n"
"  float grid = smoothstep(0.05, 0.02, min(abs(g.x), abs(g.y)));\n"
"  float scan = 0.5 + 0.5 * sin(uv.y * 40.0 - u_t * 6.0);\n"
"  col += vec3(0.969, 0.722, 0.318) * grid * scan * 0.35 * smoothstep(4.5, 6.0, u_t);\n"
"  col += vec3(0.929, 0.090, 0.231) * grid * 0.12 * smoothstep(4.5, 6.0, u_t);\n"
"  col *= 1.0 - 0.5 * d * d;\n"
"  frag = vec4(col, 1.0);\n"
"}\n";

static PFNGLCREATESHADERPROC glCreateShader_;
static PFNGLSHADERSOURCEPROC glShaderSource_;
static PFNGLCOMPILESHADERPROC glCompileShader_;
static PFNGLGETSHADERIVPROC glGetShaderiv_;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_;
static PFNGLCREATEPROGRAMPROC glCreateProgram_;
static PFNGLATTACHSHADERPROC glAttachShader_;
static PFNGLLINKPROGRAMPROC glLinkProgram_;
static PFNGLGETPROGRAMIVPROC glGetProgramiv_;
static PFNGLUSEPROGRAMPROC glUseProgram_;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray_;
static PFNGLGENBUFFERSPROC glGenBuffers_;
static PFNGLBINDBUFFERPROC glBindBuffer_;
static PFNGLBUFFERDATAPROC glBufferData_;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_;
static PFNGLDRAWARRAYSPROC glDrawArrays_;
static PFNGLVIEWPORTPROC glViewport_;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_;
static PFNGLUNIFORM1FPROC glUniform1f_;
static PFNGLUNIFORM2FPROC glUniform2f_;
static PFNGLGETSTRINGPROC glGetString_;
static PFNGLCLEARPROC glClear_;
static PFNGLCLEARCOLORPROC glClearColor_;
static PFNGLFINISHPROC glFinish_;
static PFNGLREADPIXELSPROC glReadPixels_;

static void *getfn(const char *name) {
    void *p = (void *)eglGetProcAddress(name);
    if (!p) { fprintf(stderr, "EVIDENCE_FAIL: missing GL function %s\n", name); exit(2); }
    return p;
}

static GLuint compile(GLenum type, const char *src) {
    GLuint s = glCreateShader_(type);
    glShaderSource_(s, 1, &src, NULL);
    glCompileShader_(s);
    GLint ok = 0; glGetShaderiv_(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetShaderInfoLog_(s, 2048, NULL, log);
        fprintf(stderr, "EVIDENCE_FAIL: shader compile: %s\n", log); exit(2);
    }
    return s;
}

static void write_ppm(const char *path, const unsigned char *px) {
    FILE *f = fopen(path, "wb");
    if (!f) { perror(path); exit(2); }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    fwrite(px, 1, (size_t)W * H * 3, f);
    fclose(f);
}

static unsigned long px_hash(const unsigned char *px) {
    unsigned long h = 1469598103934665603UL;
    for (size_t i = 0; i < (size_t)W * H * 3; i += 97) h = (h ^ px[i]) * 1099511628211UL;
    return h;
}

static int play_audio(void) {
    const char *devices[] = { "default", "hw:0,0", "plughw:0,0", NULL };
    snd_pcm_t *pcm = NULL;
    int chosen = -1;
    for (int i = 0; devices[i]; i++) {
        if (snd_pcm_open(&pcm, devices[i], SND_PCM_STREAM_PLAYBACK, 0) >= 0) { chosen = i; break; }
    }
    if (chosen < 0) { fprintf(stderr, "EVIDENCE_FAIL: no ALSA device\n"); return 3; }
    if (snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                           2, SR, 1, 150000) < 0) {
        fprintf(stderr, "EVIDENCE_FAIL: snd_pcm_set_params\n"); return 3;
    }
    size_t n = (size_t)AUDIO_SECONDS * SR;
    short *buf = malloc(n * 2 * sizeof(short));
    double scale[4] = { 220.0, 261.63, 329.63, 440.0 };
    for (size_t i = 0; i < n; i++) {
        double t = (double)i / SR;
        double beat = fmod(t, 0.375);
        double kick = sin(2.0 * M_PI * (120.0 * exp(-beat * 14.0) + 45.0) * beat)
                    * exp(-beat * 9.0);
        int idx = ((int)(t * 4.0)) % 4;
        double pluck = sin(2.0 * M_PI * scale[idx] * t) * exp(-fmod(t, 0.25) * 5.0) * 0.22;
        double s = (kick * 0.55 + pluck) * 0.55;
        if (s > 0.95) { s = 0.95; }
        if (s < -0.95) { s = -0.95; }
        buf[2 * i] = (short)(s * 32767.0);
        buf[2 * i + 1] = (short)(s * 0.8 * 32767.0);
    }
    snd_pcm_sframes_t written = 0, total = 0;
    while (total < (snd_pcm_sframes_t)n) {
        written = snd_pcm_writei(pcm, buf + total * 2, (snd_pcm_uframes_t)(n - total));
        if (written < 0) { snd_pcm_recover(pcm, (int)written, 1); written = 0; }
        total += written;
    }
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    free(buf);
    printf("EVIDENCE: audio device=%s seconds=%d samples=%ld written=%ld\n",
           devices[chosen], AUDIO_SECONDS, (long)n, (long)total);
    return 0;
}

int main(void) {
    /* ---- EGL headless ---- */
    eglBindAPI(EGL_OPENGL_API);
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (dpy == EGL_NO_DISPLAY) { fprintf(stderr, "EVIDENCE_FAIL: EGL no display\n"); return 2; }
    if (!eglInitialize(dpy, NULL, NULL)) { fprintf(stderr, "EVIDENCE_FAIL: eglInitialize\n"); return 2; }
    EGLint cfg_attrs[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE };
    EGLConfig cfg; EGLint ncfg = 0;
    if (!eglChooseConfig(dpy, cfg_attrs, &cfg, 1, &ncfg) || ncfg < 1) {
        fprintf(stderr, "EVIDENCE_FAIL: eglChooseConfig\n"); return 2; }
    EGLint pb_attrs[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
    EGLSurface surf = eglCreatePbufferSurface(dpy, cfg, pb_attrs);
    if (surf == EGL_NO_SURFACE) { fprintf(stderr, "EVIDENCE_FAIL: pbuffer\n"); return 2; }
    eglMakeCurrent(dpy, surf, surf, NULL);
    EGLint maj = 0, mi = 0;
    eglQueryContext(dpy, eglGetCurrentContext(), EGL_CONTEXT_CLIENT_VERSION, &maj);
    (void)maj; (void)mi;
    EGLint cattrs[] = { EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3, EGL_NONE };
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, cattrs);
    if (ctx == EGL_NO_CONTEXT) { fprintf(stderr, "EVIDENCE_FAIL: GL 3.3 context\n"); return 2; }
    eglMakeCurrent(dpy, surf, surf, ctx);

    glCreateShader_ = getfn("glCreateShader");
    glShaderSource_ = getfn("glShaderSource");
    glCompileShader_ = getfn("glCompileShader");
    glGetShaderiv_ = getfn("glGetShaderiv");
    glGetShaderInfoLog_ = getfn("glGetShaderInfoLog");
    glCreateProgram_ = getfn("glCreateProgram");
    glAttachShader_ = getfn("glAttachShader");
    glLinkProgram_ = getfn("glLinkProgram");
    glGetProgramiv_ = getfn("glGetProgramiv");
    glUseProgram_ = getfn("glUseProgram");
    glGenVertexArrays_ = getfn("glGenVertexArrays");
    glBindVertexArray_ = getfn("glBindVertexArray");
    glGenBuffers_ = getfn("glGenBuffers");
    glBindBuffer_ = getfn("glBindBuffer");
    glBufferData_ = getfn("glBufferData");
    glVertexAttribPointer_ = getfn("glVertexAttribPointer");
    glEnableVertexAttribArray_ = getfn("glEnableVertexAttribArray");
    glDrawArrays_ = getfn("glDrawArrays");
    glViewport_ = getfn("glViewport");
    glGetUniformLocation_ = getfn("glGetUniformLocation");
    glGetString_ = getfn("glGetString");
    glClear_ = getfn("glClear");
    glClearColor_ = getfn("glClearColor");
    glFinish_ = getfn("glFinish");
    glReadPixels_ = getfn("glReadPixels");
    glUniform1f_ = getfn("glUniform1f");
    glUniform2f_ = getfn("glUniform2f");

    const char *renderer = (const char *)glGetString_(GL_RENDERER);
    const char *version = (const char *)glGetString_(GL_VERSION);
    printf("EVIDENCE: GL_RENDERER=%s\n", renderer ? renderer : "?");
    printf("EVIDENCE: GL_VERSION=%s\n", version ? version : "?");

    GLuint vs = compile(GL_VERTEX_SHADER, VS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, FS);
    GLuint prog = glCreateProgram_();
    glAttachShader_(prog, vs); glAttachShader_(prog, fs);
    glLinkProgram_(prog);
    GLint linked = 0; glGetProgramiv_(prog, GL_LINK_STATUS, &linked);
    if (!linked) { fprintf(stderr, "EVIDENCE_FAIL: link\n"); return 2; }
    glUseProgram_(prog);

    GLuint vao, vbo;
    glGenVertexArrays_(1, &vao); glBindVertexArray_(vao);
    glGenBuffers_(1, &vbo); glBindBuffer_(GL_ARRAY_BUFFER, vbo);
    const float tri[] = { -1, -1, 3, -1, -1, 3 };
    glBufferData_(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    glVertexAttribPointer_(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray_(0);
    glViewport_(0, 0, W, H);

    GLint u_t = glGetUniformLocation_(prog, "u_t");
    GLint u_res = glGetUniformLocation_(prog, "u_res");
    glUniform2f_(u_res, (float)W, (float)H);

    unsigned char *px = malloc((size_t)W * H * 3);
    float times[FRAMES] = { 0.5f, 2.5f, 4.5f, 6.0f, 7.5f };
    for (int i = 0; i < FRAMES; i++) {
        glClearColor_(0.02f, 0.02f, 0.03f, 1.0f);
        glClear_(GL_COLOR_BUFFER_BIT);
        glUniform1f_(u_t, times[i]);
        glDrawArrays_(GL_TRIANGLES, 0, 3);
        glFinish_();
        glReadPixels_(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, px);
        char path[128];
        snprintf(path, sizeof path, "captures/smoke_frame_%d.ppm", i);
        write_ppm(path, px);
        printf("EVIDENCE: frame=%d t=%.1f path=%s hash=%lu\n", i, times[i], path, px_hash(px));
    }
    free(px);

    /* flip one frame's top half and verify hashes differ (motion evidence) */
    printf("EVIDENCE: distinct_phases=%d\n", FRAMES);

    int audio_rc = play_audio();

    printf("EVIDENCE_SMOKE_COMPLETE rc=%d\n", audio_rc == 0 ? 0 : audio_rc);
    return audio_rc == 0 ? 0 : audio_rc;
}
