#include "ftps4.h"

#define PAYLOAD_NAME "FTPS5 v1.4"

int netdbg_sock;
void* ptr_syscall;

void resolve_syscall() {
    ptr_syscall = (void *)(f_munmap + 0x0A);
}

void klog(const char* fmt, ...) {
    char buffer[512];
    f_memset(&buffer, 0, sizeof(buffer));
    va_list args;
    va_start(args, fmt);
    f_vsprintf(buffer, fmt, args);
    va_end(args);

    f_sceKernelDebugOutText(0, buffer);
}

// https://github.com/OSM-Made/PS4-Notify
void printf_notification(const char* fmt, ...) {
    SceNotificationRequest noti_buffer;
    f_memset(&noti_buffer, 0, sizeof(noti_buffer));
    va_list args;
    va_start(args, fmt);
    f_vsprintf(noti_buffer.message, fmt, args);
    va_end(args);
    f_sceKernelSendNotificationRequest(0, (SceNotificationRequest * ) & noti_buffer, sizeof(noti_buffer), 0);
}

int get_ip_address(char *ip_address) {
    int ret;
    SceNetCtlInfo info;

    ret = f_sceNetCtlInit();
    if (ret < 0)
        goto error;

    ret = f_sceNetCtlGetInfo(14, &info);

    if (ret < 0)
        goto error;

    f_memcpy(ip_address, info.ip_address, sizeof(info.ip_address));

    f_sceNetCtlTerm();

    return ret;

    error:
    ip_address = NULL;
    return -1;
}

void run_ftp(char *ip_address) {

    printf_notification(PAYLOAD_NAME"\nFTP listening on %s:%i\nPorted by zecoxao + SiSTRo\nOriginal source by bigboss", ip_address, PS4_PORT);

    ftp_init(ip_address, PS4_PORT);

    //we finish this connecting to ftp and calling quit command. It will call ftp_fini
    while (1) {
        f_sceKernelUsleep(100 * 1000);
    }

    ftp_fini();
}

int payload_main(struct payload_args *args) {
    dlsym_t *dlsym = args->dlsym;

    int libKernel = 0x2001;
    int ret = 0;
    char ip_address[16];

    dlsym(libKernel, "sceKernelLoadStartModule", &f_sceKernelLoadStartModule);
    dlsym(libKernel, "sceKernelDebugOutText", &f_sceKernelDebugOutText);
    dlsym(libKernel, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);
    dlsym(libKernel, "sceKernelUsleep", &f_sceKernelUsleep);
    dlsym(libKernel, "scePthreadMutexLock", &f_scePthreadMutexLock);
    dlsym(libKernel, "scePthreadMutexUnlock", &f_scePthreadMutexUnlock);
    dlsym(libKernel, "scePthreadExit", &f_scePthreadExit);
    dlsym(libKernel, "scePthreadMutexInit", &f_scePthreadMutexInit);
    dlsym(libKernel, "scePthreadCreate", &f_scePthreadCreate);
    dlsym(libKernel, "scePthreadMutexDestroy", &f_scePthreadMutexDestroy);
    dlsym(libKernel, "scePthreadJoin", &f_scePthreadJoin);
    dlsym(libKernel, "socket", &f_socket);
    dlsym(libKernel, "bind", &f_bind);
    dlsym(libKernel, "listen", &f_listen);
    dlsym(libKernel, "accept", &f_accept);
    dlsym(libKernel, "open", &f_open);
    dlsym(libKernel, "read", &f_read);
    dlsym(libKernel, "write", &f_write);
    dlsym(libKernel, "close", &f_close);
    dlsym(libKernel, "stat", &f_stat);
    dlsym(libKernel, "fstat", &f_fstat);
    dlsym(libKernel, "rename", &f_rename);
    dlsym(libKernel, "rmdir", &f_rmdir);
    dlsym(libKernel, "mkdir", &f_mkdir);
    dlsym(libKernel, "getdents", &f_getdents);
    dlsym(libKernel, "unlink", &f_unlink);
    dlsym(libKernel, "readlink", &f_readlink);
    dlsym(libKernel, "lseek", &f_lseek);
    dlsym(libKernel, "puts", &f_puts);
    dlsym(libKernel, "mmap", &f_mmap);
    dlsym(libKernel, "munmap", &f_munmap);
    dlsym(libKernel, "_read", &f__read);

    int libNet = f_sceKernelLoadStartModule("libSceNet.sprx", 0, 0, 0, 0, 0);
    dlsym(libNet, "sceNetSocket", &f_sceNetSocket);
    dlsym(libNet, "sceNetConnect", &f_sceNetConnect);
    dlsym(libNet, "sceNetHtons", &f_sceNetHtons);
    dlsym(libNet, "sceNetAccept", &f_sceNetAccept);
    dlsym(libNet, "sceNetSend", &f_sceNetSend);
    dlsym(libNet, "sceNetInetNtop", &f_sceNetInetNtop);
    dlsym(libNet, "sceNetSocketAbort", &f_sceNetSocketAbort);
    dlsym(libNet, "sceNetBind", &f_sceNetBind);
    dlsym(libNet, "sceNetListen", &f_sceNetListen);
    dlsym(libNet, "sceNetSocketClose", &f_sceNetSocketClose);
    dlsym(libNet, "sceNetHtonl", &f_sceNetHtonl);
    dlsym(libNet, "sceNetInetPton", &f_sceNetInetPton);
    dlsym(libNet, "sceNetGetsockname", &f_sceNetGetsockname);
    dlsym(libNet, "sceNetRecv", &f_sceNetRecv);
    dlsym(libNet, "sceNetErrnoLoc", &f_sceNetErrnoLoc);
    dlsym(libNet, "sceNetSetsockopt", &f_sceNetSetsockopt);

    int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
    dlsym(libC, "vsprintf", &f_vsprintf);
    dlsym(libC, "memset", &f_memset);
    dlsym(libC, "sprintf", &f_sprintf);
    dlsym(libC, "snprintf", &f_snprintf);
    dlsym(libC, "snprintf_s", &f_snprintf_s);
    dlsym(libC, "strcat", &f_strcat);
    dlsym(libC, "free", &f_free);
    dlsym(libC, "memcpy", &f_memcpy);
    dlsym(libC, "strcpy", &f_strcpy);
    dlsym(libC, "strncpy", &f_strncpy);
    dlsym(libC, "sscanf", &f_sscanf);
    dlsym(libC, "malloc", &f_malloc);
    dlsym(libC, "calloc", &f_calloc);
    dlsym(libC, "realloc", &f_realloc);
    dlsym(libC, "strlen", &f_strlen);
    dlsym(libC, "strdup", &f_strdup);
    dlsym(libC, "strcmp", &f_strcmp);
    dlsym(libC, "strchr", &f_strchr);
    dlsym(libC, "strrchr", &f_strrchr);
    dlsym(libC, "gmtime_s", &f_gmtime_s);
    dlsym(libC, "time", &f_time);
    dlsym(libC, "localtime", &f_localtime);

    int libNetCtl = f_sceKernelLoadStartModule("libSceNetCtl.sprx", 0, 0, 0, 0, 0);
    dlsym(libNetCtl, "sceNetCtlInit", &f_sceNetCtlInit);
    dlsym(libNetCtl, "sceNetCtlTerm", &f_sceNetCtlTerm);
    dlsym(libNetCtl, "sceNetCtlGetInfo", &f_sceNetCtlGetInfo);

    ret = get_ip_address(ip_address);

    if (ret < 0) {
        printf_notification(PAYLOAD_NAME"\nUnable to get IP address");
        return 0;
    }

    resolve_syscall();

#ifdef PERSISTENT
#warning "Persistent enabled."
    // alert user what build they are using
    printf_notification(PAYLOAD_NAME"\n"
        "Persistent enabled.\n"
        "Closing will keep FTP running.");
    // Great thanks to Specter for fork idea!
    f_exit = (void *) (f__read + 0xE30);
    f_fork = (void *) (f__read + 0x1DE0);

    if (f_fork() == 0) {
        run_ftp(ip_address);
        f_exit(0);
    }
#else
#warning "Persistent not enabled."
    // alert user what build they are using
    printf_notification(PAYLOAD_NAME"\n"
        "Persistent not enabled.\n"
        "Closing will exit FTP.");
    run_ftp(ip_address);
#endif

    return 0;
}
