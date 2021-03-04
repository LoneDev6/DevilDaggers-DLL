
unsigned long __stdcall main_thread(void *arg);
void setCursorPos_proxy(int &x, int &y);
void render_loop_proxy(HDC hdc);
static void glfw_error_callback(int error, const char* description);