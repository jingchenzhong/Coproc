#include "Coproc.h"
#include "Scheduler.h"

void Coproc::pass(void* param) {
		static_cast<Coproc*>(param)->Run();
}

Coproc::Coproc(int fd, int stack_size) : fd_(fd) {
	ctx_ = (ucontext_t *)malloc(sizeof(ucontext_t));
    ctx_->uc_stack.ss_sp = (char *)malloc(stack_size);
    ctx_->uc_stack.ss_size = stack_size;

    getcontext(ctx_); 
    makecontext(ctx_, (void (*)(void))pass, 1, this);
    st_ = READY;
}

Coproc::~Coproc() {
    free(ctx_->uc_stack.ss_sp);
    free(ctx_);  
}
void Coproc::Yield() {
	st_ = SUSPEND;
    Scheduler* sche = Scheduler::getInstance();
    swapcontext(ctx_, sche->GetMainCtx());
}

void Coproc::Resume() {
	st_ = RUNNING;
	Scheduler* sche = Scheduler::getInstance();
	swapcontext(sche->GetMainCtx(), ctx_);
}

void Coproc::Stop() {
    st_ = STOP;
    Scheduler* sche = Scheduler::getInstance();
    swapcontext(ctx_, sche->GetMainCtx());
}

void Coproc::Delete() {
    Scheduler* sche = Scheduler::getInstance();
    sche->DeleteCoproc(fd_);
    swapcontext(ctx_, sche->GetMainCtx());
}
