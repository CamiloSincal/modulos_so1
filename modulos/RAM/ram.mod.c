#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x8d7e33a7, "module_layout" },
	{ 0xbc75f7ad, "single_release" },
	{ 0xfe3760b1, "seq_read" },
	{ 0x9df29418, "seq_lseek" },
	{ 0x5f5f81ad, "remove_proc_entry" },
	{ 0xc5850110, "printk" },
	{ 0x7b5361d3, "proc_create" },
	{ 0xc697ac42, "seq_printf" },
	{ 0x40c7247c, "si_meminfo" },
	{ 0x1cd4c6f9, "single_open" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "310B8E969C6DF35801F8DED");
