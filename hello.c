#include <asm/io.h>
#include <linux/hw_random.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>

struct rng_state *st;

struct rng_state {
  struct task_struct *rng_task;
  void *regs;
};

static int rng_kthread(void *data) {
  struct rng_state *st = data;

  while (!kthread_should_stop()) {
    msleep_interruptible(1000);
    printk("Tick\n");
  }
out:
  st->rng_task = NULL;

  return 0;
}

static void rng_start(struct rng_state *st) {
  if (st->rng_task)
    return;

  st->rng_task = kthread_run(rng_kthread, st, "hisi-hwrng");
  if (IS_ERR(st->rng_task))
    st->rng_task = NULL;
}

static void rng_stop(struct rng_state *st) {
  if (st->rng_task)
    kthread_stop(st->rng_task);
}

static int __init mod_init(void) {
  st = kmalloc(sizeof(struct rng_state), GFP_KERNEL);
  memset(st, 0, sizeof(struct rng_state));
  rng_start(st);

  printk(KERN_INFO "========================================\n");
  // SZ_4K
  st->regs = ioremap_nocache(0x10080204, 4);
  printk("Regs: %p\n", st->regs);
  printk("Random: %#x\n", readl(st->regs));
  return 0;
}

static void __exit mod_exit(void) {
  printk(KERN_INFO "hello: exited\n");
  rng_stop(st);
  iounmap(st->regs);
  kfree(st);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Ilyin <d.ilyin@openipc.org>");
