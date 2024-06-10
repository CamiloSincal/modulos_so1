#include <linux/module.h> // THIS_MODULE, MODULE_VERSION, ...
#include <linux/init.h>   // module_{init,exit}
#include <linux/proc_fs.h>
#include <linux/sched/signal.h> // for_each_process()
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mm.h> // get_mm_rss()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo 23");
MODULE_DESCRIPTION("Información CPU");
MODULE_VERSION("1.0");

struct task_struct *task;       // sched.h para tareas/procesos
struct task_struct *task_child; // index de tareas secundarias
struct list_head *list;         // lista de cada tarea

static int escribir_a_proc(struct seq_file *file_proc, void *v)
{
    unsigned long total_cpu_time = jiffies_to_msecs(get_jiffies_64());
    unsigned long total_usage = 0;
    unsigned long rss;
    unsigned long total_ram_pages;
    int running = 0;
    int sleeping = 0;
    int zombie = 0;
    int stopped = 0;

    total_ram_pages = totalram_pages();
    if (!total_ram_pages) {
        pr_err("No hay memoria disponible\n");
        return -EINVAL;
    }

    #ifndef CONFIG_MMU
        pr_err("No MMU, no puede calcularse el RSS.\n");
        return -EINVAL;
    #endif

    for_each_process(task) {
        unsigned long cpu_time = jiffies_to_msecs(task->utime + task->stime);
        total_usage += cpu_time;
    }

    seq_printf(file_proc, "{\n\"cpu_total\":%lu,\n", total_cpu_time);
    seq_printf(file_proc, "\"cpu_porcentaje\":%lu,\n", (total_usage * 100) / total_cpu_time);
    seq_printf(file_proc, "\"processes\":[\n");

    int b = 0;
    for_each_process(task)
    {
        if (task->mm)
        {
            rss = get_mm_rss(task->mm) << PAGE_SHIFT;
        }
        else
        {
            rss = 0;
        }

        if (b == 0)
        {
            seq_printf(file_proc, "{");
            b = 1;
        }
        else
        {
            seq_printf(file_proc, ",{");
        }

        seq_printf(file_proc, "\"pid\":%d,\n", task->pid); //Identicador de cada proceso 
        seq_printf(file_proc, "\"name\":\"%s\",\n", task->comm); //Nombre
        seq_printf(file_proc, "\"user\": %d,\n", from_kuid(&init_user_ns, task_uid(task))); //Usuario 
        seq_printf(file_proc, "\"state\":%ld,\n", task->state); //Estado
        int porcentaje = (rss * 100) / total_ram_pages;
        seq_printf(file_proc, "\"ram\":%d,\n", porcentaje);

        seq_printf(file_proc, "\"child\":[\n");
        int a = 0;
        list_for_each(list, &(task->children))
        {
            task_child = list_entry(list, struct task_struct, sibling);
            if (a != 0)
            {
                seq_printf(file_proc, ",{");
            }
            else
            {
                seq_printf(file_proc, "{");
                a = 1;
            }
            seq_printf(file_proc, "\"pid\":%d,\n", task_child->pid);
            seq_printf(file_proc, "\"name\":\"%s\",\n", task_child->comm);
            seq_printf(file_proc, "\"state\":%ld,\n", task_child->state);
            seq_printf(file_proc, "\"pidPadre\":%d\n", task->pid);
            seq_printf(file_proc, "}\n");
        }
        seq_printf(file_proc, "\n]");

        if (task->state == 0)
        {
            running += 1;
        }
        else if (task->state == 1)
        {
            sleeping += 1;
        }
        else if (task->state == 4)
        {
            zombie += 1;
        }
        else
        {
            stopped += 1;
        }
        seq_printf(file_proc, "}\n");
    }
    b = 0;
    seq_printf(file_proc, "],\n");
    seq_printf(file_proc, "\"running\":%d,\n", running);
    seq_printf(file_proc, "\"sleeping\":%d,\n", sleeping);
    seq_printf(file_proc, "\"zombie\":%d,\n", zombie);
    seq_printf(file_proc, "\"stopped\":%d,\n", stopped);
    seq_printf(file_proc, "\"total\":%d\n", running + sleeping + zombie + stopped);
    seq_printf(file_proc, "}\n");
    return 0;
}

static int abrir_aproc(struct inode *inode, struct file *file)
{
    return single_open(file, escribir_a_proc, NULL);
}

static const struct file_operations archivo_operaciones =
{
    .owner = THIS_MODULE,
    .open = abrir_aproc,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init modulo_init(void)
{
    proc_create("cpu_so1_jun2024", 0, NULL, &archivo_operaciones);
    printk(KERN_INFO "Insertar módulo CPU\n");
    return 0;
}

static void __exit modulo_cleanup(void)
{
    remove_proc_entry("cpu_so1_jun2024", NULL);
    printk(KERN_INFO "Remover módulo CPU\n");
}

module_init(modulo_init);
module_exit(modulo_cleanup);
