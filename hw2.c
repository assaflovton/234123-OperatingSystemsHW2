#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/list.h>
asmlinkage long sys_hello(void) {
printk("Hello, World!\n");
return 0;
}

asmlinkage long sys_set_weight(int weight){
	if( weight < 0 )
	{
		return -EINVAL;
	}
	current->weight = weight;
	return 0;
}
asmlinkage long sys_get_weight(void){
		//printf("weight returned: %d", current->weight)
		return current->weight;
}

static asmlinkage long calc_sum(struct task_struct* ts){
    struct list_head* l;
    struct task_struct* t;
    int sum = ts->weight;
    list_for_each(l, &ts->children) {
        t = list_entry(l, struct task_struct, sibling);
        sum+=calc_sum(t);
    }
    return sum;
}

asmlinkage long sys_get_children_sum(void){
	if(list_empty(&(current->children))!=0){
		return -ECHILD;
	}
    	return calc_sum(current) - current->weight;
}

static asmlinkage long find_heaviest(struct task_struct* ts, int cur_heavy, pid_t id_of_cur_heavy){
    if (ts->pid == 1){  					// if we got to init 
	if (cur_heavy > ts->weight)  		// if init is has a lower weight
		return id_of_cur_heavy; 	// return id of heaviest
	return 1;  				        // return init id which is 1
    }
    if(ts->weight > cur_heavy){  			// we found a heavier parent
		cur_heavy = ts->weight;  		// update the cur heaviest weight
		id_of_cur_heavy = ts->pid;  		// update the cur id of heaviest process
	}
    return find_heaviest(ts->parent, cur_heavy, id_of_cur_heavy);  // look for a heaviesr ancestor
}

asmlinkage long sys_get_heaviest_ancestor(void){
	return find_heaviest(current, current->weight, current->pid);
}
