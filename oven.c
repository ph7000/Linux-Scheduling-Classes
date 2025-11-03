// SPDX-License-Identifier: GPL-2.0
/*
 * oven-task scheduling class.
 *
 * The oven task IS NOT YET but will be
 * the default priority task in the system; it will preempt
 * nothing and be preempted by everything.
 *
 * 
 */

const struct sched_class oven_sched_class;



static void wakeup_preempt_oven(struct rq *rq, struct task_struct *p, int flags)
{
	// Only preempt if current task has run for a while
	if (rq->curr && rq->curr->sched_class == &oven_sched_class) {
		struct task_struct *curr = rq->curr;
		
		// If current task has used its slice, preempt
		if (curr->se.sum_exec_runtime >= curr->se.slice)
			resched_curr(rq);
	}
}

static int balance_oven(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	// Simple: no load balancing needed for now
	return 0;
}

static int select_task_rq_oven(struct task_struct *p, int prev_cpu, int wake_flags)
{
	// Pick least loaded CPU
	int cpu, best_cpu = prev_cpu;
	unsigned long min_load = ULONG_MAX;
	
	for_each_online_cpu(cpu) {
		struct rq *rq = cpu_rq(cpu);
		unsigned long load = rq->oven.oven_nr_running;
		
		if (load < min_load) {
			min_load = load;
			best_cpu = cpu;
		}
	}
	
	return best_cpu;
}

static void task_fork_oven(struct task_struct *p)
{
	// Simple: do nothing on fork
}

static void switched_from_oven(struct rq *rq, struct task_struct *p)
{
	// Called when task switches away from OVEN class
	// Simple: do nothing
}





static unsigned int get_rr_interval_oven(struct rq *rq, struct task_struct *task)
{
	// Return time slice in microseconds
	// Example: 10ms = 10000 microseconds
	return 10000;
}

void init_oven_rq(struct oven_rq *oven_rq)
{
	printk(KERN_ALERT "OVEN: Initializing runqueue at %px\n", oven_rq);
	// printk("initializing the list head\n");
	INIT_LIST_HEAD(&oven_rq->run_list);
	oven_rq->oven_nr_running = 0;
	// printk(KERN_ALERT "OVEN: Initialization complete\n");
}

void __init init_sched_oven_class(void)
{
	unsigned int i;

	for_each_possible_cpu(i) {
		struct rq *rq = cpu_rq(i);
		init_oven_rq(&rq->oven);
	}
}


// static void check_preempt_curr_oven(struct rq *rq, struct task_struct *p, int flags)
// {
// 	printk(KERN_ALERT "OVEN: check_preempt_curr\n");
// 	// Simple: always preempt
// 	resched_curr(rq);
// }

static void yield_task_oven(struct rq *rq)
{
	printk(KERN_ALERT "OVEN: yield_task\n");
}

static void switched_to_oven(struct rq *rq, struct task_struct *p)
{
	printk(KERN_ALERT "OVEN: switched_to for %s\n", p->comm);
}

static void prio_changed_oven(struct rq *rq, struct task_struct *p, int oldprio)
{
	printk(KERN_ALERT "OVEN: prio_changed for %s\n", p->comm);
}

static void
enqueue_task_oven(struct rq *rq, struct task_struct *p, int flags)
{
	if (p->pid == 1) {
		printk(KERN_ALERT "OVEN: Init process (PID 1) %s is using OVEN!\n", p->comm);
	}
	printk(KERN_ALERT "OVEN: enqueueing the oven task\n");
	struct sched_oven_entity *oven_se = &p->oven;
	struct oven_rq *oven_rq = &rq->oven;


	list_add_tail(&oven_se->run_list, &oven_rq->run_list);

	/*
	 * Mark the entity as being on the runqueue
	 */
	oven_se->on_rq = 1;

	/*
	 * Increment the number of oven tasks on this runqueue
	 */
	oven_rq->oven_nr_running++;
	
	/*
	 * Update the total number of runnable tasks on this CPU
	 */
	add_nr_running(rq, 1);

	printk(KERN_ALERT "OVEN: oven enqueue completed\n");

}

/*
 * Update the current task's runtime statistics
 */
/*
 * Update the current task's runtime statistics (minimal version)
 */
static void update_curr_oven(struct rq *rq)
{
    struct task_struct *curr = rq->curr;
    struct sched_oven_entity *oven_se;
    u64 now, delta_exec;
    
    if (!curr || curr->sched_class != &oven_sched_class)
        return;
    
    oven_se = &curr->oven;
    now = rq_clock_task(rq);
    
    // Calculate elapsed time
    delta_exec = now - oven_se->exec_start;
    
    if ((s64)delta_exec <= 0)
        return;
    
    // Update OVEN's runtime
    oven_se->sum_exec_runtime += delta_exec;
    oven_se->exec_start = now;
    
    printk(KERN_ALERT "OVEN: update_curr delta=%llu, total=%llu\n",
           delta_exec, oven_se->sum_exec_runtime);
}

static void task_tick_oven(struct rq *rq, struct task_struct *p, int queued)
{
    struct sched_oven_entity *oven_se = &p->oven;
    u64 now = rq_clock_task(rq);
    u64 delta_exec;
    
    printk(KERN_ALERT "OVEN: task_tick for %s\n", p->comm);
    
    // Calculate time since task started running
    delta_exec = now - oven_se->exec_start;
    oven_se->sum_exec_runtime += delta_exec;
    oven_se->exec_start = now;  // Reset for next tick
    
    printk(KERN_ALERT "OVEN: runtime=%llu, slice=%llu\n", 
           oven_se->sum_exec_runtime, oven_se->time_slice);
    
    // Check if exceeded time slice
    if (oven_se->sum_exec_runtime >= oven_se->time_slice) {
        printk(KERN_ALERT "OVEN: Time slice exceeded, preempting!\n");
        resched_curr(rq);
        oven_se->sum_exec_runtime = 0;  // Reset for next run
    }
}

static void put_prev_task_oven(struct rq *rq, struct task_struct *prev, struct task_struct *next)
{
	/* Update runtime before context switch */
	update_curr_oven(rq);
	
	/*
	 * prev: the task being switched away from
	 * next: the task being switched to (may be NULL if not known yet)
	 * 
	 * OVEN-specific cleanup when task is switched away
	 * (if needed, otherwise this is sufficient)
	 */
}

static inline struct task_struct *oven_task_of(struct sched_oven_entity *oven_se)
{
	return container_of(oven_se, struct task_struct, oven);
}

static struct sched_oven_entity *pick_next_oven_entity(struct oven_rq *oven_rq)
{
	struct sched_oven_entity *next = NULL;

	/*
	 * If the list is empty, nothing to run
	 */
	if (list_empty(&oven_rq->run_list))
		return NULL;

	/*
	 * Get the first entry in the list (FIFO order)
	 * list.next points to the first element
	 */
	next = list_first_entry(&oven_rq->run_list, 
	                        struct sched_oven_entity, 
	                        run_list);

	return next;
}

static struct task_struct *_pick_next_task_oven(struct rq *rq)
{
	struct sched_oven_entity *oven_se;
	struct oven_rq *oven_rq = &rq->oven;

	if (!oven_rq->oven_nr_running)
		return NULL;

	oven_se = pick_next_oven_entity(oven_rq);
	if (unlikely(!oven_se))
		return NULL;

	return oven_task_of(oven_se);
}

static struct task_struct *pick_task_oven(struct rq *rq)
{
	struct task_struct *p;

	p = _pick_next_task_oven(rq);

	return p;
}


/*
 * Dequeue an oven task from the runqueue
 */
static bool dequeue_task_oven(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_oven_entity *oven_se = &p->oven;
	struct oven_rq *oven_rq = &rq->oven;

	/*
	 * Remove from the run list if this is an actual dequeue
	 * (not just a stats update)
	 */
	if (move_entity(flags)) {
		WARN_ON_ONCE(!oven_se->on_rq);
		list_del_init(&oven_se->run_list);
	}

	/*
	 * Mark as not on runqueue
	 */
	oven_se->on_rq = 0;

	/*
	 * Decrement the number of oven tasks
	 */
	oven_rq->oven_nr_running--;

	/*
	 * Update the total number of runnable tasks on this CPU
	 */
	sub_nr_running(rq, 1);

	/*
	 * Return true to indicate the task was successfully dequeued
	 */
	return true;
}

static void set_next_task_oven(struct rq *rq, struct task_struct *p, bool first)
{
    struct sched_oven_entity *oven_se = &p->oven;
    
    printk(KERN_ALERT "OVEN: set_next_task for %s\n", p->comm);
    
    // Initialize OVEN's exec_start
    oven_se->exec_start = rq_clock_task(rq);
    
    // Initialize time slice if not set
    if (oven_se->time_slice == 0) {
        oven_se->time_slice = 10000000ULL;  // 10ms
        printk(KERN_ALERT "OVEN: Initialized time_slice=%llu\n", 
               oven_se->time_slice);
    }
    
    // Reset runtime counter
    oven_se->sum_exec_runtime = 0;
}

static struct task_struct *pick_next_task_oven(struct rq *rq, struct task_struct *prev)
{
	struct oven_rq *oven_rq = &rq->oven;
	struct sched_oven_entity *oven_se;
	struct task_struct *p;

	// printk(KERN_ALERT "OVEN: pick_next_task called, nr_running=%d\n",
	//        oven_rq->oven_nr_running);

	if (oven_rq->oven_nr_running == 0) {
		// printk(KERN_ALERT "OVEN: No tasks, returning NULL\n");
		return NULL;
	}

	if (list_empty(&oven_rq->run_list)) {
		// printk(KERN_ALERT "OVEN: List empty but nr_running>0! Bug!\n");
		return NULL;
	}

	oven_se = list_first_entry(&oven_rq->run_list,
				   struct sched_oven_entity,
				   run_list);
	p = container_of(oven_se, struct task_struct, oven);
	
	// printk(KERN_ALERT "OVEN: Picked task %s (pid=%d)\n", p->comm, p->pid);
	return p;
}

DEFINE_SCHED_CLASS(oven) = {
	.enqueue_task		= enqueue_task_oven,
	.dequeue_task		= dequeue_task_oven,
	
	.yield_task		= yield_task_oven,
	.wakeup_preempt		= wakeup_preempt_oven,
	
	.pick_next_task		= pick_next_task_oven,
	.pick_task		= pick_task_oven,
	
	.put_prev_task		= put_prev_task_oven,
	.set_next_task		= set_next_task_oven,
	
	.balance		= balance_oven,
	.select_task_rq		= select_task_rq_oven,
	
	.task_tick		= task_tick_oven,
	.task_fork		= task_fork_oven,
	
	.prio_changed		= prio_changed_oven,
	.switched_to		= switched_to_oven,
	.switched_from		= switched_from_oven,
	
	.update_curr		= update_curr_oven,
	
	.get_rr_interval	= get_rr_interval_oven,
};